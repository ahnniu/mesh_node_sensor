
#include <sys/printk.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include "dev_prop.h"
#include "mesh.h"
#include "sensor_srv.h"

static struct net_buf_simple *sensor_srv_msg_alloc(u16_t size);
static void sensor_srv_msg_free(struct net_buf_simple *msg);
static struct net_buf_simple *sensor_srv_status_msg_alloc(struct bt_mesh_model *model);


static void sensor_desc_response_1(struct bt_mesh_model *model,
					struct bt_mesh_msg_ctx *ctx,
					u16_t prop_id,
					struct sensor_desciptor *desc)
{
	NET_BUF_SIMPLE_DEFINE(msg, 1 + sizeof(struct sensor_desciptor));

	bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENS_DESC_STATUS);
	if (desc) {
		net_buf_simple_add_mem(&msg, desc, sizeof(struct sensor_desciptor));
	} else {
		net_buf_simple_add_le16(&msg, prop_id);
	}

	if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
		printk("Unable to send Sensor Descriptor Get response\n");
	}
}

static void sensor_desc_response(struct bt_mesh_model *model,
					struct bt_mesh_msg_ctx *ctx)
{
	struct sensor_state *state = model->user_data;
	int i, j, k;
	struct sensor *sens;
	struct sensor_prop *channel;
	u16_t size;
	struct net_buf_simple *msg;

	k = sensor_srv_props_count_get(model);
	if(!k) {
		printk("No Sensor Descriptor to send\n");
		return;
	}

	// Buf size: 1-byte(opcode 0x51), desc_count * sizeof(desc)
	size = 1 + k * sizeof(struct sensor_desciptor);
	msg = sensor_srv_msg_alloc(size);
	if (!msg) {
		printk("alloc failed for 'Sensor Descriptor Status' message\n");
		return;
	}

	bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_SENS_DESC_STATUS);
	for (i = 0; i < state->sensors_count; i++) {
		sens = state->sensor[i];
		for (j = 0; j < sens->channels_count; j++) {
			channel = sens->channel[j];
			net_buf_simple_add_mem(msg, &channel->desc, sizeof(struct sensor_desciptor));
		}
	}

	if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
		printk("Unable to send Sensor Descriptor Get response\n");
	}

	sensor_srv_msg_free(msg);
}

static void sensor_desc_get(struct bt_mesh_model *model,
			    struct bt_mesh_msg_ctx *ctx,
			    struct net_buf_simple *buf)
{
	u16_t buflen = buf->len;
	struct sensor_state *state = model->user_data;
	u16_t prop_id = 0;
	int i, j;

	if (buflen) {
		prop_id = net_buf_simple_pull_le16(buf);
	}

	if (prop_id) {
		struct sensor *sens;
		struct sensor_prop *channel;
		struct mesh_device_property *prop;

		for (i = 0; i < state->sensors_count; i++) {
			sens = state->sensor[i];
			for (j = 0; j < sens->channels_count; j++) {
				channel = sens->channel[j];
				prop = channel->prop;
				if (prop_id == prop->id->uuid) {
					return sensor_desc_response_1(model, ctx, prop_id, &channel->desc);
				}
			}
		}

		return sensor_desc_response_1(model, ctx, prop_id, NULL);
	}

	return sensor_desc_response(model, ctx);
}

static void sensor_status_buf_add_value(struct net_buf_simple *msg,
					struct mesh_device_property *prop)
{
	int i;
	struct mesh_characteristic_field *field;

	if (!msg || !prop) {
		return;
	}

	for (i = 0; i < prop->character->field_count; i++) {
		field = prop->character->field[i];
		net_buf_simple_add_mem(msg, field->value, field->size);
	}
}

static void sensor_get_response_1(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *ctx,
					 u16_t prop_id,
					 struct mesh_device_property *prop)
{
	u16_t size, raw_len;
	struct net_buf_simple *msg;

	raw_len = 0;
	if (prop) {
		raw_len = mesh_device_property_value_size_get(prop);
	}

	size = 1 + SENSOR_TLV_SIZE(prop_id) + raw_len;
	msg = sensor_srv_msg_alloc(size);
	if (!msg) {
		printk("Alloc msg failed for 'Sensor Get' response message with prop_id = 0x%4x\n",
			prop_id);
		return;
	}

	bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_SENS_STATUS);

	SENSOR_SRV_STATUS_BUF_ADD_TLV(msg, prop_id, raw_len);

	if (prop) {
		sensor_status_buf_add_value(msg, prop);
	}

	if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
		printk("Unable to send Sensor Get response\n");
	}

	sensor_srv_msg_free(msg);
}

static void sensor_status_buf_prepare(struct bt_mesh_model *model,
					struct net_buf_simple *msg)
{
	struct sensor_state *state = model->user_data;
	struct sensor *sens;
	struct sensor_prop *channel;
	struct mesh_device_property *prop;

	int i, j;

	u16_t prop_id;
	u16_t raw_len;

	bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_SENS_STATUS);

	for (i = 0; i < state->sensors_count; i++) {
		sens = state->sensor[i];
		for (j = 0; j < sens->channels_count; j++) {
			channel = sens->channel[j];
			prop = channel->prop;
			prop_id = prop->id->uuid;
			raw_len = mesh_device_property_value_size_get(prop);

			SENSOR_SRV_STATUS_BUF_ADD_TLV(msg, prop_id, raw_len);
			sensor_status_buf_add_value(msg, prop);
		}
	}
}

static void sensor_get_response(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *ctx)
{
	struct net_buf_simple *msg;

	msg = sensor_srv_status_msg_alloc(model);
	if (!msg) {
		printk("Alloc failed for 'Sensor Get' response message\n");
		return;
	}

	sensor_status_buf_prepare(model, msg);

	if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
		printk("Unable to send Sensor Get response\n");
	}

	sensor_srv_msg_free(msg);
}

static void sensor_get(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *ctx,
		       struct net_buf_simple *buf)
{
	u16_t buflen = buf->len;
	struct sensor_state *state = model->user_data;
	u16_t prop_id = 0;
	int i, j;

	if (buflen) {
		prop_id = net_buf_simple_pull_le16(buf);
	}

	if (prop_id) {
		struct sensor *sens;
		struct sensor_prop *channel;
		struct mesh_device_property *prop;

		for (i = 0; i < state->sensors_count; i++) {
			sens = state->sensor[i];
			for (j = 0; j < sens->channels_count; j++) {
				channel = sens->channel[j];
				prop = channel->prop;
				if (prop_id == prop->id->uuid) {
					return sensor_get_response_1(model, ctx, prop_id, prop);
				}
			}
		}

		return sensor_get_response_1(model, ctx, prop_id, NULL);
	}

	return sensor_get_response(model, ctx);
}

static void sensor_col_get(struct bt_mesh_model *model,
			   struct bt_mesh_msg_ctx *ctx,
			   struct net_buf_simple *buf)
{

}

static void sensor_series_get(struct bt_mesh_model *model,
			      struct bt_mesh_msg_ctx *ctx,
			      struct net_buf_simple *buf)
{

}

const struct bt_mesh_model_op sensor_srv_op[] = {
	{ BT_MESH_MODEL_OP_SENS_DESC_GET, 0, sensor_desc_get },
	{ BT_MESH_MODEL_OP_SENS_GET, 0, sensor_get },
	{ BT_MESH_MODEL_OP_SENS_COL_GET, 2, sensor_col_get },
	{ BT_MESH_MODEL_OP_SENS_SERIES_GET, 2, sensor_series_get },
	BT_MESH_MODEL_OP_END,
};


int sensor_srv_pub_msg_update(struct bt_mesh_model *model)
{
	struct net_buf_simple *msg = model->pub->msg;

	if (!msg) {
		return -ENODEV;
	}

	sensor_status_buf_prepare(model, msg);

	return 0;
}

int sensor_value_size_get(struct sensor *s)
{
	int i, k;

	if (!s) {
		return -EINVAL;
	}

	k = 0;
	for (i = 0; i < s->channels_count; i++) {
		struct mesh_device_property *p = s->channel[i]->prop;
		k += mesh_device_property_value_size_get(p);
	}

	return k;
}

int sensor_srv_props_count_get(struct bt_mesh_model *model)
{
	struct sensor_state *state = model->user_data;
	int i, j, k;

	k = 0;
	for (i = 0; i < state->sensors_count; i++) {
		struct sensor *s = state->sensor[i];
		for (j = 0; j < s->channels_count; j++) {
			k++;
		}
	}

	return k;
}

int sensor_srv_value_size_get(struct bt_mesh_model *model)
{
	struct sensor_state *state = model->user_data;
	int i, k;

	if (!model) {
		return -EINVAL;
	}

	k = 0;
	for (i = 0; i < state->sensors_count; i++) {
		struct sensor *s = state->sensor[i];
		k += sensor_value_size_get(s);
	}

	return k;
}

static struct net_buf_simple *sensor_srv_msg_alloc(u16_t size)
{
	u8_t *buf;

	if (!size) {
		return NULL;
	}

	buf = k_malloc(size);
	if (!buf) {
		return NULL;
	}

	struct net_buf_simple *msg = k_malloc(sizeof(struct net_buf_simple));
	if (!msg) {
		k_free(buf);
		return NULL;
	}

	msg->data = buf;
	msg->len = 0;
	msg->size = size;
	msg->__buf = buf;

	return msg;
}

static void sensor_srv_msg_free(struct net_buf_simple *msg)
{
	if (msg) {
		k_free(msg->__buf);
		k_free(msg);
	}
}

static struct net_buf_simple *sensor_srv_status_msg_alloc(struct bt_mesh_model *model)
{
	struct sensor_state *state;
	u16_t size, tlv_size, value_size;
	int i, j;

	if (!model) {
		return NULL;
	}

	value_size = 0;
	tlv_size = 0;
	state = model->user_data;
	for (i = 0; i < state->sensors_count; i++) {
		struct sensor *s = state->sensor[i];
		for (j = 0; j < s->channels_count; j++) {
			struct mesh_device_property *p = s->channel[i]->prop;
			tlv_size += SENSOR_TLV_SIZE(p->id->uuid);
			value_size += mesh_device_property_value_size_get(p);
		}
	}

	size = 1 + tlv_size + value_size;
	return sensor_srv_msg_alloc(size);
}

static int sensor_srv_model_pub_init(struct bt_mesh_model *model)
{
	struct net_buf_simple *pub_msg;
	struct bt_mesh_model_pub *pub;

	if (!model) {
		return -EINVAL;
	}

	if (!model->pub) {
		return -EINVAL;
	}

	pub = model->pub;
	pub_msg = sensor_srv_status_msg_alloc(model);
	if (!pub_msg) {
		return -ENOMEM;
	}

	pub->update = sensor_srv_pub_msg_update;
	pub->msg = pub_msg;

	return 0;
}

static int sensor_srv_model_pub_deinit(struct bt_mesh_model *model)
{
	if (model && model->pub && model->pub->msg) {
		sensor_srv_msg_free(model->pub->msg);
		model->pub->msg = NULL;
	}

	return 0;
}

static int sensor_srv_model_init(struct bt_mesh_model *model)
{
	return sensor_srv_model_pub_init(model);
}

static int sensor_srv_model_deinit(struct bt_mesh_model *model)
{
	return sensor_srv_model_pub_deinit(model);
}

static int sensor_srv_sensors_init(struct sensor_state *state)
{
	int i;
	int err;

	if (!state) {
		return -EINVAL;
	}

	for(i = 0; i < state->sensors_count; i++) {
		struct sensor *s = state->sensor[i];
		if (s && s->init) {
			err = s->init(s);
			if (err < 0) {
				printk("sensor[%s] init error with code: %d\n", s->name, err);
				return err;
			}
		}
	}

	return 0;
}

static int sensor_srv_sensors_deinit(struct sensor_state *state)
{
	int i;
	int err;

	if (!state) {
		return -EINVAL;
	}

	for(i = 0; i < state->sensors_count; i++) {
		struct sensor *s = state->sensor[i];
		if (s && s->deinit) {
			err = s->deinit(s);
			if (err < 0) {
				printk("sensor[%s] deinit error with code: %d\n", s->name, err);
				return err;
			}
		}
	}

	return 0;
}

int sensor_srv_init(struct bt_mesh_model *model)
{
	int err;
	struct sensor_state *state;

	if (!model) {
		return -EINVAL;
	}

	err = sensor_srv_model_init(model);
	if (err < 0) {
		return err;
	}

	state = model->user_data;
	err = sensor_srv_sensors_init(state);

	return err;
}

int sensor_srv_deinit(struct bt_mesh_model *model)
{
	int err;
	struct sensor_state *state;

	if (!model) {
		return -EINVAL;
	}

	state = model->user_data;
	err = sensor_srv_sensors_deinit(state);
	if (err < 0) {
		return err;
	}

	return sensor_srv_model_deinit(model);
}
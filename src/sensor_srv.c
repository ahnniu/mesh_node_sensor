
#include <sys/printk.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include "dev_prop.h"
#include "mesh.h"
#include "sensor_srv.h"

#define SENSOR_SRV_MAX_SENSOR_COUNT 10
#define SENSOR_SRV_MAX_DESC_COUNT   10

static struct sensor *sensors[SENSOR_SRV_MAX_SENSOR_COUNT];
struct sensor_state sensor_srv_state;

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
	struct sensor_desciptor *desc[SENSOR_SRV_MAX_DESC_COUNT];
	u8_t *net_buf_data_msg;
	u16_t net_buf_size;
	struct net_buf_simple msg;

	k = 0;
	for (i = 0; i < state->sensors_count; i++) {
		if (k >= SENSOR_SRV_MAX_DESC_COUNT) {
			break;
		}
		sens = state->sensor[i];
		for (j = 0; j < sens->channels_count; j++) {
			if (k >= SENSOR_SRV_MAX_DESC_COUNT) {
				break;
			}
			channel = sens->channel[j];
			desc[k++] = &channel->desc;
		}
	}

	if(!k) {
		printk("No Sensor Descriptor to send\n");
		return;
	}
	// Buf size: 1-byte(opcode 0x51), desc_count * sizeof(desc)
	net_buf_size = 1 + k * sizeof(struct sensor_desciptor);
	net_buf_data_msg = k_malloc(net_buf_size);
	if (!net_buf_data_msg) {
		printk("alloc mem failed for 'Sensor Descriptor Status' message\n");
		return;
	}

	msg.data = net_buf_data_msg;
	msg.len = 0;
	msg.size = net_buf_size;
	msg.__buf = net_buf_data_msg;

	bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENS_DESC_STATUS);
	for(i = 0; i < k; i++) {
		net_buf_simple_add_mem(&msg, desc[i], sizeof(struct sensor_desciptor));
	}

	if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
		printk("Unable to send Sensor Descriptor Get response\n");
	}

	if (net_buf_data_msg) {
		k_free(net_buf_data_msg);
	}
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

static void sensor_get_response_1(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *ctx,
					 u16_t prop_id,
					 struct mesh_characteristic *character)
{
	u16_t raw_len;
	struct sensor_tlv_a a;
	struct sensor_tlv_b b;
	struct mesh_characteristic_field *field;
	int i;
	u8_t *net_buf_data_msg;
	u16_t net_buf_size;
	struct net_buf_simple msg;

	raw_len = 0;
	if (character) {
		for (i = 0; i < character->field_count; i++) {
			field = character->field[i];
			raw_len += field->size;
		}
	}

	net_buf_size = 1 + sizeof(struct sensor_tlv_b) + raw_len; // 1-byte opcode + tlv + raw len
	net_buf_data_msg = k_malloc(net_buf_size);
	if (!net_buf_data_msg) {
		printk("Alloc mem failed for 'Sensor Get' response message with prop_id = 0x%4x\n",
			prop_id);
		return;
	}

	msg.data = net_buf_data_msg;
	msg.len = 0;
	msg.size = net_buf_size;
	msg.__buf = net_buf_data_msg;

	bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENS_STATUS);

	if (SENSOR_IS_TLV_A(prop_id)) {
		a.format = SENSOR_DATA_FORMAT_A;
		a.length = raw_len;
		a.prop_id = prop_id;
		net_buf_simple_add_mem(&msg, &a, sizeof(struct sensor_tlv_a));
	} else {
		b.format = SENSOR_DATA_FORMAT_B;
		b.length = raw_len;
		b.prop_id = prop_id;
		net_buf_simple_add_mem(&msg, &b, sizeof(struct sensor_tlv_b));
	}

	if (character) {
		for (i = 0; i < character->field_count; i++) {
			field = character->field[i];
			net_buf_simple_add_mem(&msg, field->value, field->size);
		}
	}

	if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
		printk("Unable to send Sensor Get response\n");
	}

	if (net_buf_data_msg) {
		k_free(net_buf_data_msg);
	}
}

static void sensor_get_response(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *ctx)
{
	struct sensor_state *state = model->user_data;
	struct sensor *sens;
	struct sensor_prop *channel;
	struct mesh_characteristic *character;
	struct mesh_characteristic_field *field;

	struct sensor_tlv_a a;
	struct sensor_tlv_b b;

	int i, j, k;

	u16_t prop_id;
	u16_t raw_len, prop_count;
	u8_t *net_buf_data_msg;
	u16_t net_buf_size;
	struct net_buf_simple msg;

	raw_len = 0;
	prop_count = 0;
	for (i = 0; i < state->sensors_count; i++) {
		sens = state->sensor[i];
		for (j = 0; j < sens->channels_count; j++) {
			prop_count += 1;
			channel = sens->channel[j];
			character = channel->prop->character;
			for (k = 0; k < character->field_count; k++) {
				field = character->field[k];
				raw_len += field->size;
			}
		}
	}

	// 1-byte opcode + prop_count * tlv + raw_len
	net_buf_size = 1 + prop_count * sizeof(struct sensor_tlv_b) + raw_len;
	net_buf_data_msg = k_malloc(net_buf_size);
	if (!net_buf_data_msg) {
		printk("Alloc mem failed for 'Sensor Get' response message\n");
		return;
	}

	msg.data = net_buf_data_msg;
	msg.len = 0;
	msg.size = net_buf_size;
	msg.__buf = net_buf_data_msg;

	bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENS_STATUS);

	for (i = 0; i < state->sensors_count; i++) {
		sens = state->sensor[i];
		for (j = 0; j < sens->channels_count; j++) {
			channel = sens->channel[j];
			character = channel->prop->character;
			prop_id = character->id->uuid;

			if (SENSOR_IS_TLV_A(prop_id)) {
				a.format = SENSOR_DATA_FORMAT_A;
				a.length = raw_len;
				a.prop_id = prop_id;
				net_buf_simple_add_mem(&msg, &a, sizeof(struct sensor_tlv_a));
			} else {
				b.format = SENSOR_DATA_FORMAT_B;
				b.length = raw_len;
				b.prop_id = prop_id;
				net_buf_simple_add_mem(&msg, &b, sizeof(struct sensor_tlv_b));
			}

			for (k = 0; k < character->field_count; k++) {
				field = character->field[k];
				net_buf_simple_add_mem(&msg, field->value, field->size);
			}
		}
	}

	if (bt_mesh_model_send(model, ctx, &msg, NULL, NULL)) {
		printk("Unable to send Sensor Get response\n");
	}

	if (net_buf_data_msg) {
		k_free(net_buf_data_msg);
	}
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
					return sensor_get_response_1(model, ctx, prop_id, prop->character);
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

static void sensor_srv_state_init()
{
	int i;

	for(i = 0; i < SENSOR_SRV_MAX_SENSOR_COUNT; i++) {
		sensors[i] = NULL;
	}
	sensor_srv_state.sensors_count = 0;
	sensor_srv_state.sensor = sensors;
}

int sensor_srv_register(struct sensor *sens)
{
	int i = sensor_srv_state.sensors_count;

	if (!sens) {
		return -EINVAL;
	}

	if (i < SENSOR_SRV_MAX_SENSOR_COUNT - 1) {
		sensors[i++] = sens;
	} else {
		return -ENOMEM;
	}

	return 0;
}

int sensor_srv_init()
{
	int err;

	sensor_srv_state_init();

	// Sensor registers
	err = pat_register();

	if (err < 0) {
		return err;
	}

	return 0;
}
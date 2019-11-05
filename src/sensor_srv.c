
#include <sys/printk.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include "dev_prop.h"
#include "mesh.h"
#include "sensor_srv.h"

MESH_CHARACTER_FIELD_DEFINE(temperature_8, temperature_8, 1);
MESH_CHARACTER_1_DEFINE(temperature_8, temperature_8);
MESH_DEVICE_PROPERTY_DEFINE(present_ambient_temperature, temperature_8);

static struct mesh_device_property *sensor_props[] = {
	&present_ambient_temperature
};

struct sensor_state sensor_srv_state = {
	.prop = sensor_props,
	.prop_count = ARRAY_SIZE(sensor_props)
};

static void sensor_desc_get(struct bt_mesh_model *model,
			    struct bt_mesh_msg_ctx *ctx,
			    struct net_buf_simple *buf)
{

}

static void sensor_get(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *ctx,
		       struct net_buf_simple *buf)
{

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

void sensor_srv_init()
{

}
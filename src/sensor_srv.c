
#include <sys/printk.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include "dev_prop.h"
#include "mesh.h"
#include "sensor_srv.h"

#define SENSOR_SRV_MAX_SENSOR_COUNT 10

static struct sensor *sensors[SENSOR_SRV_MAX_SENSOR_COUNT];
struct sensor_state sensor_srv_state;

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
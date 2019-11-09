#ifndef ZEPHYR_INCLUDE_APP_SENSOR_SRV_H_
#define ZEPHYR_INCLUDE_APP_SENSOR_SRV_H_

#include <zephyr.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>

#include "dev_prop.h"

enum sensor_data_format {
	SENSOR_DATA_FORMAT_A = 0,
	SENSOR_DATA_FORMAT_B = 1
};

struct sensor_tlv_a {
	u16_t format:1;
	u16_t length:4;
	u16_t prop_id:11;
} __packed;

struct sensor_tlv_b {
	u8_t format:1;
	u8_t length:7;
	u16_t prop_id:11;
} __packed;

struct sensor_desciptor {
	u16_t prop_id;
	u16_t positive_tolerance:12;
	u16_t negative_tolerance:12;
	u8_t sampling_function;
	u8_t measurement_period;
	u8_t update_interval;
} __packed;

struct sensor_prop {
	struct mesh_device_property *prop;
	struct sensor_desciptor desc;
};

struct sensor {
	const char* name;
	struct sensor_prop **channel;
	int channels_count;
	void *dev;
	int (*init)(struct sensor *s);
	int (*deinit)(struct sensor *s);
};

struct sensor_state {
	struct sensor **sensor;
	int sensors_count;
};

#define SENSOR_IS_TLV_A(_prop_id) (_prop_id < 2048)
#define SENSOR_IS_TLV_B(_prop_id) (!SENSOR_IS_TLV_A(_prop_id))

#define SENSOR_SRV_STATE_DEFINE(_name, _sensors)                    \
	static struct sensor_state _name = {                              \
		.sensor = _sensors,                                             \
		.sensors_count = ARRAY_SIZE(_sensors)                           \
	}

#define SENSOR_SRV_MODEL_PUB_DEFINE(_name)                          \
	static struct bt_mesh_model_pub _name = {                         \
	}

#define SENOSR_SRV_MODEL_DATA_DEFINE(_name, _sensors)               \
	SENSOR_SRV_STATE_DEFINE(_CONCAT(sensor_srv_state_, _name),        \
		_sensors);                                                      \
	SENSOR_SRV_MODEL_PUB_DEFINE(_CONCAT(sensor_srv_pub_, _name))

#define SENSOR_SRV_MODEL(_name)                                     \
	BT_MESH_MODEL(                                                    \
		BT_MESH_MODEL_ID_SENSOR_SRV,                                    \
		sensor_srv_op,                                                  \
		&_CONCAT(sensor_srv_pub_, _name),                               \
		&_CONCAT(sensor_srv_state_, _name)                              \
	)

#define SENSOR_SRV_MODEL_DEFINE(_name, _sensors)                    \
	static struct sensor_state _CONCAT(sensor_srv_state_, _name) = {  \
		.sensor = _sensors,                                             \
		.sensors_count = ARRAY_SIZE(_sensors)                           \
	};                                                                \
	static struct bt_mesh_model _name =                               \
		BT_MESH_MODEL(                                                  \
			BT_MESH_MODEL_ID_SENSOR_SRV,                                  \
			NULL,                                                         \
			NULL,                                                         \
			&_CONCAT(sensor_srv_state_, _name),                           \
		)                                                               \

extern const struct bt_mesh_model_op sensor_srv_op[];

int sensor_value_size_get(struct sensor *s);
int sensor_srv_props_count_get(struct bt_mesh_model *model);

int sensor_srv_init(struct bt_mesh_model *model);
int sensor_srv_deinit(struct bt_mesh_model *model);

int sensor_srv_pub_msg_update(struct bt_mesh_model *model);

#endif
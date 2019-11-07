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
	struct sensor_prop **channel;
	int channels_count;
	void *dev;
};

struct sensor_state {
	struct sensor **sensor;
	int sensors_count;
};

#define SENSOR_IS_TLV_A(_prop_id) (_prop_id < 2048)
#define SENSOR_IS_TLV_B(_prop_id) (!SENSOR_IS_TLV_A(_prop_id))

extern const struct bt_mesh_model_op sensor_srv_op[];
extern struct sensor_state sensor_srv_state;

int sensor_srv_register(struct sensor *sens);
int sensor_srv_init();

int pat_register();

#endif
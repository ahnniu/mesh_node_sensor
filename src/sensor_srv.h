#ifndef ZEPHYR_INCLUDE_APP_SENSOR_SRV_H_
#define ZEPHYR_INCLUDE_APP_SENSOR_SRV_H_

#include <zephyr.h>
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

struct sensor_state {
	struct mesh_device_property **prop;
	int prop_count;
};

extern const struct bt_mesh_model_op sensor_srv_op[];
extern struct sensor_state sensor_srv_state;

void sensor_srv_init();

#endif
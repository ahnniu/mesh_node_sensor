#include <minode.h>

#include "sensor_srv.h"

static void temp_on_new_sampling(struct minode_dht_device *dev);

static struct sensor pat_sensor;

MESH_CHARACTER_FIELD_DEFINE(temperature_8, temperature_8, 1);
MESH_CHARACTER_1_DEFINE(temperature_8, temperature_8);
MESH_DEVICE_PROPERTY_DEFINE(present_ambient_temperature, temperature_8);

MINODE_DHT_DEVICE_DEFINE(temp_sensor_dev, A0, &pat_sensor, temp_on_new_sampling);

static struct sensor pat_sensor = {
	.prop = &present_ambient_temperature,
	.dev = &temp_sensor_dev
};

static void temp_on_new_sampling(struct minode_dht_device *dev)
{

}

int pat_register()
{
	int err;

	err = minode_dht_init(&temp_sensor_dev);
	if (err < 0) {
		return err;
	}

	minode_dht_start_sampling(&temp_sensor_dev, 1000 * 60);

	pat_sensor.desc.prop_id = pat_sensor.prop->id->uuid;
	pat_sensor.desc.positive_tolerance = 0;
	pat_sensor.desc.negative_tolerance = 0;
	pat_sensor.desc.measurement_period = 88;	// 10s
	pat_sensor.desc.update_interval = 107;		// 60s
	pat_sensor.desc.sampling_function = 1;		// Instantaneous

	return sensor_srv_register(&pat_sensor);
}
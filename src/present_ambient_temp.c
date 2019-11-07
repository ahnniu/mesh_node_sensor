#include <minode.h>

#include "sensor_srv.h"

static void temp_on_new_sampling(struct minode_dht_device *dev);

static struct sensor pat_sensor;

MESH_CHARACTER_FIELD_DEFINE(temperature_8, temperature_8, sint8);
MESH_CHARACTER_1_DEFINE(temperature_8, temperature_8);
MESH_DEVICE_PROPERTY_DEFINE(present_ambient_temperature, temperature_8);

MINODE_DHT_DEVICE_DEFINE(temp_sensor_dev, A0, &pat_sensor, temp_on_new_sampling);

static struct sensor_prop pat_sensor_prop = {
	.prop = &present_ambient_temperature,
};

static struct sensor_prop *channels[] = {
	&pat_sensor_prop
};
static struct sensor pat_sensor = {
	.channel = channels,
	.channels_count = ARRAY_SIZE(channels),
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

	pat_sensor_prop.desc.prop_id = pat_sensor_prop.prop->id->uuid;
	pat_sensor_prop.desc.positive_tolerance = 0;
	pat_sensor_prop.desc.negative_tolerance = 0;
	pat_sensor_prop.desc.measurement_period = 88;		// 10s
	pat_sensor_prop.desc.update_interval = 107;			// 60s
	pat_sensor_prop.desc.sampling_function = 1;			// Instantaneous

	return sensor_srv_register(&pat_sensor);
}
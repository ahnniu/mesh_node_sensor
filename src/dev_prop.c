#include <zephyr.h>
#include <sys/printk.h>
#include "dev_prop.h"

MESH_DEVICE_PROPERTY_ID_DEFINE(
	present_ambient_temperature,
	org.bluetooth.property.present_ambient_temperature,
	0x004F
);

MESH_CHARACTER_ID_DEFINE(
	temperature_8,
	org.bluetooth.characteristic.temperature_8,
	0x2B0D
);

MESH_CHARACTER_TYPE_DEFINE(temperature_8, sint8, 0, -1, 1);


int mesh_characteristic_value_size_get(struct mesh_characteristic *c)
{
	int i, k;

	if (!c) {
		return -EINVAL;
	}

	k = 0;
	for(i = 0; i < c->field_count; i++) {
		k += c->field[i]->size;
	}

	return k;
}

int mesh_device_property_value_size_get(struct mesh_device_property *p)
{
	if (!p) {
		return -EINVAL;
	}

	return mesh_characteristic_value_size_get(p->character);
}


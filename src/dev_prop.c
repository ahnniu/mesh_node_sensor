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

MESH_CHARACTER_TYPE_DEFINE(temperature_8, 0, -1, 1, 1);
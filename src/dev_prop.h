#ifndef ZEPHYR_INCLUDE_APP_DEV_PROP_H_
#define ZEPHYR_INCLUDE_APP_DEV_PROP_H_

#include <zephyr.h>
#include "format_types.h"

struct mesh_characteristic_id {
	const char *name;
	const char *type;		// org.bluetooth.characteristic.temperature_8
	u16_t uuid;
};

struct mesh_characteristic_type {
	struct mesh_characteristic_id *id;
	const char *format; // sint8
	// const char *unit;   // org.bluetooth.unit.thermodynamic_temperature.degree_celsius
	// const char *mini;
	// const char *max;
	int d;							// DecimalExponent
	int b;							// BinaryExponent
	int m;							// Multipler
};

struct mesh_characteristic_field {
	const char *name;
	struct mesh_characteristic_type *type;
	u8_t *value;
	int size;
	int bits;
};

struct mesh_characteristic {
	struct mesh_characteristic_id *id;
	struct mesh_characteristic_field **field;
	int field_count;
};

struct mesh_device_property_id {
	const char *name;
	const char *type;
	u16_t uuid;
};

struct mesh_device_property {
	struct mesh_device_property_id *id;
	struct mesh_characteristic *character;
};

#define MESH_CHARACTER_ID_DEFINE(_name, _type, _uuid)             \
	struct mesh_characteristic_id _CONCAT(mci_,_name) = {           \
		.name = STRINGIFY(_name),                                     \
		.type = STRINGIFY(_type),                                     \
		.uuid = _uuid                                                 \
	}

#define MESH_CHARACTER_ID_DECLARE(_name)                          \
	extern struct mesh_characteristic_id _CONCAT(mci_, _name)

#define MESH_CHARACTER_TYPE_DEFINE(_name, _format, _d, _b, _m)    \
	struct mesh_characteristic_type _CONCAT(mct_, _name) = {        \
		.id = &_CONCAT(mci_,_name),                                   \
		.d = _d,                                                      \
		.b = _b,                                                      \
		.m = _m,                                                      \
		.format = STRINGIFY(_format)                                  \
	}

#define MESH_CHARACTER_TYPE_DECLARE(_name)                        \
	extern struct mesh_characteristic_type _CONCAT(mct_, _name)

#define MESH_CHARACTER_FIELD_DEFINE(_name, _type_name, _format)           \
	static __used u8_t mc_field_value_##_name[MESH_FORMAT_SIZE(_format)];   \
	static struct mesh_characteristic_field mc_field_##_name = {            \
		.name = STRINGIFY(_name),                                             \
		.type = &_CONCAT(mct_, _type_name),                                   \
		.value = mc_field_value_##_name,                                      \
		.size = MESH_FORMAT_SIZE(_format),                                    \
		.bits = MESH_FORMAT_BITS(_format)                                     \
	}

#define MESH_CHARACTER_FIELD(_name) _CONCAT(mc_field_, _name)

#define MESH_CHARACTER_DEFINE(_name, _fields)                     \
	static struct mesh_characteristic _name = {                     \
		.id = &_CONCAT(mci_, _name),                                  \
		.field = _fields,                                             \
		.field_count = ARRAY_SIZE(_fields)                            \
	}

#define _MESH_CHARACTER_DEFINE(_name)                             \
	static struct mesh_characteristic _name = {                     \
		.id = &_CONCAT(mci_,_name),                                   \
		.field = _CONCAT(mc_fields_, _name),                          \
		.field_count = ARRAY_SIZE(_CONCAT(mc_fields_, _name))         \
	}

#define _MESH_CHARACTER_FIELDS_NAME(_name) _CONCAT(mc_fields_, _name)
#define _MESH_CHARACTER_FIELD(_field_name) &_CONCAT(mc_field_, _field_name)

#define MESH_CHARACTER_DEFINE_EX(_name, _fields_array_in_braces)                    \
	static struct mesh_characteristic_field *_MESH_CHARACTER_FIELDS_NAME(_name)[] =   \
		_fields_array_in_braces;                                                        \
	_MESH_CHARACTER_DEFINE(_name)

#define MESH_CHARACTER_1_DEFINE(_name, _field0_name)                                \
	static struct mesh_characteristic_field *_MESH_CHARACTER_FIELDS_NAME(_name)[] = { \
		_MESH_CHARACTER_FIELD(_field0_name)                                             \
	};                                                                                \
	_MESH_CHARACTER_DEFINE(_name)

#define MESH_CHARACTER_2_DEFINE(_name, _field0_name, _field1_name)                  \
	static struct mesh_characteristic_field *_MESH_CHARACTER_FIELDS_NAME(_name)[] = { \
		_MESH_CHARACTER_FIELD(_field0_name),                                            \
		_MESH_CHARACTER_FIELD(_field1_name)                                             \
	};                                                                                \
	_MESH_CHARACTER_DEFINE(_name)

#define MESH_CHARACTER_3_DEFINE(_name, _field0_name, _field1_name, _field2_name)    \
	static struct mesh_characteristic_field *_MESH_CHARACTER_FIELDS_NAME(_name)[] = { \
		_MESH_CHARACTER_FIELD(_field0_name),                                            \
		_MESH_CHARACTER_FIELD(_field1_name),                                            \
		_MESH_CHARACTER_FIELD(_field2_name)                                             \
	};                                                                                \
	_MESH_CHARACTER_DEFINE(_name)

#define MESH_DEVICE_PROPERTY_ID_DEFINE(_name, _type, _uuid)       \
	struct mesh_device_property_id _CONCAT(mdpi_,_name) = {         \
		.name = STRINGIFY(_name),                                     \
		.type = STRINGIFY(_type),                                     \
		.uuid = _uuid                                                 \
	}

#define MESH_DEVICE_PROPERTY_ID_DECLARE(_name)                    \
	extern struct mesh_device_property_id _CONCAT(mdpi_, _name)

#define MESH_DEVICE_PROPERTY_DEFINE(_name, _character_name)       \
	static struct mesh_device_property _name = {                    \
		.id = &_CONCAT(mdpi_, _name),                                 \
		.character = &_character_name                                 \
	}


int mesh_characteristic_value_size_get(struct mesh_characteristic *c);
int mesh_device_property_value_size_get(struct mesh_device_property *p);

// Mesh Device Property ID declares
MESH_DEVICE_PROPERTY_ID_DECLARE(present_ambient_temperature);

// Mesh Characteristic ID declares
MESH_CHARACTER_ID_DECLARE(temperature_8);

// Mesh Characteristic base type declares
MESH_CHARACTER_TYPE_DECLARE(temperature_8);

#endif
#ifndef ZEPHYR_INCLUDE_APP_FORMAT_TYPES_H_
#define ZEPHYR_INCLUDE_APP_FORMAT_TYPES_H_

#define MESH_FORMAT_SIZE(format) _CONCAT(MESH_FORMAT_SIZE_, format)
#define MESH_FORMAT_BITS(format) _CONCAT(MESH_FORMAT_BITS_, format)

#define MESH_FORMAT_SIZE_boolean      1
#define MESH_FORMAT_BITS_boolean      1

#define MESH_FORMAT_SIZE_2bit         1
#define MESH_FORMAT_BITS_2bit         2

#define MESH_FORMAT_SIZE_sint8        1
#define MESH_FORMAT_BITS_sint8        8

#define MESH_FORMAT_SIZE_uint8        1
#define MESH_FORMAT_BITS_uint8        8

#define MESH_FORMAT_SIZE_sint12       2
#define MESH_FORMAT_BITS_sint12       12

#define MESH_FORMAT_SIZE_uint12       2
#define MESH_FORMAT_BITS_uint12       12

#define MESH_FORMAT_SIZE_sint16       2
#define MESH_FORMAT_BITS_sint16       16

#define MESH_FORMAT_SIZE_uint16       2
#define MESH_FORMAT_BITS_sint16       16

#define MESH_FORMAT_SIZE_sint24       3
#define MESH_FORMAT_BITS_sint24       24

#define MESH_FORMAT_SIZE_uint24       3
#define MESH_FORMAT_BITS_uint24       24

#define MESH_FORMAT_SIZE_sint32       4
#define MESH_FORMAT_BITS_sint32       32

#define MESH_FORMAT_SIZE_uint32       4
#define MESH_FORMAT_BITS_uint32       32

#define MESH_FORMAT_SIZE_sint48       6
#define MESH_FORMAT_BITS_sint48       48

#define MESH_FORMAT_SIZE_uint48       6
#define MESH_FORMAT_BITS_uint48       48

#define MESH_FORMAT_SIZE_sint64       8
#define MESH_FORMAT_BITS_sint64       64

#define MESH_FORMAT_SIZE_uint64       8
#define MESH_FORMAT_BITS_uint64       64

#endif

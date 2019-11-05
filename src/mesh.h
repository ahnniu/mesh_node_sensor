#ifndef ZEPHYR_INCLUDE_APP_MESH_H_
#define ZEPHYR_INCLUDE_APP_MESH_H_

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>

#define BT_MESH_MODEL_OP_GEN_ONOFF_GET		BT_MESH_MODEL_OP_2(0x82, 0x01)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET		BT_MESH_MODEL_OP_2(0x82, 0x02)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK	BT_MESH_MODEL_OP_2(0x82, 0x03)
#define BT_MESH_MODEL_OP_GEN_ONOFF_STATUS	BT_MESH_MODEL_OP_2(0x82, 0x04)

#define BT_MESH_MODEL_OP_SENS_DESC_GET		BT_MESH_MODEL_OP_2(0x82, 0x30)
#define BT_MESH_MODEL_OP_SENS_GET		BT_MESH_MODEL_OP_2(0x82, 0x31)
#define BT_MESH_MODEL_OP_SENS_COL_GET		BT_MESH_MODEL_OP_2(0x82, 0x32)
#define BT_MESH_MODEL_OP_SENS_SERIES_GET	BT_MESH_MODEL_OP_2(0x82, 0x33)

#define BT_MESH_MODEL_OP_SENS_DESC_STATUS	BT_MESH_MODEL_OP_1(0x51)
#define BT_MESH_MODEL_OP_SENS_STATUS		BT_MESH_MODEL_OP_1(0x52)

extern u16_t primary_addr;

extern struct bt_mesh_model root_models[];
extern struct bt_mesh_elem elements[];
extern const struct bt_mesh_comp comp;

int mesh_init();

#endif
#ifndef ZEPHYR_INCLUDE_APP_BOARD_H_
#define ZEPHYR_INCLUDE_APP_BOARD_H_

void board_init(void);

void board_output_number(bt_mesh_output_action_t action, u32_t number);
void board_prov_complete(void);
void blink_a_led(void);

#endif

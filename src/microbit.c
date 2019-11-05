/* microbit.c - BBC micro:bit specific hooks */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <drivers/gpio.h>

#include <display/mb_display.h>

#include <bluetooth/mesh.h>
#include <mb_button.h>
#include "board.h"


static u32_t oob_number;
static int is_prov_completed = 0;
static struct mb_button_device *btn_a;

static void btn_a_pressed(struct mb_button_device *dev, void *user_data)
{
	struct mb_display *disp = mb_display_get();

	if(!is_prov_completed) {
		mb_display_print(disp, MB_DISPLAY_MODE_DEFAULT, K_MSEC(500),
				"%04u", oob_number);
	} else {
		bt_mesh_proxy_identity_enable();
		blink_a_led();
	}
}

static void configure_button(void)
{
	btn_a = mb_button_device_get(DT_ALIAS_SW0_LABEL);
	if (!btn_a) {
		printk("cannot find %s", DT_ALIAS_SW0_LABEL);
	}

	mb_button_init(btn_a, btn_a_pressed, NULL, false);
}

void board_output_number(bt_mesh_output_action_t action, u32_t number)
{
	struct mb_display *disp = mb_display_get();
	struct mb_image arrow = MB_IMAGE({ 0, 0, 1, 0, 0 },
					 { 0, 1, 0, 0, 0 },
					 { 1, 1, 1, 1, 1 },
					 { 0, 1, 0, 0, 0 },
					 { 0, 0, 1, 0, 0 });

	oob_number = number;

	mb_button_enable_callback(btn_a);

	mb_display_image(disp, MB_DISPLAY_MODE_DEFAULT, K_FOREVER, &arrow, 1);
}

void board_prov_complete(void)
{
	struct mb_display *disp = mb_display_get();
	struct mb_image arrow = MB_IMAGE({ 0, 1, 0, 1, 0 },
					 { 0, 1, 0, 1, 0 },
					 { 0, 0, 0, 0, 0 },
					 { 1, 0, 0, 0, 1 },
					 { 0, 1, 1, 1, 0 });

	is_prov_completed = 1;

	mb_display_image(disp, MB_DISPLAY_MODE_DEFAULT, K_SECONDS(10),
			 &arrow, 1);
}

void board_init(void)
{
	struct mb_display *disp = mb_display_get();
	static struct mb_image blink[] = {
		MB_IMAGE({ 1, 1, 1, 1, 1 },
			 { 1, 1, 1, 1, 1 },
			 { 1, 1, 1, 1, 1 },
			 { 1, 1, 1, 1, 1 },
			 { 1, 1, 1, 1, 1 }),
		MB_IMAGE({ 0, 0, 0, 0, 0 },
			 { 0, 0, 0, 0, 0 },
			 { 0, 0, 0, 0, 0 },
			 { 0, 0, 0, 0, 0 },
			 { 0, 0, 0, 0, 0 })
	};

	mb_display_image(disp, MB_DISPLAY_MODE_DEFAULT | MB_DISPLAY_FLAG_LOOP,
			 K_SECONDS(1), blink, ARRAY_SIZE(blink));

	configure_button();
}

void blink_a_led(void)
{
	struct mb_display *disp = mb_display_get();
	static struct mb_image blink[] = {
		MB_IMAGE({ 0, 0, 0, 0, 0 },
			 { 0, 0, 0, 0, 0 },
			 { 0, 0, 1, 0, 0 },
			 { 0, 0, 0, 0, 0 },
			 { 0, 0, 0, 0, 0 }),
		MB_IMAGE({ 0, 0, 0, 0, 0 },
			 { 0, 0, 0, 0, 0 },
			 { 0, 0, 0, 0, 0 },
			 { 0, 0, 0, 0, 0 },
			 { 0, 0, 0, 0, 0 })
	};

	mb_display_image(disp, MB_DISPLAY_MODE_DEFAULT | MB_DISPLAY_FLAG_LOOP,
			 K_SECONDS(1), blink, ARRAY_SIZE(blink));

}
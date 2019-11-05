/* main.c - Application main entry point */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include "mesh.h"

void main(void)
{
	int err;

	printk("Initializing...\n");

	err = mesh_init();
	if (err) {
		printk("Bluetooth mesh init failed (err %d)\n", err);
	}
}

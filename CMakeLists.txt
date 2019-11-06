# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

set(BOARD bbc_microbit)
set(SHIELD element14_minode)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(sensor)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/dev_prop.c)
target_sources(app PRIVATE src/sensor_srv.c)
target_sources(app PRIVATE src/microbit.c)
target_sources(app PRIVATE src/mesh.c)
target_sources(app PRIVATE src/present_ambient_temp.c)
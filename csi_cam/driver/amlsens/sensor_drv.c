// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2023 Amlogic, Inc. All rights reserved.
 */
#include <linux/version.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <media/media-entity.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-fwnode.h>
#include <media/v4l2-subdev.h>
#include <linux/of_platform.h>
#include <linux/of_graph.h>

#include "sensor_drv.h"

#include "ov5640/aml_ov5640.h"

struct sensor_subdev sd_ov5640 = {
	.sensor_init = ov5640_init,
	.sensor_deinit = ov5640_deinit,
	.sensor_get_id = ov5640_sensor_id,
	.sensor_power_on = ov5640_power_on,
	.sensor_power_off = ov5640_power_off,
	.sensor_power_suspend = ov5640_power_suspend,
	.sensor_power_resume = ov5640_power_resume,
};


struct sensor_subdev *aml_sensors[] = {
	&sd_ov5640,
};



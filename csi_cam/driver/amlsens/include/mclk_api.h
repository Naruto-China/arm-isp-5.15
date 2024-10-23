// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2023 Amlogic, Inc. All rights reserved.
 */
#ifndef __MCLK_API__
#define __MCLK_API__
#include <linux/version.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
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
#include <linux/of_gpio.h>

#include "aml_misc.h"

static inline int mclk_enable(struct device *dev, uint32_t rate)
{
	struct clk *mclk0 = NULL;
	int clk_val;
	int ret;

	mclk0 = devm_clk_get(dev, "mclk0");
	if (IS_ERR_OR_NULL(mclk0)) {
		pr_err("cannot get %s\n", "mclk0");
		mclk0 = NULL;
		return -1;
	}

	ret = clk_set_rate(mclk0, rate);
	if (ret < 0)
		dev_err(dev, "clk_set_rate failed\n");
	usleep_range(30, 40);

	ret = clk_prepare_enable(mclk0);
	if (ret < 0)
		dev_err(dev, " clk_prepare_enable failed\n");

	clk_val = clk_get_rate(mclk0);
	devm_clk_put(dev, mclk0);

	dev_dbg(dev, "std clk api - mclk is %d MHZ\n", clk_val / 1000000);
	return 0;
}

static inline int mclk_disable(struct device *dev)
{
	struct clk *mclk0 = NULL;

	mclk0 = devm_clk_get(dev, "mclk0");
	if (IS_ERR_OR_NULL(mclk0)) {
		pr_err("cannot get %s clk\n", "mclk0");
		mclk0 = NULL;
		return -1;
	}

	if (__clk_is_enabled(mclk0))
		clk_disable_unprepare(mclk0);

	devm_clk_put(dev, mclk0);

	dev_dbg(dev, "disable mclk\n");
	return 0;
}

static inline int reset_am_enable(struct device *dev, const char* propname, int val)
{
	int ret = -1;

	int reset = of_get_named_gpio(dev->of_node, propname, 0);
	ret = reset;

	if (ret >= 0) {
		devm_gpio_request(dev, reset, "RESET");
		if (gpio_is_valid(reset)) {
			gpio_direction_output(reset, val);
			pr_info("reset init\n");
		} else {
			pr_err("reset_enable: gpio %s is not valid\n", propname);
			return -1;
		}
	} else {
		pr_err("reset_enable: get_named_gpio %s fail\n", propname);
	}

	return ret;
}
static inline int pwdn_am_enable(struct device *dev, const char* propname, int val)
{
	int ret = -1;

	int reset = of_get_named_gpio(dev->of_node, propname, 0);
	ret = reset;

	if (ret >= 0) {
		devm_gpio_request(dev, reset, "PWDN");
		if (gpio_is_valid(reset)) {
			gpio_direction_output(reset, val);
			pr_info("reset init\n");
		} else {
			pr_err("reset_enable: gpio %s is not valid\n", propname);
			return -1;
		}
	} else {
		pr_err("reset_enable: get_named_gpio %s fail\n", propname);
	}

	return ret;
}
#endif

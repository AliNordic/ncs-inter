/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT blink_gpio_led

#include <zephyr/device.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <blink.h>

LOG_MODULE_REGISTER(blink_gpio_led, CONFIG_BLINK_LOG_LEVEL);

/* STEP 3.1 Define data structure */


/* STEP 3.2 Define configuration structure */



static void blink_gpio_led_on_timer_expire(struct k_timer *timer)
{
	const struct device *dev = k_timer_user_data_get(timer);
	const struct blink_gpio_led_config *config = dev->config;
	int ret;

	ret = gpio_pin_toggle_dt(&config->led);
	if (ret < 0) {
		LOG_ERR("Could not toggle LED GPIO (%d)", ret);
	}
}


static int blink_gpio_led_set_period_ms(const struct device *dev,
					unsigned int period_ms)
{
	const struct blink_gpio_led_config *config = dev->config;
	struct blink_gpio_led_data *data = dev->data;

	if (period_ms == 0) {
		k_timer_stop(&data->timer);
		return gpio_pin_set_dt(&config->led, 0);
	}

	k_timer_start(&data->timer, K_MSEC(period_ms), K_MSEC(period_ms));

	return 0;
}

/* STEP 3.3 Assign set perdiod function to drivers API*/


static int blink_gpio_led_init(const struct device *dev)
{
	const struct blink_gpio_led_config *config = dev->config;
	struct blink_gpio_led_data *data = dev->data;
	int ret;

	if (!gpio_is_ready_dt(&config->led)) {
		LOG_ERR("LED GPIO not ready");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		LOG_ERR("Could not configure LED GPIO (%d)", ret);
		return ret;
	}

	k_timer_init(&data->timer, blink_gpio_led_on_timer_expire, NULL);
	k_timer_user_data_set(&data->timer, (void *)dev);

	if (config->period_ms > 0) {
		k_timer_start(&data->timer, K_MSEC(config->period_ms),
			      K_MSEC(config->period_ms));
	}

	return 0;
}

#define BLINK_GPIO_LED_DEFINE(inst)                                       \
    /* STEP 4.1 Create data structure instance template*/				 
                                                                           
	/* STEP 4.2 Create configuration structure instance template */		   
                                                                            
    /*STEP 4.3 Declare device definition template */				 


DT_INST_FOREACH_STATUS_OKAY(BLINK_GPIO_LED_DEFINE)

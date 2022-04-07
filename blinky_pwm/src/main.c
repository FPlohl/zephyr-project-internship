/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM.
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/pwm.h>


#define PWM_LED0_NODE	DT_ALIAS(pwm_led0)
#define PWM_CTLR	DT_PWMS_CTLR(PWM_LED0_NODE)
#define PWM_CHANNEL	DT_PWMS_CHANNEL(PWM_LED0_NODE)
#define PWM_FLAGS	DT_PWMS_FLAGS(PWM_LED0_NODE)

void main(void)
{
	const struct device *pwm;
	uint32_t period;
	
	printk("PWM-based blinky\n");

	pwm = DEVICE_DT_GET(PWM_CTLR);
	if (!device_is_ready(pwm)) {
		printk("Error: PWM device %s is not ready\n", pwm->name);
		return;
	}

	period = 8000;
	uint32_t pwm_percent = 0;
	bool updown = true;
	while (1) {
		if (pwm_percent == 0) updown = true;
		else if (pwm_percent == 100) updown = false;
		pwm_pin_set_usec(pwm, PWM_CHANNEL, period, pwm_percent * period/100, PWM_FLAGS);
		//printk("Pwm precent %d", pwm_percent);
		if (updown == true) pwm_percent++;
		else if (updown == false) pwm_percent--;
		k_sleep(K_MSEC(10U));
	}
}

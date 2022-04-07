#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/printk.h>
#include <sys/__assert.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

static struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios,{0});
static struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios,{0});

void blink0(void)
{
	//blink(&led0, 100, 0);
	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	while(1) {
		gpio_pin_toggle_dt(&led0);
		k_msleep(100);
	}
}

void blink1(void)
{
	gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);
	while(1) {
		gpio_pin_toggle_dt(&led1);
		k_msleep(1000);
	}
}

K_THREAD_DEFINE(blink0_id, STACKSIZE, blink0, NULL, NULL, NULL,
		PRIORITY, 0, 0);
K_THREAD_DEFINE(blink1_id, STACKSIZE, blink1, NULL, NULL, NULL,
		PRIORITY, 0, 0);

#include <zephyr.h>
#include <drivers/gpio.h>

#include "os_mgmt/os_mgmt.h"
#include "img_mgmt/img_mgmt.h"


#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led'n'" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

void main(void)
{
	printk("build time: " __DATE__ " " __TIME__ "\n");
	os_mgmt_register_group();
	img_mgmt_register_group();


	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);

	while (1) {
		gpio_pin_toggle_dt(&led0);
		gpio_pin_toggle_dt(&led1);
		k_msleep(SLEEP_TIME_MS);
	}
}

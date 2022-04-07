#include <zephyr.h>
#include <drivers/gpio.h>

#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led'n'" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

void main(void)
{
	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&led1, GPIO_OUTPUT_ACTIVE);

	while (1) {
		gpio_pin_toggle_dt(&led0);
		gpio_pin_toggle_dt(&led1);
		k_msleep(SLEEP_TIME_MS);
	}
}

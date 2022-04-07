#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>

#define SLEEP_TIME_MS	1

// Button
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios,{0});
static struct gpio_callback button0_cb_data;

static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw1), gpios,{0});
static struct gpio_callback button1_cb_data;

// LED
static struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios,{0});
static struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios,{0});

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	if(cb == &button0_cb_data) gpio_pin_toggle_dt(&led0);
	if(cb == &button1_cb_data) gpio_pin_toggle_dt(&led1);
}

void main(void)
{
	//Button 0
	gpio_pin_configure_dt(&button0, GPIO_INPUT);
	
	gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_TO_ACTIVE);
	
	gpio_init_callback(&button0_cb_data, button_pressed, BIT(button0.pin));
	gpio_add_callback(button0.port, &button0_cb_data);

	//Button 1
	gpio_pin_configure_dt(&button1, GPIO_INPUT);
	
	gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_TO_ACTIVE);
	
	gpio_init_callback(&button1_cb_data, button_pressed, BIT(button1.pin));
	gpio_add_callback(button1.port, &button1_cb_data);

	//LED
	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);

	while (1) {

	}	
}

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/printk.h>
#include <sys/__assert.h>

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

#define green led0
#define red led1
#define blue led2

static struct gpio_dt_spec green = GPIO_DT_SPEC_GET_OR(DT_ALIAS(green), gpios,{0}); //green
static struct gpio_dt_spec red = GPIO_DT_SPEC_GET_OR(DT_ALIAS(red), gpios,{0}); //red
static struct gpio_dt_spec blue = GPIO_DT_SPEC_GET_OR(DT_ALIAS(blue), gpios,{0}); //blue

bool flag = 1;


void AccessSharedResource(void){
	if(flag == 1){
		flag = 0;
	} else {
		gpio_pin_set_dt(&blue, 1);
	}
	k_msleep(500);
	flag = 1;

}

void greenTask(void)
{
	gpio_pin_configure_dt(&blue, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&green, GPIO_OUTPUT_ACTIVE);
	while(1) {
		gpio_pin_set_dt(&green, 1);
		AccessSharedResource();
		gpio_pin_set_dt(&green, 0);
		k_msleep(500);
	}
}

void redTask(void)
{
	gpio_pin_configure_dt(&red, GPIO_OUTPUT_ACTIVE);
	while(1) {
		gpio_pin_set_dt(&red, 1);
		AccessSharedResource();
		gpio_pin_set_dt(&red, 0);
		k_msleep(100);
	}
}

K_THREAD_DEFINE(greenTask_id, STACKSIZE, greenTask, NULL, NULL, NULL,
		PRIORITY, 0, 0);
K_THREAD_DEFINE(redTask_id, STACKSIZE, redTask, NULL, NULL, NULL,
		PRIORITY, 0, 0);

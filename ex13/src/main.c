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
#define orange led3

static struct gpio_dt_spec green = GPIO_DT_SPEC_GET_OR(DT_ALIAS(green), gpios,{0}); //green
static struct gpio_dt_spec red = GPIO_DT_SPEC_GET_OR(DT_ALIAS(red), gpios,{0}); //red
static struct gpio_dt_spec blue = GPIO_DT_SPEC_GET_OR(DT_ALIAS(blue), gpios,{0}); //blue
static struct gpio_dt_spec orange = GPIO_DT_SPEC_GET_OR(DT_ALIAS(orange), gpios,{0}); //orange

bool flag1 = 0;
bool flag2 = 0;


K_MUTEX_DEFINE(CriticalMutex);

/*void AccessSharedResource(void){
	k_mutex_lock(&CriticalMutex, K_FOREVER);
	for (int i = 0; i < 40; i++)
	{
		gpio_pin_toggle_dt(&blue);
		k_busy_wait(50000);	
	}
	k_mutex_unlock(&CriticalMutex);

}*/

void greenTask(void)
{
	gpio_pin_configure_dt(&green, GPIO_OUTPUT_INACTIVE);
	while(1) {
		for (int i = 0; i < 200; i++)
		{
			gpio_pin_toggle_dt(&green);
			k_msleep(50);	
		}
		flag1 = 1;

		for (int i = 0; i < 20; i++)
		{
			gpio_pin_toggle_dt(&green);
			k_msleep(500);	
		}

		flag1 = 0;

		for (int i = 0; i < 200; i++)
		{
			gpio_pin_toggle_dt(&green);
			k_msleep(50);	
		}	
		return; //suspend thread
	}
}

void redTask(void)
{
	gpio_pin_configure_dt(&blue, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&red, GPIO_OUTPUT_INACTIVE);
	while(1) {
		for (int i = 0; i < 300; i++)
		{
			gpio_pin_toggle_dt(&red);
			k_msleep(50);	
		}
		flag2 = 1;

		for (int i = 0; i < 20; i++)
		{
			gpio_pin_toggle_dt(&red);
			k_msleep(500);	
		}

		flag2 = 0;

		for (int i = 0; i < 100; i++)
		{
			gpio_pin_toggle_dt(&red);
			k_msleep(50);	
		}
		return; // suspend thread
	}
}

void orangeTask(void)
{
	gpio_pin_configure_dt(&orange, GPIO_OUTPUT_INACTIVE);
	while(1) {
		while(flag1 != 1 || flag2 != 1){
			gpio_pin_toggle_dt(&orange);
			k_msleep(50);
		}
		gpio_pin_toggle_dt(&orange);
		k_msleep(500);

	}
}

K_THREAD_DEFINE(greenTask_id, STACKSIZE, greenTask, NULL, NULL, NULL, 3, 0, 0);
K_THREAD_DEFINE(redTask_id, STACKSIZE, redTask, NULL, NULL, NULL, 1, 0, 0);
K_THREAD_DEFINE(orangeTask_id, STACKSIZE, orangeTask, NULL, NULL, NULL, 2, 0, 0);

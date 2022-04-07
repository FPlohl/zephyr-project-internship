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


K_SEM_DEFINE(CriticalSemaphore,1,1);

void AccessSharedResource(void){
	k_sem_take(&CriticalSemaphore, K_FOREVER);
	for (int i = 0; i < 40; i++)
	{
		gpio_pin_toggle_dt(&blue);
		k_busy_wait(50000);	
	}
	k_sem_give(&CriticalSemaphore);

}

void greenTask(void)
{
	gpio_pin_configure_dt(&green, GPIO_OUTPUT_INACTIVE);
	while(1) {
		AccessSharedResource();
		for (int i = 0; i < 80; i++)
		{
			gpio_pin_toggle_dt(&green);
			k_busy_wait(50000);	
		}	
		return; //suspend thread
	}
}

void redTask(void)
{
	gpio_pin_configure_dt(&blue, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&red, GPIO_OUTPUT_INACTIVE);
	while(1) {
		k_msleep(1000);
		AccessSharedResource();
		for (int i = 0; i < 80; i++)
		{
			gpio_pin_toggle_dt(&red);
			k_busy_wait(50000);	
		}
		return; // suspend thread
	}
}

void orangeTask(void)
{
	gpio_pin_configure_dt(&orange, GPIO_OUTPUT_INACTIVE);
	while(1) {
		k_msleep(1000);
		for (int i = 0; i < 80; i++)
		{
			gpio_pin_toggle_dt(&orange);
			k_busy_wait(50000);	
		}
		return; // suspend thread
	}
}

K_THREAD_DEFINE(greenTask_id, STACKSIZE, greenTask, NULL, NULL, NULL, 3, 0, 0);
K_THREAD_DEFINE(redTask_id, STACKSIZE, redTask, NULL, NULL, NULL, 1, 0, 0);
K_THREAD_DEFINE(orangeTask_id, STACKSIZE, orangeTask, NULL, NULL, NULL, 2, 0, 0);

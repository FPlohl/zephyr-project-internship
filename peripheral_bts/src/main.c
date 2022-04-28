/* main.c - Application main entry point */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>
#include <drivers/gpio.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/services/bas.h>

// Button
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios,{0});
static struct gpio_callback button0_cb_data;

#define SENSOR_1_NAME				"Sensor"

#define BT_UUID_SERVICE_VAL \
	BT_UUID_128_ENCODE(0xdc8fd89d, 0x5849, 0x468e, 0x951a, 0x7adb3ec6549e)

#define BT_UUID_SERVICE       	BT_UUID_DECLARE_128(BT_UUID_SERVICE_VAL)

#define BT_UUID_BUTTON_VAL \
	BT_UUID_128_ENCODE(0x9903df36, 0x9e45, 0x4123, 0xab4e, 0x9d564bd70071)

#define BT_UUID_BUTTON			BT_UUID_DECLARE_128(BT_UUID_BUTTON_VAL)

static ssize_t read_u16(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	const uint16_t *u16 = attr->user_data;
	uint16_t value = sys_cpu_to_le16(*u16);

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &value,
				 sizeof(value));
}

/* Button Service Declaration */

struct button_sensor {
	int16_t button_state;
};

static bool state;
static struct button_sensor sensor_1 = {
		.button_state = 0,
};

static void button_ccc_cfg_changed(const struct bt_gatt_attr *attr,
				 uint16_t value)
{
	state = value == BT_GATT_CCC_NOTIFY;
}

static void update_state(struct bt_conn *conn,
			       const struct bt_gatt_attr *chrc, int16_t value,
			       struct button_sensor *sensor)
{
	sensor->button_state = value;

	value = sys_cpu_to_le16(sensor->button_state);

	bt_gatt_notify(conn, chrc, &value, sizeof(value));
}

BT_GATT_SERVICE_DEFINE(bts_svc,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_SERVICE),

	/* Sensor 1 */
	BT_GATT_CHARACTERISTIC(BT_UUID_BUTTON,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ,
			       read_u16, NULL, &sensor_1.button_state),
	BT_GATT_CCC(button_ccc_cfg_changed,
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, 0x00, 0x03),
	BT_DATA_BYTES(BT_DATA_UUID128_ALL,
		      BT_UUID_128_ENCODE(0xdc8fd89d, 0x5849, 0x468e, 0x951a, 0x7adb3ec6549e)),
};

static void bt_ready(void)
{
	int err;

	printk("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err 0x%02x)\n", err);
	} else {
		printk("Connected\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason 0x%02x)\n", reason);
	bt_ready();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Passkey for %s: %06u\n", addr, passkey);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = NULL,
	.cancel = auth_cancel,
};

uint16_t val = 0;
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	if(cb == &button0_cb_data){
		val = !val;
		printk("%d", val);
		update_state(NULL, &bts_svc.attrs[2], val, &sensor_1);
	}
}

void main(void)
{
	gpio_pin_configure_dt(&button0, GPIO_INPUT);
	
	gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_TO_ACTIVE);
	
	gpio_init_callback(&button0_cb_data, button_pressed, BIT(button0.pin));
	gpio_add_callback(button0.port, &button0_cb_data);

	int err;

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	bt_ready();

	bt_conn_auth_cb_register(&auth_cb_display);

	while (1) {

	}
}

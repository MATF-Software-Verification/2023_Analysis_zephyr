/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mocks_unit_tests/conn.h"
#include "mocks_unit_tests/hci_core.h"
#include "mocks_unit_tests/net_buf.h"

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/cs.h>
#include <zephyr/fff.h>

DEFINE_FFF_GLOBALS;

ZTEST_SUITE(bt_le_cs_security_enable, NULL, NULL, NULL, NULL, NULL);

/**
 * Mock bt_conn structure used for unit tests
 */
static struct bt_conn test_conn_mock = {
    .handle = 0x0001, // Assign a test handle
    .state = BT_CONN_CONNECTED, // Simulate a connected state
};

/**
 * Custom net_buf_simple_add stub implementation
 */
void *custom_net_buf_simple_add(struct net_buf_simple *buf, size_t len) {
    printk("Mock net_buf_simple_add called! len=%zu\n", len);
    void *ptr = &buf->data[buf->len];
    buf->len += len;
    return ptr;
}

ZTEST(bt_le_cs_security_enable, test_sec_enable_success)
{
	struct bt_conn *test_conn = &test_conn_mock;

	// Setup the data buffer for net_buf_add
	static uint8_t test_data[256];
	static struct net_buf test_buf = {.b = {
						  .data = test_data,
						  .len = 0,
						  .size = sizeof(test_data),
					  }};
	
	// Since net_buf_add is static inline, setup a buffer for mocked net_buf_simple_add function
	struct net_buf_simple *simple_test_buf = &test_buf.b;

	// Replace generic mock implementation with stubbed function
	net_buf_simple_add_fake.custom_fake = custom_net_buf_simple_add;

	// Make sure bt_hci_cmd_create returns a non-null value
	bt_hci_cmd_create_fake.return_val = &test_buf;
	zassert_not_null(bt_hci_cmd_create_fake.return_val, "bt_hci_cmd_create returned NULL!");

	// Function under test
	zassert_false(bt_le_cs_security_enable(test_conn) == -ENOBUFS);

	// Confirm bt_hci_cmd_create expecations
	zassert_equal(bt_hci_cmd_create_fake.arg0_val, BT_HCI_OP_LE_CS_SECURITY_ENABLE);
	zassert_equal(bt_hci_cmd_create_fake.call_count, 1);

	// Confirm net_buf_simple_add expecations
	zassert_equal(net_buf_simple_add_fake.call_count, 1);
	zassert_equal_ptr(net_buf_simple_add_fake.arg0_val, simple_test_buf);
}

/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mocks_unit_tests/conn.h"
#include "mocks_unit_tests/hci_core.h"
#include "mocks_unit_tests/net_buf.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/bluetooth/cs.h>
#include <zephyr/fff.h>

DEFINE_FFF_GLOBALS;

static void fff_reset_rule_before(const struct ztest_unit_test *test, void *fixture)
{
	CONN_FFF_FAKES_LIST(RESET_FAKE);
	HCI_CORE_FFF_FAKES_LIST(RESET_FAKE);
	NET_BUF_FFF_FAKES_LIST(RESET_FAKE);
}

ZTEST_RULE(fff_reset_rule, fff_reset_rule_before, NULL);

ZTEST_SUITE(channel_sounding_tests, NULL, NULL, NULL, NULL, NULL);

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

ZTEST(channel_sounding_tests, test_sec_enable_success)
{
	// [Given]
	// Assign the mock structure to test connection
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

	// [When]
	// Function under test
	zassert_false(bt_le_cs_security_enable(test_conn) == -ENOBUFS);

	// [Then]
	// Confirm bt_hci_cmd_create expecations
	zassert_equal(bt_hci_cmd_create_fake.arg0_val, BT_HCI_OP_LE_CS_SECURITY_ENABLE);
	zassert_equal(bt_hci_cmd_create_fake.call_count, 1);

	// Confirm net_buf_simple_add expecations
	zassert_equal(net_buf_simple_add_fake.call_count, 1);
	zassert_equal_ptr(net_buf_simple_add_fake.arg0_val, simple_test_buf);

	zassert_equal(bt_hci_cmd_send_sync_fake.call_count, 1);
}

ZTEST(channel_sounding_tests, test_bt_le_cs_read_remote_supported_capabilities)
{
	// Setup the data buffer for net_buf_add
	static uint8_t test_data[256];
	static struct net_buf test_buf = {.b = {
						  .data = test_data,
						  .len = 0,
						  .size = sizeof(test_data),
					  }};
	
	// Since net_buf_add is static inline, setup a buffer for mocked net_buf_simple_add function
	struct net_buf_simple *simple_test_buf = &test_buf.b;

	// Make sure bt_hci_cmd_create returns a non-null value
	bt_hci_cmd_create_fake.return_val = &test_buf;
	zassert_not_null(bt_hci_cmd_create_fake.return_val, "bt_hci_cmd_create returned NULL!");

	// Replace generic mock implementation with stubbed function
	net_buf_simple_add_fake.custom_fake = custom_net_buf_simple_add;

	struct bt_conn *test_conn = &test_conn_mock;

	bt_le_cs_read_remote_supported_capabilities(test_conn);

	zassert_equal(bt_hci_cmd_send_sync_fake.call_count, 1, "Actual call count %d", bt_hci_cmd_send_sync_fake.call_count);
	zassert_equal(bt_hci_cmd_send_sync_fake.arg0_val, BT_HCI_OP_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES);
	// zassert_equal_ptr(bt_hci_cmd_send_sync_fake.arg1_val, simple_test_buf);
	zassert_equal_ptr(bt_hci_cmd_send_sync_fake.arg2_val, NULL);

	// Confirm net_buf_simple_add expecations
	zassert_equal(net_buf_simple_add_fake.call_count, 1);
	zassert_equal_ptr(net_buf_simple_add_fake.arg0_val, simple_test_buf);
}

ZTEST(channel_sounding_tests, test_read_remote_supported_capabilities_complete){
    // Setup
    struct net_buf buf;
    struct bt_hci_evt_le_cs_read_remote_supported_capabilities_complete evt = {
        .status = 0, // Success
        .conn_handle = sys_cpu_to_le16(0x1234),
        .num_config_supported = 5,
        .max_consecutive_procedures_supported = sys_cpu_to_le16(10),
        .num_antennas_supported = 2,
        .max_antenna_paths_supported = 4,
        .roles_supported = BT_HCI_LE_CS_INITIATOR_ROLE_MASK | BT_HCI_LE_CS_REFLECTOR_ROLE_MASK,
        .modes_supported = BT_HCI_LE_CS_MODES_SUPPORTED_MODE_3_MASK,
        .rtt_aa_only_n = 1,
        .rtt_sounding_n = 1,
        .rtt_random_payload_n = 1,
        .rtt_capability = BT_HCI_LE_CS_RTT_AA_ONLY_N_10NS_MASK | BT_HCI_LE_CS_RTT_SOUNDING_N_10NS_MASK |
                          BT_HCI_LE_CS_RTT_RANDOM_PAYLOAD_N_10NS_MASK,
        .nadm_sounding_capability = sys_cpu_to_le16(BT_HCI_LE_CS_NADM_SOUNDING_CAPABILITY_PHASE_BASED_MASK),
        .nadm_random_capability = sys_cpu_to_le16(BT_HCI_LE_CS_NADM_RANDOM_CAPABILITY_PHASE_BASED_MASK),
        .cs_sync_phys_supported = BT_HCI_LE_CS_SYNC_PHYS_2M_MASK | BT_HCI_LE_CS_SYNC_PHYS_2M_2BT_MASK,
        .subfeatures_supported = sys_cpu_to_le16(BT_HCI_LE_CS_SUBFEATURE_NO_TX_FAE_MASK |
                                                BT_HCI_LE_CS_SUBFEATURE_CHSEL_ALG_3C_MASK),
        .t_ip1_times_supported = sys_cpu_to_le16(100),
        .t_ip2_times_supported = sys_cpu_to_le16(200),
        .t_fcs_times_supported = sys_cpu_to_le16(300),
        .t_pm_times_supported = sys_cpu_to_le16(400),
        .t_sw_time_supported = 10,
        .tx_snr_capability = 5,
    };

    buf.data = (uint8_t *)&evt;
    buf.len = sizeof(evt);

	net_buf_simple_pull_mem_fake.return_val = &evt;

    // struct bt_conn conn;
	struct bt_conn *test_conn = &test_conn_mock;
    bt_conn_lookup_handle_fake.return_val = test_conn;

    // Function under test
    bt_hci_le_cs_read_remote_supported_capabilities_complete(&buf);

    // Assertions
    zassert_equal(bt_conn_lookup_handle_fake.call_count, 1, "bt_conn_lookup_handle not called");
    zassert_equal(bt_conn_lookup_handle_fake.arg0_val, 0x1234, "Incorrect connection handle");
    zassert_equal(notify_remote_cs_capabilities_fake.call_count, 1, "notify_remote_cs_capabilities not called");
    zassert_equal(bt_conn_unref_fake.call_count, 1, "bt_conn_unref not called");
}

ZTEST(channel_sounding_tests, test_read_remote_fae_table)
{
	// Setup
    struct bt_conn *test_conn = &test_conn_mock;
    // struct net_buf buf;

	static uint8_t test_data[256];
	static struct net_buf test_buf = {.b = {
						  .data = test_data,
						  .len = 0,
						  .size = sizeof(test_data),
					  }};
					  
    bt_hci_cmd_create_fake.return_val = &test_buf;

	// Since net_buf_add is static inline, setup a buffer for mocked net_buf_simple_add function
	struct net_buf_simple *simple_test_buf = &test_buf.b;

	// Replace generic mock implementation with stubbed function
	net_buf_simple_add_fake.custom_fake = custom_net_buf_simple_add;

    // Test
    int ret = bt_le_cs_read_remote_fae_table(test_conn);

    // Assertions
    zassert_equal(ret, 0, "Function failed");
    zassert_equal(bt_hci_cmd_create_fake.call_count, 1, "bt_hci_cmd_create not called");
    zassert_equal(bt_hci_cmd_send_sync_fake.call_count, 1, "bt_hci_cmd_send_sync not called");
	zassert_equal_ptr(net_buf_simple_add_fake.arg0_val, simple_test_buf);
}

void *net_buf_simple_pull_mem_mock(struct net_buf_simple *buf, size_t len)
{
	void *data = buf->data; // Save the current data pointer
	buf->data += len;       // Advance the data pointer
	buf->len -= len;        // Reduce the remaining length
	return data;            // Return the original data pointer
}

ZTEST(channel_sounding_tests, test_bt_hci_le_cs_read_remote_fae_table_complete_success)
{
	struct bt_conn *test_conn = &test_conn_mock;

	int8_t test_fae_table[72] = { /* Example FAE table data */
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
		0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
		0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
		0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48
	};

	/* Test buffer */
	uint8_t test_buf_data[sizeof(struct bt_hci_evt_le_cs_read_remote_fae_table_complete)];
	struct net_buf test_buf = {
		.data = test_buf_data,
		.len = sizeof(test_buf_data),
	};

	struct bt_hci_evt_le_cs_read_remote_fae_table_complete *evt;
	struct bt_conn_le_cs_fae_table expected_fae_table;

	/* Set the status field to zero */
	evt = (struct bt_hci_evt_le_cs_read_remote_fae_table_complete *)test_buf.data;
	evt->status = 0x00;
	evt->conn_handle = sys_cpu_to_le16(0x1234); /* Example value */
	memcpy(evt->remote_fae_table, test_fae_table, sizeof(test_fae_table));

	/* Make bt_conn_lookup_handle return a valid connection */
	bt_conn_lookup_handle_fake.return_val = test_conn;

	// Replace generic mock implementation with stubbed function
	// net_buf_simple_add_fake.custom_fake = custom_net_buf_simple_add;
	net_buf_simple_pull_mem_fake.custom_fake = net_buf_simple_pull_mem_mock;

	/* Call the function */
	bt_hci_le_cs_read_remote_fae_table_complete(&test_buf);

	/* Verify that the function called notify_remote_cs_fae_table with the correct parameters */
	expected_fae_table.remote_fae_table = evt->remote_fae_table;
	zassert_equal(notify_remote_cs_fae_table_fake.call_count, 1, "Expected notify_remote_cs_fae_table to be called once");
	zassert_equal_ptr(notify_remote_cs_fae_table_fake.arg0_val, test_conn, "Expected notify_remote_cs_fae_table to be called with the correct connection");
	zassert_mem_equal(&notify_remote_cs_fae_table_fake.arg1_val, &expected_fae_table, sizeof(expected_fae_table), "Expected notify_remote_cs_fae_table to be called with the correct FAE table");

	/* Verify that bt_conn_unref was called */
	zassert_equal(bt_conn_unref_fake.call_count, 1, "Expected bt_conn_unref to be called once");
	zassert_equal_ptr(bt_conn_unref_fake.arg0_val, test_conn, "Expected bt_conn_unref to be called with the correct connection");
}
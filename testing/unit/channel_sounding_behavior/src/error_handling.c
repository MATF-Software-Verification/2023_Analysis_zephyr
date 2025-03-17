#include "mocks_unit_tests/conn.h"
#include "mocks_unit_tests/hci_core.h"
#include "mocks_unit_tests/net_buf.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/bluetooth/cs.h>
#include <zephyr/fff.h>

// DEFINE_FFF_GLOBALS;

static void fff_reset_rule_before(const struct ztest_unit_test *test, void *fixture)
{
	CONN_FFF_FAKES_LIST(RESET_FAKE);
	HCI_CORE_FFF_FAKES_LIST(RESET_FAKE);
	NET_BUF_FFF_FAKES_LIST(RESET_FAKE);
}

ZTEST_RULE(fff_reset_rule, fff_reset_rule_before, NULL);

ZTEST_SUITE(channel_sounding_tests_error_handling, NULL, NULL, NULL, NULL, NULL);

// LOG_MODULE_REGISTER(channel_sounding_tests_error_handling);

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
extern void *custom_net_buf_simple_add(struct net_buf_simple *buf, size_t len);

ZTEST(channel_sounding_tests_error_handling, test_sec_enable_hci_cmd_fail)
{
	struct bt_conn *test_conn = &test_conn_mock;

	// Trigger bt_hci_cmd_create null return
	bt_hci_cmd_create_fake.return_val = NULL;

	// Function under test
	zassert_equal(bt_le_cs_security_enable(test_conn), -ENOBUFS);

	// Confirm bt_hci_cmd_create expecations
	zassert_equal(bt_hci_cmd_create_fake.arg0_val, BT_HCI_OP_LE_CS_SECURITY_ENABLE);
	zassert_equal(bt_hci_cmd_create_fake.call_count, 1);

	// Confirm net_buf_simple_add expecations - return before reaching this function
	zassert_equal(net_buf_simple_add_fake.call_count, 0);
}

ZTEST(channel_sounding_tests_error_handling, test_sec_enable_hci_cmd_fail_null_conn)
{
	// Function under test
	zassert_equal(bt_le_cs_security_enable(NULL), -ENOBUFS);

	// Confirm bt_hci_cmd_create expecations
	zassert_equal(bt_hci_cmd_create_fake.call_count, 0);

	// Confirm net_buf_simple_add expecations - return before reaching this function
	zassert_equal(net_buf_simple_add_fake.call_count, 0);
}

ZTEST(channel_sounding_tests, test_sec_enable_cmd_send_fail)
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

	bt_hci_cmd_send_sync_fake.return_val = -EACCES;

	// [When]
	// Function under test
	zassert_equal(bt_le_cs_security_enable(test_conn), bt_hci_cmd_send_sync_fake.return_val);

	// [Then]
	// Confirm bt_hci_cmd_create expecations
	zassert_equal(bt_hci_cmd_create_fake.arg0_val, BT_HCI_OP_LE_CS_SECURITY_ENABLE);
	zassert_equal(bt_hci_cmd_create_fake.call_count, 1);

	// Confirm net_buf_simple_add expecations
	zassert_equal(net_buf_simple_add_fake.call_count, 1);
	zassert_equal_ptr(net_buf_simple_add_fake.arg0_val, simple_test_buf);

	zassert_equal(bt_hci_cmd_send_sync_fake.call_count, 1);
}

// ZTEST(channel_sounding_tests_error_handling, test_read_remote_supported_capabilities_complete_null_buf)
// {
// 	// Function under test
// 	bt_hci_le_cs_read_remote_supported_capabilities_complete(NULL);

// 	// Assertions
// 	zassert_equal(net_buf_simple_pull_mem_fake.call_count, 0, "net_buf_simple_pull_mem unexpectedly called!");
// 	zassert_equal(bt_conn_lookup_handle_fake.call_count, 0, "bt_conn_lookup_handle unexpectedly called!");
// 	zassert_equal(notify_remote_cs_capabilities_fake.call_count, 0, "notify_remote_cs_capabilities unexpectedly called!");
// 	zassert_equal(bt_conn_unref_fake.call_count, 0, "bt_conn_unref unexpectedly called!");
// }

ZTEST(channel_sounding_tests_error_handling, test_read_remote_supported_capabilities_complete_invalid_buf_len){
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
    buf.len = sizeof(evt) - 1; // Force invalid buffer lenght

	
    // struct bt_conn conn;
	struct bt_conn *test_conn = &test_conn_mock;
    bt_conn_lookup_handle_fake.return_val = test_conn;

    // Function under test
    bt_hci_le_cs_read_remote_supported_capabilities_complete(&buf);

    // Assertions
    zassert_equal(bt_conn_lookup_handle_fake.call_count, 0, "bt_conn_lookup_handle called more times than expected!");
	zassert_equal(net_buf_simple_pull_mem_fake.call_count, 0, "net_buf_simple_pull_mem called more times than expected!");
    zassert_equal(notify_remote_cs_capabilities_fake.call_count, 0, "notify_remote_cs_capabilities called more times than expected");
    zassert_equal(bt_conn_unref_fake.call_count, 0, "bt_conn_unref called more times than expected");
}

ZTEST(channel_sounding_tests_error_handling, test_read_remote_supported_capabilities_complete_evt_status_fail){
    // Setup
    struct net_buf buf;
    struct bt_hci_evt_le_cs_read_remote_supported_capabilities_complete evt = {
        .status = 1, // Success
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
    zassert_equal(bt_conn_lookup_handle_fake.call_count, 0, "bt_conn_lookup_handle called more times than expected!");
    zassert_equal(notify_remote_cs_capabilities_fake.call_count, 0, "notify_remote_cs_capabilities called more times than expected!");
    zassert_equal(bt_conn_unref_fake.call_count, 0, "bt_conn_unref called more times than expected!");
}

ZTEST(channel_sounding_tests_error_handling, test_read_remote_supported_capabilities_complete_conn_lookup_handle_fail){
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

    // Force the invalid return value	
    bt_conn_lookup_handle_fake.return_val = NULL;

    // Function under test
    bt_hci_le_cs_read_remote_supported_capabilities_complete(&buf);

    // Assertions
    zassert_equal(bt_conn_lookup_handle_fake.call_count, 1, "bt_conn_lookup_handle not called!");
    zassert_equal(notify_remote_cs_capabilities_fake.call_count, 0, "notify_remote_cs_capabilities called more times than expected!");
    zassert_equal(bt_conn_unref_fake.call_count, 0, "bt_conn_unref called more times than expected!");
}

ZTEST(channel_sounding_tests_error_handling, test_read_remote_fae_table_hci_cmd_fail)
{
	// Setup
    struct bt_conn *test_conn = &test_conn_mock;
					  
    bt_hci_cmd_create_fake.return_val = NULL;

    // Function under test
    int ret = bt_le_cs_read_remote_fae_table(test_conn);

    // Assertions
    zassert_equal(ret, -ENOBUFS, "Function succeeded, when it was expected to fail!");
    zassert_equal(bt_hci_cmd_create_fake.call_count, 1, "bt_hci_cmd_create not called");
    zassert_equal(bt_hci_cmd_send_sync_fake.call_count, 0, "bt_hci_cmd_send_sync called more times than expected!");
	zassert_equal_ptr(net_buf_simple_add_fake.call_count, 0, "net_buf_simple_add called more times than expected!");
}

// ZTEST(channel_sounding_tests_error_handling, test_read_remote_fae_table_null_conn)
// {
// 	// Setup
//     struct bt_conn *test_conn = &test_conn_mock;
//     // struct net_buf buf;

// 	static uint8_t test_data[256];
// 	static struct net_buf test_buf = {.b = {
// 						  .data = test_data,
// 						  .len = 0,
// 						  .size = sizeof(test_data),
// 					  }};
					  
//     bt_hci_cmd_create_fake.return_val = &test_buf;

// 	// Replace generic mock implementation with stubbed function
// 	net_buf_simple_add_fake.return_val= NULL;

//     // Test
//     int ret = bt_le_cs_read_remote_fae_table(test_conn);

//     // Assertions
//     zassert_equal(ret, 0, "Function failed");
//     zassert_equal(bt_hci_cmd_create_fake.call_count, 1, "bt_hci_cmd_create not called");
//     zassert_equal(bt_hci_cmd_send_sync_fake.call_count, 0, "bt_hci_cmd_send_sync called more times than expected!");
// }

ZTEST(channel_sounding_tests_error_handling, test_read_remote_fae_table_send_sync_failed)
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

    // Propagate failure to the callee
    bt_hci_cmd_send_sync_fake.return_val = -42;

    // Test
    int ret = bt_le_cs_read_remote_fae_table(test_conn);

    // Assertions
    zassert_equal(ret, bt_hci_cmd_send_sync_fake.return_val, "Function failed");
    zassert_equal(bt_hci_cmd_create_fake.call_count, 1, "bt_hci_cmd_create not called");
    zassert_equal(bt_hci_cmd_send_sync_fake.call_count, 1, "bt_hci_cmd_send_sync not called");
	zassert_equal_ptr(net_buf_simple_add_fake.arg0_val, simple_test_buf);
}

static uint8_t test_buf_data[sizeof(struct bt_hci_evt_le_cs_read_remote_fae_table_complete)];
static struct net_buf test_buf = {
	.data = test_buf_data,
	.len = sizeof(test_buf_data),
};

// static struct bt_conn test_conn;

ZTEST(channel_sounding_tests_error_handling, test_bt_hci_le_cs_read_remote_fae_table_complete_invalid_buf_size_fail)
{
	// Setup
	test_buf.len = sizeof(struct bt_hci_evt_le_cs_read_remote_fae_table_complete) - 1;

	// Function under test
	bt_hci_le_cs_read_remote_fae_table_complete(&test_buf);

	// Assertions
	zassert_equal(notify_remote_cs_fae_table_fake.call_count, 0, "Expected notify_remote_cs_fae_table not to be called");
}

ZTEST(channel_sounding_tests_error_handling, test_bt_hci_le_cs_read_remote_fae_table_complete_evt_status_fail)
{
	struct bt_hci_evt_le_cs_read_remote_fae_table_complete *evt;

	// Setup
	evt = (struct bt_hci_evt_le_cs_read_remote_fae_table_complete *)test_buf.data;
	evt->status = 0x01;

	// Function under test
	bt_hci_le_cs_read_remote_fae_table_complete(&test_buf);

	// Assertions
	zassert_equal(notify_remote_cs_fae_table_fake.call_count, 0, "Expected notify_remote_cs_fae_table not to be called");
}

ZTEST(channel_sounding_tests_error_handling, test_bt_hci_le_cs_read_remote_fae_table_complete_conn_lookup_fail)
{
	struct bt_hci_evt_le_cs_read_remote_fae_table_complete *evt;

	// Setup
	evt = (struct bt_hci_evt_le_cs_read_remote_fae_table_complete *)test_buf.data;
	evt->status = 0x00;

	// Force NULL return
	bt_conn_lookup_handle_fake.return_val = NULL;

	// Function under test
	bt_hci_le_cs_read_remote_fae_table_complete(&test_buf);

	// Assertions
	zassert_equal(notify_remote_cs_fae_table_fake.call_count, 0, "Expected notify_remote_cs_fae_table not to be called");
}
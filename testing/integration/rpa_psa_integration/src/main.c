#include <zephyr/ztest.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/crypto.h>
#include <zephyr/logging/log.h>
#include "common/rpa.h"
#include <zephyr/settings/settings.h>


LOG_MODULE_REGISTER(bt_psa_integration);

static void *bt_suite_setup(void){
	int err;

	LOG_INF("Enabling the bt stack");
	err = bt_enable(NULL);
	zassert_equal(err, 0, "Bluetooth init failed (err %d)", err);

	settings_load();

	return NULL;
}

ZTEST_SUITE(bt_psa_integration, NULL, bt_suite_setup, NULL, NULL, NULL);

static const uint8_t test_irk[16] = {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00
};

ZTEST(bt_psa_integration, test_rpa_creation_and_resolution)
{
    bt_addr_t rpa;
    int err;

    // Generate an RPA using a known IRK (uses bt_encrypt_le internally -> PSA)
    err = bt_rpa_create(test_irk, &rpa);
    zassert_equal(err, 0, "Failed to create RPA: %d", err);

    // Now try to resolve it back
    bool rpa_match = bt_rpa_irk_matches(test_irk, &rpa);
    zassert_true(rpa_match, "Failed to resolve RPA with correct IRK");

    // Modify IRK (simulate wrong peer)
    uint8_t corrupted_irk[16];
    memcpy(corrupted_irk, test_irk, 16);
    corrupted_irk[15] ^= 0xFF; // Flip a bit

    rpa_match = bt_rpa_irk_matches(corrupted_irk, &rpa);
    zassert_false(rpa_match, "Resolved RPA with wrong IRK — should fail");
}

ZTEST(bt_psa_integration, test_advertising_address_is_rpa) {
	int err;

	uint8_t irk[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
	                 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};

	int id = bt_id_create(NULL, irk);
	zassert_true(id >= 0, "ID creation failed: %d", id);

	// Set this identity as active
    bt_id_reset(id, NULL, irk);

	k_sleep(K_SECONDS(1));
	struct bt_le_ext_adv *adv;
	const struct bt_le_adv_param adv_params = {
		.id = id,
		.options = BT_LE_ADV_OPT_CONN,
		.interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
		.interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
	};

	err = bt_le_ext_adv_create(&adv_params, NULL, &adv);
	zassert_equal(err, 0, "Extended advertising object creation failed: %d", err);

	LOG_INF("Starting advertising...");
	const struct bt_le_ext_adv_start_param ext_adv_params = {
		.timeout = 60,
		.num_events = 100,
	};
	err = bt_le_ext_adv_start(adv, &ext_adv_params);
	zassert_equal(err, 0, "Advertising start failed: %d", err);

	k_sleep(K_SECONDS(1));

	struct bt_le_ext_adv_info advertising_info;
	err = bt_le_ext_adv_get_info(adv, &advertising_info);
	zassert_equal(err, 0, "Failed to get advertising info: %d", err);

	LOG_INF("Advertising address (from advertising info): %02X:%02X:%02X:%02X:%02X:%02X (%s)",
	        advertising_info.addr->a.val[5], advertising_info.addr->a.val[4], advertising_info.addr->a.val[3],
	        advertising_info.addr->a.val[2], advertising_info.addr->a.val[1], advertising_info.addr->a.val[0],
	        advertising_info.addr->type == BT_ADDR_LE_RANDOM ? "RANDOM" : "PUBLIC");

	zassert_true(BT_ADDR_IS_RPA(&advertising_info.addr->a), "Advertiser address is not RPA");

	bt_le_ext_adv_stop(adv);
	bt_le_ext_adv_delete(adv);
	// bt_disable();
}

ZTEST(bt_psa_integration, test_rpa_rotation) {
	int err;

	// LOG_INF("Enabling the bt stack");
	// err = bt_enable(NULL);
	// zassert_equal(err, 0, "Bluetooth init failed (err %d)", err);

	// settings_load();

	uint8_t irk[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
	                 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};

	int id = bt_id_create(NULL, irk);
	zassert_true(id >= 0, "ID creation failed: %d", id);

	// Set this identity as active
    bt_id_reset(id, NULL, irk);

	k_sleep(K_SECONDS(1));
	struct bt_le_ext_adv *adv;
	const struct bt_le_adv_param adv_params = {
		.id = id,
		.options = BT_LE_ADV_OPT_CONN,
		.interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
		.interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
	};

	err = bt_le_ext_adv_create(&adv_params, NULL, &adv);
	zassert_equal(err, 0, "Extended advertising object creation failed: %d", err);

	LOG_INF("Starting advertising...");
	const struct bt_le_ext_adv_start_param ext_adv_params = {
		.timeout = 0,
		.num_events = 0,
	};
	err = bt_le_ext_adv_start(adv, &ext_adv_params);
	zassert_equal(err, 0, "Advertising start failed: %d", err);

	k_sleep(K_SECONDS(1));

	struct bt_le_ext_adv_info advertising_info;
	err = bt_le_ext_adv_get_info(adv, &advertising_info);
	zassert_equal(err, 0, "Failed to get advertising info: %d", err);


	LOG_INF("Advertising address (from OOB): %02X:%02X:%02X:%02X:%02X:%02X (%s)",
	        advertising_info.addr->a.val[5], advertising_info.addr->a.val[4], advertising_info.addr->a.val[3],
	        advertising_info.addr->a.val[2], advertising_info.addr->a.val[1], advertising_info.addr->a.val[0],
	        advertising_info.addr->type == BT_ADDR_LE_RANDOM ? "RANDOM" : "PUBLIC");
	
	bt_addr_le_t addr_before = *advertising_info.addr;
	zassert_true(BT_ADDR_IS_RPA(&advertising_info.addr->a), "Advertiser address is not RPA");

	uint16_t rpa_short_timeout = 1;
	err = bt_le_set_rpa_timeout(rpa_short_timeout);
	zassert_equal(err, 0, "Failed to set RPA timeout");
	LOG_INF("Sleeping for RPA timeout [%d]...", rpa_short_timeout);
	k_sleep(K_SECONDS(rpa_short_timeout * 35));

	err = bt_le_ext_adv_get_info(adv, &advertising_info);
	zassert_equal(err, 0, "Failed to get advertising info: %d", err);
	bt_addr_le_t addr_after = *advertising_info.addr;

	LOG_INF("Rotated address: %02X:%02X:%02X:%02X:%02X:%02X",
		addr_after.a.val[5], addr_after.a.val[4], addr_after.a.val[3],
		addr_after.a.val[2], addr_after.a.val[1], addr_after.a.val[0]);

	zassert_true(BT_ADDR_IS_RPA(&addr_after.a), "Advertiser address is not RPA");

	zassert_true(memcmp(addr_before.a.val, addr_after.a.val, sizeof(bt_addr_t)) != 0,
	                      "RPA did not rotate");

	bt_le_ext_adv_stop(adv);
	bt_le_ext_adv_delete(adv);
}

ZTEST(bt_psa_integration, test_rpa_is_stable_before_timeout)
{
	int err;

	uint8_t irk[16] = { [0 ... 15] = 0xAA };
	int id = bt_id_create(NULL, irk);
	zassert_true(id >= 0, "ID creation failed");

	struct bt_le_ext_adv *adv;
	struct bt_le_adv_param adv_params = {
		.id = id,
		.options = BT_LE_ADV_OPT_CONN,
		.interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
		.interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
	};

	err = bt_le_ext_adv_create(&adv_params, NULL, &adv);
	zassert_equal(err, 0, "Extended advertising object creation failed: %d", err);

	LOG_INF("Starting advertising...");
	const struct bt_le_ext_adv_start_param ext_adv_params = {
		.timeout = 60,
		.num_events = 100,
	};
	err = bt_le_ext_adv_start(adv, &ext_adv_params);
	zassert_equal(err, 0, "Adv start failed");

	// Set timeout to something long
	err = bt_le_set_rpa_timeout(30 * 2);
	zassert_equal(err, 0, "RPA timeout set failed");

	struct bt_le_ext_adv_info advertising_info;
	err = bt_le_ext_adv_get_info(adv, &advertising_info);
	zassert_equal(err, 0, "Failed to get advertising info: %d", err);

	LOG_INF("Advertising address (from advertising info): %02X:%02X:%02X:%02X:%02X:%02X (%s)",
	        advertising_info.addr->a.val[5], advertising_info.addr->a.val[4], advertising_info.addr->a.val[3],
	        advertising_info.addr->a.val[2], advertising_info.addr->a.val[1], advertising_info.addr->a.val[0],
	        advertising_info.addr->type == BT_ADDR_LE_RANDOM ? "RANDOM" : "PUBLIC");

	bt_addr_le_t addr_before = *advertising_info.addr;
	zassert_true(BT_ADDR_IS_RPA(&advertising_info.addr->a), "Advertiser address is not RPA");

	k_sleep(K_SECONDS(5));

	err = bt_le_ext_adv_get_info(adv, &advertising_info);
	zassert_equal(err, 0, "Failed to get advertising info: %d", err);
	bt_addr_le_t addr_after = *advertising_info.addr;

	LOG_INF("Rotated address: %02X:%02X:%02X:%02X:%02X:%02X",
		addr_after.a.val[5], addr_after.a.val[4], addr_after.a.val[3],
		addr_after.a.val[2], addr_after.a.val[1], addr_after.a.val[0]);

	zassert_true(BT_ADDR_IS_RPA(&addr_after.a), "Advertiser address is not RPA");
	
	zassert_true(memcmp(addr_before.a.val, addr_after.a.val, sizeof(bt_addr_t)) == 0,
	             "RPA changed unexpectedly");
	
	bt_le_ext_adv_stop(adv);
	bt_le_ext_adv_delete(adv);
}

ZTEST(bt_psa_integration, test_different_irks_yield_different_rpas)
{
    int err;
    uint8_t irk1[16] = { [0 ... 15] = 0x11 };
    uint8_t irk2[16] = { [0 ... 15] = 0x22 };

    int id1 = bt_id_create(NULL, irk1);
    int id2 = bt_id_create(NULL, irk2);
    zassert_true(id1 >= 0 && id2 >= 0, "Failed to create identities");

    struct bt_le_ext_adv *adv1, *adv2;
    struct bt_le_ext_adv_info adv_info1, adv_info2;
	bt_addr_le_t addr1, addr2;

    const struct bt_le_adv_param adv_params1 = {
        .id = id1,
        .options = BT_LE_ADV_OPT_CONN,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        .peer = NULL,
    };

    const struct bt_le_adv_param adv_params2 = {
        .id = id2,
        .options = BT_LE_ADV_OPT_CONN,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
        .peer = NULL,
    };

    err = bt_le_ext_adv_create(&adv_params1, NULL, &adv1);
    zassert_equal(err, 0, "Failed to create advertising set 1: %d", err);

	err = bt_le_ext_adv_get_info(adv1, &adv_info1);
    zassert_equal(err, 0, "Failed to get advertising info 1: %d", err);

	LOG_INF("Advertising address (from advertising info): %02X:%02X:%02X:%02X:%02X:%02X (%s)",
	        adv_info1.addr->a.val[5], adv_info1.addr->a.val[4], adv_info1.addr->a.val[3],
	        adv_info1.addr->a.val[2], adv_info1.addr->a.val[1], adv_info1.addr->a.val[0],
	        adv_info1.addr->type == BT_ADDR_LE_RANDOM ? "RANDOM" : "PUBLIC");

	// Copy the address in order to preserve it
	bt_addr_le_copy(&addr1, adv_info1.addr);

	bt_le_ext_adv_delete(adv1);

    err = bt_le_ext_adv_create(&adv_params2, NULL, &adv2);
    zassert_equal(err, 0, "Failed to create advertising set 2: %d", err);

    err = bt_le_ext_adv_get_info(adv2, &adv_info2);
    zassert_equal(err, 0, "Failed to get advertising info 2: %d", err);

	LOG_INF("Advertising address (from advertising info): %02X:%02X:%02X:%02X:%02X:%02X (%s)",
	        adv_info2.addr->a.val[5], adv_info2.addr->a.val[4], adv_info2.addr->a.val[3],
	        adv_info2.addr->a.val[2], adv_info2.addr->a.val[1], adv_info2.addr->a.val[0],
	        adv_info2.addr->type == BT_ADDR_LE_RANDOM ? "RANDOM" : "PUBLIC");

	// Copy the address in order to preserve it
	bt_addr_le_copy(&addr2, adv_info2.addr);			

    bt_le_ext_adv_delete(adv2);

	zassert_false(bt_addr_le_eq(&addr1, &addr2),
                  "RPAs are unexpectedly equal");
}

ZTEST(bt_psa_integration, test_null_irk_generates_random_irk)
{
	int err;

	int id = bt_id_create(NULL, NULL); // Null IRK
	zassert_true(id >= 0, "ID creation with null IRK failed");

	struct bt_le_ext_adv *adv;
	struct bt_le_adv_param adv_params = {
		.id = id,
		.options = BT_LE_ADV_OPT_CONN,
		.interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
		.interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
	};

	struct bt_le_ext_adv_info adv_info;

	err = bt_le_ext_adv_create(&adv_params, NULL, &adv);
    zassert_equal(err, 0, "Failed to create advertising set: %d", err);

	err = bt_le_ext_adv_get_info(adv, &adv_info);
    zassert_equal(err, 0, "Failed to get advertising info: %d", err);

	bt_le_ext_adv_stop(adv);
	bt_le_ext_adv_delete(adv);

	zassert_true(BT_ADDR_IS_RPA(&adv_info.addr->a), "Generated address is not RPA");
}
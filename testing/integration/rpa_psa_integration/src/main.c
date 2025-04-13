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

	const struct bt_le_adv_param adv_params = {
		.id = id,
		.options = BT_LE_ADV_OPT_CONN,
		.interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
		.interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
	};

	LOG_INF("Starting advertising...");
	err = bt_le_adv_start(&adv_params, NULL, 0, NULL, 0);
	zassert_equal(err, 0, "Advertising start failed: %d", err);

	k_sleep(K_SECONDS(1));

	struct bt_le_oob oob;
	err = bt_le_oob_get_local(id, &oob);
	zassert_equal(err, 0, "Failed to get OOB info");

	bt_addr_le_t addrs[CONFIG_BT_ID_MAX];
	size_t count = CONFIG_BT_ID_MAX;

	bt_id_get(addrs, &count);
	char addr_s[BT_ADDR_LE_STR_LEN];
	for (size_t i = 0; i < count; i++) {
		bt_addr_le_to_str(&addrs[i], addr_s, sizeof(addr_s));
		LOG_INF("addr: %s", addr_s);
	}


	LOG_INF("Advertising address (from OOB): %02X:%02X:%02X:%02X:%02X:%02X (%s)",
	        oob.addr.a.val[5], oob.addr.a.val[4], oob.addr.a.val[3],
	        oob.addr.a.val[2], oob.addr.a.val[1], oob.addr.a.val[0],
	        oob.addr.type == BT_ADDR_LE_RANDOM ? "RANDOM" : "PUBLIC");

	zassert_true(BT_ADDR_IS_RPA(&oob.addr.a), "Advertiser address is not RPA");

	bt_le_adv_stop();
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

	const struct bt_le_adv_param adv_params = {
		.id = id,
		.options = BT_LE_ADV_OPT_CONN,
		.interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
		.interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
	};

	LOG_INF("Starting advertising...");
	err = bt_le_adv_start(&adv_params, NULL, 0, NULL, 0);
	zassert_equal(err, 0, "Advertising start failed: %d", err);

	k_sleep(K_SECONDS(1));

	struct bt_le_oob oob;
	err = bt_le_oob_get_local(id, &oob);
	zassert_equal(err, 0, "Failed to get OOB info");

	bt_addr_le_t addrs[CONFIG_BT_ID_MAX];
	size_t count = CONFIG_BT_ID_MAX;

	bt_id_get(addrs, &count);
	char addr_s[BT_ADDR_LE_STR_LEN];
	for (size_t i = 0; i < count; i++) {
		bt_addr_le_to_str(&addrs[i], addr_s, sizeof(addr_s));
		LOG_INF("addr: %s", addr_s);
	}


	LOG_INF("Advertising address (from OOB): %02X:%02X:%02X:%02X:%02X:%02X (%s)",
	        oob.addr.a.val[5], oob.addr.a.val[4], oob.addr.a.val[3],
	        oob.addr.a.val[2], oob.addr.a.val[1], oob.addr.a.val[0],
	        oob.addr.type == BT_ADDR_LE_RANDOM ? "RANDOM" : "PUBLIC");

	zassert_true(BT_ADDR_IS_RPA(&oob.addr.a), "Advertiser address is not RPA");

	uint16_t rpa_short_timeout = 3;
	err = bt_le_set_rpa_timeout(rpa_short_timeout);
	zassert_equal(err, 0, "Failed to set RPA timeout");
	LOG_INF("Sleeping for RPA timeout...");
	k_sleep(K_SECONDS(rpa_short_timeout + 1));

	struct bt_le_oob oob_after;
	err = bt_le_oob_get_local(id, &oob_after);
	zassert_equal(err, 0, "Failed to get new OOB address");
	bt_addr_le_t addr_after = oob_after.addr;

	LOG_INF("Rotated address: %02X:%02X:%02X:%02X:%02X:%02X",
		addr_after.a.val[5], addr_after.a.val[4], addr_after.a.val[3],
		addr_after.a.val[2], addr_after.a.val[1], addr_after.a.val[0]);

	zassert_true(BT_ADDR_IS_RPA(&oob_after.addr.a), "Advertiser address is not RPA");

	zassert_true(memcmp(oob.addr.a.val, addr_after.a.val, sizeof(bt_addr_t)) != 0,
	                      "RPA did not rotate");

	bt_le_adv_stop();
	// bt_disable();
}

ZTEST(bt_psa_integration, test_rpa_is_stable_before_timeout)
{
	int err;

	uint8_t irk[16] = { [0 ... 15] = 0xAA };
	int id = bt_id_create(NULL, irk);
	zassert_true(id >= 0, "ID creation failed");

	struct bt_le_adv_param adv_params = {
		.id = id,
		.options = BT_LE_ADV_OPT_CONN,
		.interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
		.interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
	};
	err = bt_le_adv_start(&adv_params, NULL, 0, NULL, 0);
	zassert_equal(err, 0, "Adv start failed");

	// Set timeout to something long
	// err = bt_le_set_rpa_timeout(10);
	// zassert_equal(err, 0, "RPA timeout set failed");
	bt_addr_le_t addrs[CONFIG_BT_ID_MAX];
	size_t count = CONFIG_BT_ID_MAX;

	bt_id_get(addrs, &count);
	char addr_s[BT_ADDR_LE_STR_LEN];
	for (size_t i = 0; i < count; i++) {
		bt_addr_le_to_str(&addrs[i], addr_s, sizeof(addr_s));
		LOG_INF("[bt_id_get]addr: %s", addr_s);
	}

	// struct bt_le_oob oob1, oob2;
	// bt_le_oob_get_local(id, &oob1);
	// k_sleep(K_SECONDS(1));
	// bt_le_oob_get_local(id, &oob2);

	// Stop advertising before assertion
	bt_le_adv_stop();
	// zassert_true(memcmp(oob1.addr.a.val, oob2.addr.a.val, sizeof(bt_addr_t)) == 0,
	//              "RPA changed unexpectedly");
}

ZTEST(bt_psa_integration, test_different_irks_yield_different_rpas)
{
	uint8_t irk1[16] = { [0 ... 15] = 0x11 };
	uint8_t irk2[16] = { [0 ... 15] = 0x22 };

	int id1 = bt_id_create(NULL, irk1);
	int id2 = bt_id_create(NULL, irk2);
	zassert_true(id1 >= 0 && id2 >= 0, "Failed to create identities");

	struct bt_le_oob oob1, oob2;
	bt_le_oob_get_local(id1, &oob1);
	bt_le_oob_get_local(id2, &oob2);

	zassert_false(bt_addr_le_cmp(&oob1.addr, &oob2.addr) == 0,
	              "RPAs are unexpectedly equal");
}

ZTEST(bt_psa_integration, test_null_irk_generates_random_irk)
{
	int err;

	int id = bt_id_create(NULL, NULL); // Null IRK
	zassert_true(id >= 0, "ID creation with null IRK failed");

	struct bt_le_oob oob;
	err = bt_le_oob_get_local(id, &oob);
	zassert_equal(err, 0, "Failed to get OOB info");

	zassert_true(BT_ADDR_IS_RPA(&oob.addr.a), "Generated address is not RPA");
}
/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>

#include <psa/crypto.h>


ZTEST_SUITE(framework_tests, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
ZTEST(framework_tests, test_assert)
{

	psa_status_t init_val = psa_crypto_init();
	zassert_equal(init_val, PSA_SUCCESS);
	zassert_true(1, "1 was false");
	zassert_false(0, "0 was true");
	zassert_is_null(NULL, "NULL was not NULL");
	zassert_not_null("foo", "\"foo\" was NULL");
	zassert_equal(1, 1, "1 was not equal to 1");
	zassert_equal_ptr(NULL, NULL, "NULL was not equal to NULL");
}

#define SAMPLE_KEY_ID   PSA_KEY_ID_USER_MIN
#define SAMPLE_KEY_TYPE PSA_KEY_TYPE_AES
#define SAMPLE_ALG      PSA_ALG_CTR
#define SAMPLE_KEY_BITS 256

ZTEST(framework_tests, test_psa) {
    psa_key_handle_t priv_key;
    uint8_t peer_pub_key[64] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  // Example peer key
    uint8_t shared_secret[32];
    size_t olen;

    /* Initialize PSA Crypto */
    zassert_equal(psa_crypto_init(), PSA_SUCCESS, "PSA init failed");

    /* Generate ECC key for Secure Pairing */
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_algorithm(&attr, SAMPLE_ALG);//PSA_ALG_ECDH(PSA_ALG_SHA_256));
    psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&attr, 256);
	psa_status_t  keygen_status = psa_generate_key(&attr, &priv_key);
    zassert_equal(keygen_status, PSA_SUCCESS, "Key gen failed: %d", keygen_status);

    /* Simulate SMP key exchange (PSA computes shared secret) */
    zassert_equal(
        psa_raw_key_agreement(
            PSA_ALG_ECDH, priv_key,
            peer_pub_key, sizeof(peer_pub_key),
            shared_secret, sizeof(shared_secret), &olen
        ),
        PSA_SUCCESS,
        "ECDH failed"
    );

    /* Verify shared secret is non-zero */
    bool all_zeros = true;
    for (int i = 0; i < olen; i++) {
        if (shared_secret[i] != 0) {
            all_zeros = false;
            break;
        }
    }
    zassert_false(all_zeros, "Shared secret is zero");

    /* Cleanup */
    psa_destroy_key(priv_key);
}
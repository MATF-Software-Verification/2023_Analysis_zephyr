/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <psa/crypto.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>

#define SAMPLE_KEY_ID   PSA_KEY_ID_USER_MIN
#define SAMPLE_KEY_TYPE PSA_KEY_TYPE_AES
#define SAMPLE_ALG      PSA_ALG_CTR
#define SAMPLE_KEY_BITS 256

#define NUM_REPETITIONS 1000

LOG_MODULE_REGISTER(psa_hci_integration_tests);

psa_status_t generate_key_helper(psa_key_id_t* key_id, psa_algorithm_t algorithm, size_t key_bits);

void setup_pre_reboot(){
    // Use this function as a setup for the next testsuite that should simulate reboot
    psa_status_t ret;
    psa_key_id_t key_id;
    ret = generate_key_helper(&key_id, SAMPLE_ALG, SAMPLE_KEY_BITS);
    if(ret != PSA_SUCCESS){
	    printk("Key generation failed: %d", ret);
	}

    // Don't destroy the key, but purge it for good measure
    ret = psa_purge_key(SAMPLE_KEY_ID);
    if(ret != PSA_SUCCESS){
		printk("Failed to purge the key: %d", ret);
	}
}

static uint8_t plaintext[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};
static uint8_t ciphertext[PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(SAMPLE_KEY_TYPE, SAMPLE_ALG,
    sizeof(plaintext))];
static size_t ct_len;

psa_status_t generate_key_helper(psa_key_id_t* key_id, psa_algorithm_t algorithm, size_t key_bits) {
	psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

	psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_PERSISTENT);
	psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
	psa_set_key_id(&key_attributes, SAMPLE_KEY_ID);
	psa_set_key_type(&key_attributes, SAMPLE_KEY_TYPE);
	psa_set_key_algorithm(&key_attributes, algorithm);
	psa_set_key_bits(&key_attributes, key_bits);

    return psa_generate_key(&key_attributes, key_id);
}

/*
ZTEST(psa_hci_integration_tests, test_psa_persistence_encrypt_decrypt) {
    psa_status_t ret;
	psa_key_id_t key_id;

    // Create a persistent signing key
	ret = generate_key_helper(&key_id, SAMPLE_ALG, SAMPLE_KEY_BITS);
    zassert_equal(ret, PSA_SUCCESS, "Key gen failed: %d", ret);

    ret = psa_cipher_encrypt(key_id, SAMPLE_ALG, plaintext, sizeof(plaintext), ciphertext, sizeof(ciphertext), &ct_len);

    LOG_INF("Encrypted %d bytes", ct_len);

    zassert_equal(ret, PSA_SUCCESS, "Encryption failed: %d", ret);

    // Purge key from volatile storage
    ret = psa_purge_key(SAMPLE_KEY_ID);
    zassert_equal(ret, PSA_SUCCESS, "Failed to purge the key: %d", ret);

        //Open persisted key
        ret = psa_open_key(SAMPLE_KEY_ID, &key_id);
        zassert_equal(ret, PSA_SUCCESS, "Open failed: %d", ret);

    uint8_t decrypted[sizeof(plaintext)];
    size_t dlen;

        // Verify key usability
    ret = psa_cipher_decrypt(
        key_id, SAMPLE_ALG,
        ciphertext, ct_len,
        decrypted, sizeof(decrypted), &dlen
    );
    zassert_equal(ret, PSA_SUCCESS, "Decrypt failed: %d", ret);
    zassert_mem_equal(plaintext, decrypted, sizeof(plaintext),
                    "Data corrupted");

                         Cleanup
    psa_destroy_key(key_id);
}


ZTEST(psa_hci_integration_tests, test_psa_key_deletion_and_regeneration) {
    psa_status_t ret;
    psa_key_id_t key_id;

    // Generate the key
    ret = generate_key_helper(&key_id, SAMPLE_ALG, SAMPLE_KEY_BITS);
    zassert_equal(ret, PSA_SUCCESS, "Key generation failed: %d", ret);

    // Encrypt some data with the key
    ret = psa_cipher_encrypt(key_id, SAMPLE_ALG, plaintext, sizeof(plaintext), ciphertext, sizeof(ciphertext), &ct_len);
    zassert_equal(ret, PSA_SUCCESS, "Encryption failed: %d", ret);

    // Destroy the key
    ret = psa_destroy_key(key_id);
    zassert_equal(ret, PSA_SUCCESS, "Failed to destroy key: %d", ret);

    // Attempt to re-generate the key
    ret = generate_key_helper(&key_id, SAMPLE_ALG, SAMPLE_KEY_BITS);
    zassert_equal(ret, PSA_SUCCESS, "Key re-generation failed: %d", ret);

    // Attempt to decrypt the ciphertext encrypted using the different key (should fail)
    uint8_t decrypted[sizeof(plaintext)];
    size_t dlen;
    ret = psa_cipher_decrypt(key_id, SAMPLE_ALG, ciphertext, ct_len, decrypted, sizeof(decrypted), &dlen);
    zassert_equal(ret, PSA_SUCCESS, "Decryption failed: %d", ret);
    zassert_true(memcmp(plaintext, decrypted, sizeof(plaintext)) != 0, "Decryption succeeded with a different key!");

    // Cleanup
    psa_destroy_key(key_id);
}*/

int main(int argc, char* argv[]) {
	size_t key_bits = 256;
	psa_key_id_t key_id;

	char *key_size_str = getenv("KEY_SIZE");

	if (key_size_str != NULL){
		key_bits = atoi(key_size_str);
	}

	if(key_bits != 128 && key_bits != 256){
		printk("Invalid key size: %d! Only 128 or 256 are supported!\n", key_bits);
		exit(-1);
	}

	generate_key_helper(&key_id, SAMPLE_ALG, key_bits);
	//printk("Starting performance test [encryption]...\n");
	int ret;
	for (int i = 0; i < NUM_REPETITIONS; i++){
		//printk("Running encryption %d/%d\n", i, NUM_REPETITIONS);
		ret = psa_cipher_encrypt(key_id, SAMPLE_ALG, plaintext, sizeof(plaintext), ciphertext, sizeof(ciphertext), &ct_len);
		if(PSA_SUCCESS != ret){
			printk("Encryption failed: %d!\n", ret);
		}
	}
	//printk("Performance test ended [encryption]\n");

	psa_destroy_key(key_id);
	exit(0);

	return 0;
}

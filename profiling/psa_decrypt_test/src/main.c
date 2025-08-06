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

int main(int argc, char* argv[]) {
	size_t key_bits = 256;
	psa_key_id_t key_id;

	int ret;
	uint8_t plaintext[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};
	uint8_t ciphertext[32];
	size_t ciphertext_len;
	uint8_t decryptoed_output[32];
	size_t decrypted_len;

	char *key_size_str = getenv("KEY_SIZE");
	if (key_size_str != NULL){
		key_bits = atoi(key_size_str);
	}

	if(key_bits != 128 && key_bits != 256){
		printk("Invalid key size: %d! Only 128 or 256 are supported!\n", key_bits);
		exit(-1);
	}

	generate_key_helper(&key_id, SAMPLE_ALG, key_bits);

	ret = psa_cipher_encrypt(key_id, SAMPLE_ALG, plaintext, sizeof(plaintext), ciphertext, sizeof(ciphertext), &ciphertext_len);

	//printk("Starting performance test [decryption]...\n");
	for (int i = 0; i < NUM_REPETITIONS; i++){
		//printk("Running encryption %d/%d\n", i, NUM_REPETITIONS);
		ret = psa_cipher_decrypt(key_id, SAMPLE_ALG, ciphertext, ciphertext_len, decrypted_output, sizeof(decrypted_output), &decrypted_len);
		if(PSA_SUCCESS != ret){
			printk("Encryption failed: %d!\n", ret);
		}
	}
	//printk("Performance test ended [decryption]\n");

	psa_destroy_key(key_id);
	exit(0);

	return 0;
}

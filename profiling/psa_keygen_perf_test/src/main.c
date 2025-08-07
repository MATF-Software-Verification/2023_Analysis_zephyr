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

int main(int argc, char *argv[]) {
	psa_key_id_t key_id;
	size_t key_bits = 128;
	const char* algo_name = "CTR";
	psa_algorithm_t psa_alg = PSA_ALG_CTR;

	char* key_size_str = getenv("KEY_SIZE");
	if (key_size_str != NULL) {
		key_bits = atoi(key_size_str);
	}

	char* algo_flavor_str = getenv("ALGO_FLAVOR");
	if (algo_flavor_str != NULL) {
		if (strcmp(algo_flavor_str, "GCM") == 0){
			psa_alg = PSA_ALG_GCM;
			algo_name = "GCM";
		}
	}

	printk("Starting performance test [key generation]...\n");
	printk("  - Key %zu\n", key_bits);
	printk("  - Algo %s\n", (psa_alg == PSA_ALG_GCM) ? "GCM" : "CTR");
	for (int i = 0; i < NUM_REPETITIONS; i++){
		generate_key_helper(&key_id, SAMPLE_ALG, SAMPLE_KEY_BITS);
		psa_destroy_key(key_id);
	}
	printk("Performance test ended [key generation]\n");

	exit(0);

	return 0;
}

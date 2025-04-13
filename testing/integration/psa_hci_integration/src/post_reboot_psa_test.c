/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #include <zephyr/ztest.h>
 #include <psa/crypto.h>
 #include <zephyr/logging/log.h>
 
 LOG_MODULE_REGISTER(psa_hci_reboot_integration_tests);

 ZTEST_SUITE(psa_hci_reboot_integration_tests, NULL, NULL, NULL, NULL, NULL);
 
 #define SAMPLE_KEY_ID   PSA_KEY_ID_USER_MIN
 #define SAMPLE_KEY_TYPE PSA_KEY_TYPE_AES
 #define SAMPLE_ALG      PSA_ALG_CTR
 #define SAMPLE_KEY_BITS 256
 
 static uint8_t plaintext[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};
 static uint8_t ciphertext[PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(SAMPLE_KEY_TYPE, SAMPLE_ALG,
     sizeof(plaintext))];
 static size_t ct_len;
  
 ZTEST(psa_hci_reboot_integration_tests, test_psa_persistence_after_reboot) {
     psa_status_t ret;
     psa_key_id_t key_id;
 
     ret = psa_open_key(SAMPLE_KEY_ID, &key_id);
     zassert_equal(ret, PSA_SUCCESS, "Open failed: %d", ret);
 
     // Encrypt the data
     ret = psa_cipher_encrypt(key_id, SAMPLE_ALG, plaintext, sizeof(plaintext), ciphertext, sizeof(ciphertext), &ct_len);
     zassert_equal(ret, PSA_SUCCESS, "Encryption failed: %d", ret);
  
     // Decrypt the ciphertext to verify persistence
     uint8_t decrypted[sizeof(plaintext)];
     size_t dlen;
     ret = psa_cipher_decrypt(key_id, SAMPLE_ALG, ciphertext, ct_len, decrypted, sizeof(decrypted), &dlen);
     zassert_equal(ret, PSA_SUCCESS, "Decryption failed after reboot: %d", ret);
     zassert_mem_equal(plaintext, decrypted, sizeof(plaintext), "Decryption resulted in corrupted data after reboot");
 
     // Cleanup
     psa_destroy_key(key_id);
 }
 
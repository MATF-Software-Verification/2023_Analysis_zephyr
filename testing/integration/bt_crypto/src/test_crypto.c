// test_crypto.c
#include <zephyr/bluetooth/crypto.h>
#include <zephyr/ztest.h>
// #include "mocks/aes.h"

ZTEST_SUITE(bt_host_crypto_test, NULL, NULL, NULL, NULL, NULL);

ZTEST(bt_host_crypto_test, test_null_ct) {
  const uint8_t key[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
                         0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};

  const uint8_t pt[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
                        0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};

  zassert_equal(-EINVAL, bt_encrypt_le(key, pt, NULL));
}

ZTEST(bt_host_crypto_test, test_null_pt) {
  const uint8_t key[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
                         0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};

  uint8_t ct[16];

  zassert_equal(-EINVAL, bt_encrypt_le(key, NULL, ct));
}

ZTEST(bt_host_crypto_test, test_null_key) {
  const uint8_t pt[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
                        0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};

  uint8_t ct[16];

  zassert_equal(-EINVAL, bt_encrypt_le(NULL, pt, ct));
}

ZTEST(bt_host_crypto_test, test_test_ok) {
  const uint8_t key[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
                         0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};

  const uint8_t pt[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
                        0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};

  uint8_t ct[16];

//   const uint8_t expected_ct[] = {0xd7, 0xe5, 0xe0, 0x8c, 0x58, 0x8e,
//                                  0xe9, 0x67, 0xdf, 0x9c, 0x32, 0x6e,
//                                  0xce, 0xc3, 0xee, 0x7e};

//   zassert_mem_equal(ct, expected_ct, sizeof(ct),
//                     "Expected and actual CTs don't match!");
  zassert_equal(0, bt_encrypt_le(key, pt, ct));
}
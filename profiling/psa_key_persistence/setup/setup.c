#include <psa/crypto.h>
#include <zephyr/kernel.h>

#include <stdlib.h>

#define PERSISTENT_KEY_ID (PSA_KEY_ID_USER_MIN + 1)

int main(void){

	psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
	psa_key_id_t key_id;

	psa_crypto_init();

	psa_set_key_id(&attributes, PERSISTENT_KEY_ID);
	psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_PERSISTENT);
	psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
	psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
	psa_set_key_bits(&attributes, 256);

	psa_destroy_key(PERSISTENT_KEY_ID);

	psa_status_t status = psa_generate_key(&attributes, &key_id);
	if(status != PSA_SUCCESS) {
		printk("Falied to create persistent key! Status: %d\n", status);
	} else {
		printk("Persistent key created successfully.\n");
	}

	exit(0);
	return 0;
}

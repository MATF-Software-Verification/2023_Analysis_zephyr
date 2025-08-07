#include <psa/crypto.h>
#include <zephyr/kernel.h>

#define PERSISTENT_KEY_ID (PSA_KEY_ID_USER_MIN + 1)

#define NUM_REPETITIONS 1000

int main(void){

	psa_key_id_t key_handle;

	psa_crypto_init();
	printk("Starting key loading performance test...\n");

	for (int i = 0; i < NUM_REPETITIONS; i++){
		psa_status_t status = psa_open_key(PERSISTENT_KEY_ID, &key_handle);
		if(status != PSA_SUCCESS){
			printk("Failed to open the key: %d\n!", (int)status);
			break;
		}

		psa_close_key(key_handle);
	}

	printk("Test completed.\n");
	exit(0);
	return 0;
}

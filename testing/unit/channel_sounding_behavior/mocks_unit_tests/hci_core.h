/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/fff.h>
#include <zephyr/kernel.h>

/* List of fakes used by this unit tester */
#define HCI_CORE_FFF_FAKES_LIST(FAKE)                                                              \
	FAKE(bt_hci_cmd_create)                                                                    \
	FAKE(bt_hci_cmd_send_sync)

DECLARE_FAKE_VALUE_FUNC(struct net_buf *, bt_hci_cmd_create, uint16_t, uint8_t);
DECLARE_FAKE_VALUE_FUNC(int, bt_hci_cmd_send_sync, uint16_t, struct net_buf *, struct net_buf **);

// Insert real function declarations - these functions are defined in hci_core.h, but implemented in cs.c that is our module-under-test
void bt_hci_le_cs_read_remote_supported_capabilities_complete(struct net_buf *buf);
// void bt_hci_le_cs_read_remote_fae_table_complete(struct net_buf *buf);

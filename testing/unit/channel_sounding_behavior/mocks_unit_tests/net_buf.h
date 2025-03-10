/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/fff.h>
#include <zephyr/kernel.h>
#include <zephyr/net_buf.h>

/* List of fakes used by this unit tester */
#define NET_BUF_FFF_FAKES_LIST(FAKE)                                                               \
	FAKE(net_buf_unref)                                                                        \
	FAKE(net_buf_reset)                                                                        \
	FAKE(net_buf_slist_put)                                                                    \
	FAKE(net_buf_alloc_fixed)																   \
	FAKE(net_buf_simple_add_mem)																   \
	FAKE(net_buf_simple_pull_mem)																   \
	FAKE(net_buf_simple_tailroom)																   \
	FAKE(net_buf_simple_init_with_data)																   \
	FAKE(net_buf_simple_pull_u8)																   \
	FAKE(net_buf_simple_pull)																   \
	FAKE(net_buf_simple_add)																	\
	FAKE(net_buf_simple_add_u8) 																\
	FAKE(net_buf_simple_add_le32)


DECLARE_FAKE_VOID_FUNC(net_buf_unref, struct net_buf *);
DECLARE_FAKE_VOID_FUNC(net_buf_reset, struct net_buf *);
DECLARE_FAKE_VOID_FUNC(net_buf_slist_put, sys_slist_t *, struct net_buf *);
DECLARE_FAKE_VALUE_FUNC(struct net_buf *, net_buf_alloc_fixed, struct net_buf_pool *, k_timeout_t);
DECLARE_FAKE_VALUE_FUNC(void *, net_buf_simple_add, struct net_buf_simple *, size_t);
DECLARE_FAKE_VALUE_FUNC(uint8_t *, net_buf_simple_add_u8, struct net_buf_simple *, uint8_t);
DECLARE_FAKE_VOID_FUNC(net_buf_simple_add_le32, struct net_buf_simple *, uint32_t);
DECLARE_FAKE_VALUE_FUNC(void *, net_buf_simple_add_mem, struct net_buf_simple *, const void *, size_t);
DECLARE_FAKE_VALUE_FUNC(void *, net_buf_simple_pull_mem, struct net_buf_simple *, size_t);
DECLARE_FAKE_VALUE_FUNC(size_t, net_buf_simple_tailroom, const struct net_buf_simple *);
DECLARE_FAKE_VOID_FUNC(net_buf_simple_init_with_data, struct net_buf_simple *, void *, size_t);
DECLARE_FAKE_VALUE_FUNC(uint8_t, net_buf_simple_pull_u8, struct net_buf_simple *);
DECLARE_FAKE_VALUE_FUNC(void *, net_buf_simple_pull, struct net_buf_simple *, size_t);
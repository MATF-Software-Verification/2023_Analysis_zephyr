/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/net_buf.h>

#include "mocks_unit_tests/net_buf.h"

const struct net_buf_data_cb net_buf_fixed_cb;

DEFINE_FAKE_VOID_FUNC(net_buf_unref, struct net_buf *);
DEFINE_FAKE_VOID_FUNC(net_buf_reset, struct net_buf *);
DEFINE_FAKE_VOID_FUNC(net_buf_slist_put, sys_slist_t *, struct net_buf *);
DEFINE_FAKE_VALUE_FUNC(struct net_buf *, net_buf_alloc_fixed, struct net_buf_pool *, k_timeout_t);
DEFINE_FAKE_VALUE_FUNC(void *, net_buf_simple_add, struct net_buf_simple *, size_t);
DEFINE_FAKE_VALUE_FUNC(void *, net_buf_simple_add_mem, struct net_buf_simple *, const void *, size_t);
DEFINE_FAKE_VALUE_FUNC(void *, net_buf_simple_pull_mem, struct net_buf_simple *, size_t);
DEFINE_FAKE_VALUE_FUNC(size_t, net_buf_simple_tailroom, const struct net_buf_simple *);
DEFINE_FAKE_VOID_FUNC(net_buf_simple_init_with_data, struct net_buf_simple *, void *, size_t);
DEFINE_FAKE_VALUE_FUNC(uint8_t, net_buf_simple_pull_u8, struct net_buf_simple *);
DEFINE_FAKE_VALUE_FUNC(void *, net_buf_simple_pull, struct net_buf_simple *, size_t);
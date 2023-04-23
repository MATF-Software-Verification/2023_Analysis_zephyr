/* Core kernel objects */
case K_OBJ_MEM_SLAB: ret = "k_mem_slab"; break;
case K_OBJ_MSGQ: ret = "k_msgq"; break;
case K_OBJ_MUTEX: ret = "k_mutex"; break;
case K_OBJ_PIPE: ret = "k_pipe"; break;
case K_OBJ_QUEUE: ret = "k_queue"; break;
case K_OBJ_POLL_SIGNAL: ret = "k_poll_signal"; break;
case K_OBJ_SEM: ret = "k_sem"; break;
case K_OBJ_STACK: ret = "k_stack"; break;
case K_OBJ_THREAD: ret = "k_thread"; break;
case K_OBJ_TIMER: ret = "k_timer"; break;
case K_OBJ_THREAD_STACK_ELEMENT: ret = "z_thread_stack_element"; break;
case K_OBJ_NET_SOCKET: ret = "NET_SOCKET"; break;
case K_OBJ_NET_IF: ret = "net_if"; break;
case K_OBJ_SYS_MUTEX: ret = "sys_mutex"; break;
case K_OBJ_FUTEX: ret = "k_futex"; break;
case K_OBJ_CONDVAR: ret = "k_condvar"; break;
#ifdef CONFIG_EVENTS
case K_OBJ_EVENT: ret = "k_event"; break;
#endif
#ifdef CONFIG_ZTEST
case K_OBJ_ZTEST_SUITE_NODE: ret = "ztest_suite_node"; break;
#endif
#ifdef CONFIG_ZTEST
case K_OBJ_ZTEST_SUITE_STATS: ret = "ztest_suite_stats"; break;
#endif
#ifdef CONFIG_ZTEST_NEW_API
case K_OBJ_ZTEST_UNIT_TEST: ret = "ztest_unit_test"; break;
#endif
#ifdef CONFIG_ZTEST_NEW_API
case K_OBJ_ZTEST_TEST_RULE: ret = "ztest_test_rule"; break;
#endif
#ifdef CONFIG_RTIO
case K_OBJ_RTIO: ret = "rtio"; break;
#endif
#ifdef CONFIG_RTIO
case K_OBJ_RTIO_IODEV: ret = "rtio_iodev"; break;
#endif
/* Driver subsystems */

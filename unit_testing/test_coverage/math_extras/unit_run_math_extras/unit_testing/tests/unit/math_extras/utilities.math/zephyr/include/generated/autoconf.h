#define CONFIG_HAS_DTS 1
#define CONFIG_ROM_START_OFFSET 0x0
#define CONFIG_BOARD_REVISION ""
#define CONFIG_BOARD_UNIT_TESTING 1
#define CONFIG_SOC_UNIT_TESTING 1
#define CONFIG_SOC_LOG_LEVEL_INF 1
#define CONFIG_SOC_LOG_LEVEL 3
#define CONFIG_X86 1
#define CONFIG_NATIVE_POSIX_CONSOLE 1
#define CONFIG_ARCH_IS_SET 1
#define CONFIG_ARCH_LOG_LEVEL_INF 1
#define CONFIG_ARCH_LOG_LEVEL 3
#define CONFIG_LITTLE_ENDIAN 1
#define CONFIG_SRAM_SIZE 0
#define CONFIG_SRAM_BASE_ADDRESS 0x0
#define CONFIG_FLASH_SIZE 0
#define CONFIG_FLASH_BASE_ADDRESS 0x0
#define CONFIG_SIMPLIFIED_EXCEPTION_CODES 1
#define CONFIG_ARCH_HAS_TIMING_FUNCTIONS 1
#define CONFIG_ARCH_SUPPORTS_COREDUMP 1
#define CONFIG_ARCH_HAS_GDBSTUB 1
#define CONFIG_ARCH_HAS_THREAD_LOCAL_STORAGE 1
#define CONFIG_CPU_HAS_MMU 1
#define CONFIG_ARCH_HAS_DEMAND_PAGING 1
#define CONFIG_TOOLCHAIN_HAS_BUILTIN_FFS 1
#define CONFIG_KERNEL_LOG_LEVEL_INF 1
#define CONFIG_KERNEL_LOG_LEVEL 3
#define CONFIG_MULTITHREADING 1
#define CONFIG_NUM_COOP_PRIORITIES 16
#define CONFIG_NUM_PREEMPT_PRIORITIES 15
#define CONFIG_MAIN_THREAD_PRIORITY 0
#define CONFIG_COOP_ENABLED 1
#define CONFIG_PREEMPT_ENABLED 1
#define CONFIG_PRIORITY_CEILING -127
#define CONFIG_NUM_METAIRQ_PRIORITIES 0
#define CONFIG_MAIN_STACK_SIZE 1024
#define CONFIG_IDLE_STACK_SIZE 256
#define CONFIG_ISR_STACK_SIZE 2048
#define CONFIG_ERRNO 1
#define CONFIG_SCHED_DUMB 1
#define CONFIG_WAITQ_DUMB 1
#define CONFIG_BOOT_BANNER 1
#define CONFIG_BOOT_DELAY 0
#define CONFIG_THREAD_NAME 1
#define CONFIG_THREAD_MAX_NAME_LEN 32
#define CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE 4096
#define CONFIG_SYSTEM_WORKQUEUE_PRIORITY -1
#define CONFIG_ATOMIC_OPERATIONS_BUILTIN 1
#define CONFIG_TIMESLICING 1
#define CONFIG_TIMESLICE_SIZE 0
#define CONFIG_TIMESLICE_PRIORITY 0
#define CONFIG_NUM_MBOX_ASYNC_MSGS 10
#define CONFIG_KERNEL_MEM_POOL 1
#define CONFIG_HEAP_MEM_POOL_SIZE 0
#define CONFIG_SYS_CLOCK_TICKS_PER_SEC 100
#define CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC 1000000
#define CONFIG_SYS_CLOCK_EXISTS 1
#define CONFIG_TIMEOUT_64BIT 1
#define CONFIG_SYS_CLOCK_MAX_TIMEOUT_DAYS 365
#define CONFIG_KERNEL_INIT_PRIORITY_OBJECTS 30
#define CONFIG_KERNEL_INIT_PRIORITY_DEFAULT 40
#define CONFIG_KERNEL_INIT_PRIORITY_DEVICE 50
#define CONFIG_APPLICATION_INIT_PRIORITY 90
#define CONFIG_MP_NUM_CPUS 1
#define CONFIG_MP_MAX_NUM_CPUS 1
#define CONFIG_CONSOLE 1
#define CONFIG_CONSOLE_INPUT_MAX_LINE_LEN 128
#define CONFIG_CONSOLE_HAS_DRIVER 1
#define CONFIG_CONSOLE_INIT_PRIORITY 40
#define CONFIG_NATIVE_POSIX_STDOUT_CONSOLE 1
#define CONFIG_NATIVE_POSIX_CONSOLE_INIT_PRIORITY 99
#define CONFIG_UART_CONSOLE_LOG_LEVEL_INF 1
#define CONFIG_UART_CONSOLE_LOG_LEVEL 3
#define CONFIG_ETH_INIT_PRIORITY 80
#define CONFIG_INTC_INIT_PRIORITY 40
#define CONFIG_INTC_LOG_LEVEL_INF 1
#define CONFIG_INTC_LOG_LEVEL 3
#define CONFIG_SYSTEM_CLOCK_INIT_PRIORITY 0
#define CONFIG_USBC_LOG_LEVEL_INF 1
#define CONFIG_USBC_LOG_LEVEL 3
#define CONFIG_SUPPORT_MINIMAL_LIBC 1
#define CONFIG_EXTERNAL_LIBC 1
#define CONFIG_CRC 1
#define CONFIG_CBPRINTF_COMPLETE 1
#define CONFIG_CBPRINTF_FULL_INTEGRAL 1
#define CONFIG_CBPRINTF_N_SPECIFIER 1
#define CONFIG_CBPRINTF_PACKAGE_LOG_LEVEL_INF 1
#define CONFIG_CBPRINTF_PACKAGE_LOG_LEVEL 3
#define CONFIG_SYS_HEAP_ALLOC_LOOPS 3
#define CONFIG_SYS_HEAP_SMALL_ONLY 1
#define CONFIG_POSIX_MAX_FDS 4
#define CONFIG_MAX_TIMER_COUNT 5
#define CONFIG_TIMER_CREATE_WAIT 100
#define CONFIG_PRINTK 1
#define CONFIG_EARLY_CONSOLE 1
#define CONFIG_ASSERT_VERBOSE 1
#define CONFIG_LOG 1
#define CONFIG_LOG_CORE_INIT_PRIORITY 0
#define CONFIG_LOG_MODE_MINIMAL 1
#define CONFIG_LOG_DEFAULT_MINIMAL 1
#define CONFIG_LOG_DEFAULT_LEVEL 3
#define CONFIG_LOG_OVERRIDE_LEVEL 0
#define CONFIG_LOG_MAX_LEVEL 4
#define CONFIG_LOG_TEST_CLEAR_MESSAGE_SPACE 1
#define CONFIG_LOG_USE_VLA 1
#define CONFIG_LOG_ALWAYS_RUNTIME 1
#define CONFIG_LOG_DOMAIN_NAME ""
#define CONFIG_ZTEST 1
#define CONFIG_ZTEST_NEW_API 1
#define CONFIG_ZTEST_STACK_SIZE 1024
#define CONFIG_ZTEST_TEST_DELAY_MS 0
#define CONFIG_ZTEST_CPU_HOLD_TIME_MS 3000
#define CONFIG_ZTEST_ASSERT_VERBOSE 1
#define CONFIG_ZTEST_THREAD_PRIORITY -1
#define CONFIG_ZTEST_VERIFY_RUN_ALL 1
#define CONFIG_ZTEST_SUMMARY 1
#define CONFIG_ZTEST_VERBOSE_OUTPUT 1
#define CONFIG_ZTEST_VERBOSE_SUMMARY 1
#define CONFIG_ZTEST_FAIL_ON_ASSUME 1
#define CONFIG_TEST_LOGGING_FLUSH_AFTER_TEST 1
#define CONFIG_ZTEST_MOCKING 1
#define CONFIG_ZTEST_PARAMETER_COUNT 10
#define CONFIG_TEST 1
#define CONFIG_TEST_EXTRA_STACK_SIZE 0
#define CONFIG_HAS_COVERAGE_SUPPORT 1
#define CONFIG_COVERAGE 1
#define CONFIG_COVERAGE_GCOV_HEAP_SIZE 32768
#define CONFIG_TEST_LOGGING_DEFAULTS 1
#define CONFIG_LINKER_ORPHAN_SECTION_WARN 1
#define CONFIG_LD_LINKER_SCRIPT_SUPPORTED 1
#define CONFIG_LD_LINKER_TEMPLATE 1
#define CONFIG_KERNEL_ENTRY "__start"
#define CONFIG_LINKER_SORT_BY_ALIGNMENT 1
#define CONFIG_SRAM_OFFSET 0x0
#define CONFIG_LINKER_GENERIC_SECTIONS_PRESENT_AT_BOOT 1
#define CONFIG_LINKER_USE_RELAX 1
#define CONFIG_NATIVE_APPLICATION 1
#define CONFIG_NO_OPTIMIZATIONS 1
#define CONFIG_COMPILER_WARNINGS_AS_ERRORS 1
#define CONFIG_COMPILER_COLOR_DIAGNOSTICS 1
#define CONFIG_FORTIFY_SOURCE_NONE 1
#define CONFIG_COMPILER_OPT ""
#define CONFIG_RUNTIME_ERROR_CHECKS 1
#define CONFIG_KERNEL_BIN_NAME "zephyr"
#define CONFIG_OUTPUT_STAT 1
#define CONFIG_OUTPUT_SYMBOLS 1
#define CONFIG_OUTPUT_DISASSEMBLY 1
#define CONFIG_OUTPUT_PRINT_MEMORY_USAGE 1
#define CONFIG_BUILD_OUTPUT_BIN 1
#define CONFIG_WARN_DEPRECATED 1
#define CONFIG_ENFORCE_ZEPHYR_STDINT 1
#define CONFIG_COMPAT_INCLUDES 1

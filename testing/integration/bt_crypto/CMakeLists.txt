cmake_minimum_required(VERSION 3.20)
set(BOARD native_posix_64)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(bt_crypto_tests)

target_sources(app PRIVATE src/test_crypto.c)

target_link_libraries(app PRIVATE zephyr)

# Ensure the Zephyr include directory is included
include_directories($ENV{ZEPHYR_BASE}/include)

# include_directories($ENV{ZEPHYR_BASE}/tests/bluetooth/host/crypto)
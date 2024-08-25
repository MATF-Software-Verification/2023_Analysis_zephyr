# 2023_Analysis_zephyr

This repository "Software Verification" course at the Faculty of Mathematics in Belgrade.

Techniques and tools used:
- Unit testing via [twister](https://docs.zephyrproject.org/3.1.0/develop/test/twister.html) tool
- Valgrind memcheck
- Static analysis
- Adress sanitizer

Repo organization:

```
.
├── README.md
├── patches
│   └── bt_crypto.patch
├── testing
│   ├── integration
│   │   └── bt_crypto
│   └── unit
│       ├── bt_encrypt_decrypt
│       └── run_unit_tests.sh
└── zephyr @ 419dda3ed72
    .
    .
    .
    ├── samples
    │   ├── application_development
    │   ├── arch
    │   ├── basic
    │   ├── bluetooth
    │   ├── boards
    │   ├── classic.rst
    │   ├── compression
    │   ├── cpp
    │   ├── drivers
    │   ├── hello_world
    │   ├── index.rst
    │   ├── kernel
    │   ├── modules
    │   ├── net
    │   ├── philosophers
    │   ├── posix
    │   ├── sample_definition_and_criteria.rst
    │   ├── sensor
    │   ├── shields
    │   ├── subsys
    │   ├── synchronization
    │   ├── tfm_integration
    │   └── userspace
    .
    .
    .
    ├── subsys
    │   ├── CMakeLists.txt
    │   ├── Kconfig
    │   ├── bluetooth
    │   .
    │   .
    │   .
    │   └── usb
    ├── tests
    │   ├── application_development
    │   ├── arch
    │   ├── benchmarks
    │   ├── bluetooth
    │   ├── boards
    │   ├── boot
    │   ├── cmake
    │   ├── crypto
    │   ├── drivers
    │   ├── kernel
    │   ├── lib
    │   ├── misc
    │   ├── net
    │   ├── posix
    │   ├── subsys
    │   ├── unit
    │   └── ztest
    ├── version.h.in
    ├── west.yml
    ├── zephyr-env.cmd
    └── zephyr-env.sh
```

### Initializing zephyr submodule

From the root directory of the project run the following:

```
git submodule init
git submodule update
```

#### Install zephyr tools

Zephyr-project's west tool is used for updating the zephyr repository, building, flashing zephyr apps etc. It can be installed using following commands:

```
   pip3 install --user -U west
   echo 'export PATH=~/.local/bin:"$PATH"' >> ~/.bashrc
   source ~/.bashrc
```

#### Install additional requirements

```
cd zephyr
pip3 install --user -r scripts/requirements.txt
```

### Install the zephyr SDK

In order to use all of the required tools for testing and building zephyr applications, SDK is needed. It can be downloaded using following commands:

#### Set up the environment for SDK installation

```
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
export ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk
. ./zephyr-env.sh
```

#### Download and install the SDK

```
cd ~
wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.0/zephyr-sdk-0.16.0_linux-x86_64.tar.xz
wget -O - https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.0/sha256.sum | shasum --check --ignore-missing
```

Extract and install the SDK:

```
tar xvf zephyr-sdk-0.16.0_linux-x86_64.tar.xz
cd zephyr-sdk-0.16.0
./setup.sh
```

### Initialize zephyr

From the root project directory run the following in order to initialize zephyr build system

`west init -l ./zephyr`

`west update`

### System configuration

Since some of the targets require 32bit libraries, there are some additional system setup needed.

#### Install 32 bit libraries for building on 64bit system

`sudo apt-get install gcc-multilib g++-multilib`

#### Install 32 bit libraries for Valgrind

`sudo apt-get install libc6-dbg libc6-dbg:i386`

#### Install ASAN

`sudo apt-get install libasan6`

Compiler used for the project is `gcc-9` on `wsl` version of `Ubuntu22.04`.
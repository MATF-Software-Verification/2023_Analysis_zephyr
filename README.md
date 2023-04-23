# 2023_Analysis_zephyr

This repository "Software Verification" course at the Faculty of Mathematics in Belgrade.

Techniques and tools used:
- Unit testing via [twister](https://docs.zephyrproject.org/3.1.0/develop/test/twister.html) tool
- Valgrind memcheck
- Static analysis
- Adress sanitizer

Repo organization:

```
├── unit_testing
│   ├── code_coverage
│   └── test_coverage_reports
└── zephyr
```

### Initializing zephyr submodule

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

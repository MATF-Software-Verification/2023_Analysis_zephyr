# 2023_Analysis_zephyr

### Initializing zephyr submodule

#### Install zephyr tools
`cd zephyr`
`pip3 install --user -r scripts/requirements.txt`

#### Set up the environment for SDK installation
TODO: create a script for setting these variables
`export ZEPHYR_TOOLCHAIN_VARIANT=zephyr`
`export ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk`
`. ./zephyr-env.sh`

#### Install the zephyr SDK
`wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.10.0/zephyr-sdk-0.10.0-setup.run
sudo sh zephyr-sdk-0.10.0-setup.run -- -d $ZEPHYR_SDK_INSTALL_DIR`

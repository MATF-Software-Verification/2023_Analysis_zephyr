# Valgrind

Given the complexity of the RtOS in question, a decision has been made to test already implemented samples withing the bluetooth subsystem. As already mentioned, some of the samples are targeting real embedded devices, and are not suitable for execution on POSIX platforms. That being said, there is a subset of samples that **can** be executed even in this way. Other way to run bt samples is to use Qemu for one of the supported embedded platform emulators.

If the host machine doesn't support BLE (Bluetooth spec 4.0 and above), a bluetooth dongle can be plugged in and configured.

#### Bluetooth dongle configuration [optional]

Zephyr executables compiled for a native platform require a HCI controller as an argument to be supplied. 
To determine which device you should pass as an argument, `hciconfig` tool can be used.

- First check the output of the `hciconfig` without the dongle attached, and memorize it's MAC address. The result should look something like this:

```
hci0:	Type: Primary  Bus: USB
	BD Address: DE:AD:BE:EF:FF:FF  ACL MTU: 1021:8  SCO MTU: 64:1
	UP RUNNING 
	RX bytes:614 acl:0 sco:0 events:40 errors:0
	TX bytes:3070 acl:0 sco:0 commands:40 errors:0
```

After plugging in the dongle, run the same command and memorize the new **hciN** device. This is the device you'll pass to the zephyr.exe during execution

#### Running the sample with dongle / host with BLE support

Running the samples is pretty straightforward once the configuration is done properly. For example, in order to run `broadcaster` sample, you'll need to follow these steps:

- build the sample for posix platform

`west build -p always -b native_sim <path_to_bt_samples>/broadcaster`

- You should have a `buld/zephyr/zephyr.exe` executable available.

- To run it, you can chain these commands:

`sudo hciconfig hci<N> down && sudo build/zephyr/zephyr.exe -bt-dev=hci<N> -rt`


#### BabbleSim configuration

If you don't have access to a bluetooth dongle, bt radio communication can be simulated using BabbleSim. To install BabbleSim, you can follow the instructions from [here](https://babblesim.github.io/fetching.html) and [here](https://babblesim.github.io/building.html).

*NOTE:* I recommend fetching using Android's repo synce i had trouble integrating using Zephyr's west.

NOTE: Add the following env variables in .bashrc (or you can export them manually every session):
```
export BSIM_OUT_PATH=${HOME}/bsim/
export BSIM_COMPONENTS_PATH=${BSIM_OUT_PATH}/components/
```

#### Running the sample utilizing BabbleSim

The process for running the sample with BabbleSim is similar to running it as a native app. In order to do so, you can transfer (and rename) the built `zephyr.exe` to the location where you installed BabbleSim (using the instructions above). Now you need to run a simulated physical link (for example `bs_2G4_phy_v1`), supply the custom test name, the number of devices PHY link should expect and chain executables, connecting them to the same name of test, while also providing the unique device numbers. It's easier to explain with an example:

```
cd <path_to_bablesim>/bin

/bs_2G4_phy_v1 -s=sample_test -D=1 & ./zephyr.exe -s=sample_test -d=0
```

Where `-s=sample_test` uniquely names the test, `-D=1` indicates that there will be a single device connected to the PHY, and `-d=0` uniquely identifies a device that'll connect to the physical layer.

### Using valgrind to analyze binaries

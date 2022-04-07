# Zephry Project notes <!-- omit in toc -->

- [1. Installation](#1-installation)
	- [1.1. Dependencies](#11-dependencies)
	- [1.2. Zephyr and Python dependencies](#12-zephyr-and-python-dependencies)
	- [1.3. Building and Flashing](#13-building-and-flashing)
	- [1.4. Supported Boards](#14-supported-boards)
- [2. Programming](#2-programming)
	- [2.1. GPIO](#21-gpio)
	- [2.2. Threads](#22-threads)
		- [2.2.1. Note on Thread Priorities](#221-note-on-thread-priorities)
- [3. MCUBoot](#3-mcuboot)
	- [3.1. Build and Flash Bootloader](#31-build-and-flash-bootloader)
	- [3.2. Build and Flash Application](#32-build-and-flash-application)
	- [3.3. Upgrading image](#33-upgrading-image)
		- [3.3.1. Flashing using pyocd](#331-flashing-using-pyocd)
		- [3.3.2. Flashing using mcumgr](#332-flashing-using-mcumgr)

## 1. Installation 

### 1.1. Dependencies
  
| Tool | Version |
|------|---------|
| Cmake | 3.20.0 |
| Python | 3.6 |
| Device tree compiler | 1.4.6 |

Download Kitware archive script:

    wget https://apt.kitware.com/kitware-archive.sh
    sudo bash kitware-archive.sh

 Download required dependencies: 

    sudo apt install --no-install-recommends git cmake ninja-build gperf \
      ccache dfu-util device-tree-compiler wget \
      python3-dev python3-pip python3-setuptools python3-tk python3-wheel xz-utils file \
      make gcc gcc-multilib g++-multilib libsdl2-dev

### 1.2. Zephyr and Python dependencies

Install west and adding `~/.local/bin` to `PATH`:

    pip3 install --user -U west
    echo 'export PATH=~/.local/bin:"$PATH"' >> ~/.bashrc
    source ~/.bashrc

Get Zephyr source code;

    west init ~/zephyrproject
    cd ~/zephyrproject
    west update

Export a Zephyr CMake package:

    west zephyr-export

Install additional Python dependencies:

    pip3 install --user -r ~/zephyrproject/zephyr/scripts/requirements.txt

### 1.3. Building and Flashing

Build application with west build, changing <your-board-name> to used board and <path/to/application/folder>

    cd ~/zephyrproject/zephyr
    west build -p auto -b <your-board-name> <path/to/application/folder>

To flash application use `west flash`:

    west flash

Sample applications are located in `/samples`

### 1.4. Supported Boards

IDs of supported boards, their device trees and documentations are located in `/boards`

[Reference](https://docs.zephyrproject.org/latest/getting_started/index.html)

## 2. Programming

This chapter includes simple example code for programming with Zephyr SDK. For more information about API go to [API References](https://docs.zephyrproject.org/latest/reference/index.html).

### 2.1. GPIO

Simple blinky example:
```cpp

/* get pointers to GPIO device, pin number of device and pin configuration flags for this device from device tree*/
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios); 

void main(void)
{
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE); // configure led pin as output and initalize it to logic 0
    while (1) 
    {
        gpio_pin_toggle_dt(&led); // toggle led
        k_msleep(1000); // put this thread to sleep
    }
}
```

[GPIO API Reference](https://docs.zephyrproject.org/latest/reference/peripherals/gpio.html)

### 2.2. Threads

Thread blinky example:
```cpp
/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 2

static struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios,{0});
static struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios,{0});

void blink0(void)
{
	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
	while(1) 
    {
		gpio_pin_toggle_dt(&led0);
		k_msleep(100);
	}
}

void blink1(void)
{
	gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
	while(1) 
    {
		gpio_pin_toggle_dt(&led1);
		k_msleep(1000);
	}
}
 
/* create tasks */ 
K_THREAD_DEFINE(blink0_id, STACKSIZE, blink0, NULL, NULL, NULL, PRIORITY, 0, 0);
K_THREAD_DEFINE(blink1_id, STACKSIZE, blink1, NULL, NULL, NULL, PRIORITY, 0, 0);
```
#### 2.2.1. Note on Thread Priorities

A thread priority can be a non-negative or negative integer value and is inversly proportional to task priority. If task A has priority of 3 and task B priority of 5,  scheduler gives higher priority to task A.

[Threads API Reference](https://docs.zephyrproject.org/latest/reference/kernel/threads/index.html#api-reference)


## 3. MCUBoot

### 3.1. Build and Flash Bootloader

Build MCUboot bootloader with `west build`:

	cd ~/zephyrproject
	west build -b YOUR_BOARD -s bootloader/mcuboot/boot/zephyr -d build-mcuboot

Flash bootloader:

	west flash -d build-mcuboot


Output on serial monitor should look like this:

    *** Booting Zephyr OS build zephyr-v3.0.0-2126-g65a2e4b76008  ***
    I: Starting bootloader
    I: Primary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
    I: Secondary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
    I: Boot source: none
    W: Failed reading image headers; Image=0
    E: Unable to find bootable image

### 3.2. Build and Flash Application

Building and flashing hello_world sample application:

	west build -b YOUR_BOARD -s zephyr/samples/hello_world -d build-hello-signed -- \
		 -DCONFIG_BOOTLOADER_MCUBOOT=y \
		 -DCONFIG_MCUBOOT_SIGNATURE_KEY_FILE=\"bootloader/mcuboot/root-rsa-2048.pem\"

	west flash -d build-hello-signed

After restarting the board, serial output should look like this:

    *** Booting Zephyr OS build zephyr-v3.0.0-2126-g65a2e4b76008  ***
    I: Starting bootloader
    I: Primary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
    I: Secondary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
    I: Boot source: none
    I: Swap type: none
    I: Bootloader chainload address offset: 0xc000
    I: Jumping to the first image slot
    *** Booting Zephyr OS build zephyr-v3.0.0-2126-g65a2e4b76008  ***
    Hello World from Zephyr on nrf52840dk_nrf52840!

[Reference](https://docs.zephyrproject.org/3.0.0/guides/west/sign.html)

### 3.3. Upgrading image

#### 3.3.1. Flashing using pyocd

To flash image to specific address we will use pyocd:
	
	python3 -mpip install -U pyocd

Build image:

	west build -b YOUR_BOARD -s zephyr/samples/hello_world -d build-hello-update-signed --      -DCONFIG_BOOTLOADER_MCUBOOT=y  -DCONFIG_MCUBOOT_SIGNATURE_KEY_FILE=\"bootloader/mcuboot/root-rsa-2048.pem\"



Sign image with imgtool.py. This example uses private key that is publicly available. For production generate your own keys.

	bootloader/mcuboot/scripts/imgtool.py sign --key bootloader/mcuboot/root-rsa-2048.pem --header-size 0x200 --align 8 --version 1.3 --slot-size 0x67000 --pad build-hello-update-signed/zephyr/zephyr.bin signed-updated-hello.bin

Image needs to be written to memory addres of slot 1 partition. Address can be found in ~/zephyrproject/zephyr/boards in .dts file of the board. Example for nrf52840dk_nrf52840:

	&flash0 {

		partitions {
			compatible = "fixed-partitions";
			#address-cells = <1>;
			#size-cells = <1>;

			boot_partition: partition@0 {
				label = "mcuboot";
				reg = <0x000000000 0x0000C000>;
			};
			slot0_partition: partition@c000 {
				label = "image-0";
				reg = <0x0000C000 0x00067000>;
			};
			slot1_partition: partition@73000 {
				label = "image-1";
				reg = <0x00073000 0x00067000>;
			};
			scratch_partition: partition@da000 {
				label = "image-scratch";
				reg = <0x000da000 0x0001e000>;
			};

			/*
			* The flash starting at 0x000f8000 and ending at
			* 0x000fffff is reserved for use by the application.
			*/

			/*
			* Storage partition will be used by FCB/LittleFS/NVS
			* if enabled.
			*/
			storage_partition: partition@f8000 {
				label = "storage";
				reg = <0x000f8000 0x00008000>;
			};
		};
	};


Flash image	with `pyocd flash` to address 0x73000. 

	pyocd flash -a 0x73000 -t nrf52840 signed-updated-hello.bin

Serial output should look like this:

	*** Booting Zephyr OS build zephyr-v3.0.0-2316-g2a9b2dd7b950  ***
	I: Starting bootloader
	I: Primary image: magic=good, swap_type=0x4, copy_done=0x1, image_ok=0x1
	I: Secondary image: magic=good, swap_type=0x1, copy_done=0x3, image_ok=0x3
	I: Boot source: none
	I: Swap type: test
	I: Starting swap using move algorithm.
	I: Bootloader chainload address offset: 0xc000
	I: Jumping to the first image slot
	*** Booting Zephyr OS build zephyr-v3.0.0-2316-g2a9b2dd7b950  ***
	Hello World! This is Updated image nrf52840dk_nrf52840

`Swap type: test` means that if upgrade image is not confirmed it will be reverted to primary image after reset (`Swap type: revert`):

	*** Booting Zephyr OS build zephyr-v3.0.0-2316-g2a9b2dd7b950  ***
	I: Starting bootloader
	I: Primary image: magic=good, swap_type=0x2, copy_done=0x1, image_ok=0x3
	I: Secondary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
	I: Boot source: none
	I: Swap type: revert
	I: Starting swap using move algorithm.
	I: Secondary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
	I: Bootloader chainload address offset: 0xc000
	I: Jumping to the first image slot
	*** Booting Zephyr OS build zephyr-v3.0.0-2316-g2a9b2dd7b950  ***
	Hello World! nrf52840dk_nrf52840

To upgrade permanently add --confirm to sign image command.

[Reference]( https://www.methodpark.de/blog/a-setup-for-firmware-updates-over-the-air-part-3-wireless-sensor-nodes-mcuboot/)

[Other reference](https://github.com/mcu-tools/mcuboot/blob/main/docs/readme-zephyr.md)

#### 3.3.2. Flashing using mcumgr

Couldn't get this to work because of Error: NMP timeout when using mcumgr.

Install `go`:

	sudo add-apt-repository ppa:longsleep/golang-backports
	sudo apt update
	sudo apt install golang-go

Install `mcumgr` with `go`:

	go install github.com/apache/mynewt-mcumgr-cli/mcumgr@latest

Add go to `PATH`:

	vim ~/.bashrc
	export PATH="/home/USER/mcumgr/mcumgr":$PATH
	source ~/.bashrc


For nRF52840 disable Mass Storage Device with JLinkExe

	mcumgr --conntype="serial" --connstring="dev=/dev/ttyACM0,baud=115200,mtu=512" image list

[Reference](https://docs.zephyrproject.org/latest/guides/device_mgmt/mcumgr.html?highlight=device%20tree)



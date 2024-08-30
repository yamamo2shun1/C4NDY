# Firmware Update
## Introduction
The C4NDY KeyVLM/STK does not require a firmware rewrite to change the keymap.
Normal users will only update the firmware when bug fixes or new features are added.
C4NDY KeyVLM/STK has a built-in boot loader for this purpose.

## How to switch to boot loader mode
Press and hold down the LGUI and ESC keys or RESET key until the green LED on the Side-Panel blinks three times to enter the boot loader mode.

<video controls="controls" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/79a93170-637f-4472-83ec-ebd615135d6c" />

## Installing the driver
If you are using Windows, you will need to install the driver on [Zadig](https://zadig.akeo.ie/) after entering the boot loader mode (first time only).

![zadig](/images/zadig.png)

## Installing the programmer
To update the firmware, it is recommended to install and use [STM32CubeProg](https://www.st.com/ja/development-tools/stm32cubeprog.html).
The procedure to update the firmware using STM32CubeProg is as follows

0. Download the latest ELF file from [here](https://github.com/yamamo2shun1/C4NDY/releases).
1. launch STM32CubeProg.
2. Rewrite "PID" to "0x0001"(KeyVLM) or "0x0002"(STK).
3. Rewrite "VID" to "0x31BF".
4. Click the "Port" update button.
5. Confirm that "Port" is changed from "No DFU detected" to "USB1" and click the "Connect" button.
6. Click the "Erasing & Programming" button, the second green button from the top on the left side of the window.
7. Check the "Verify programming" and "Run after programming" check boxes.
8. Confirm that "Full chip erase" is not checked.
9. Set the path of downloaded .elf file in "File path".
10. Click "Start Programming" to start programming.

<video controls="controls" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/fc8503f9-ba2c-4c98-849b-17fdc625cbd1" />


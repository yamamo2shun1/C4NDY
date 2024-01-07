# C4NDY KeyVLM

## Features
The C4NDY KeyVLM is a USB keyboard with a 65-key Ortholinear layout. Its greatest feature is that it can be connected to a turntable, allowing you to mix the music you are playing on your computer or smartphone with the music on your vinyl records.

- Supported Keyswitch: Gateron Low Profile 2.0(Implemented Hotswap socket KS-27)  
- Supported USB Audio Spec: 16bit 48kHz Input only(Codec is [Analog Devices ADAU1761](https://www.analog.com/jp/products/adau1761.html))
- Supported Analog Audio In/Out: Line/Phono In and Headphone Out, [Thonkiconn 3.5mm jack socket](https://www.thonk.co.uk/shop/thonkiconn/))

![1](https://github.com/yamamo2shun1/C4NDY/assets/96638/20440f42-f48b-4111-9ce8-993f187b1d1e)
![2](https://github.com/yamamo2shun1/C4NDY/assets/96638/e7a72ce8-ff90-4a15-8e16-572257de4128)
![3](https://github.com/yamamo2shun1/C4NDY/assets/96638/33fd7535-f1e1-4e20-a99c-ce84f68deabb)

## Side Panel I/O
<img width="550" alt="C4NDY_KeyVLM_PanelIO_explain" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/ab5ae116-6d78-4700-a677-635909ac28c7">

## Example Layout
![keymap-layout](https://github.com/yamamo2shun1/C4NDY/assets/96638/bc8e57b4-f748-4dd8-b59a-7187f392ab16)

## Keymap Configurator
The keymap can be changed using the [KeyVLM Configurator](https://github.com/yamamo2shun1/KeyVLM_Configurator), a dedicated configuration tool developed in the Go language.

## Mainboard(rev.D)
The C4NDY is the STM32G491 prototyping board.

- MCU: [STM32G491CET6](https://www.st.com/ja/microcontrollers-microprocessors/stm32g4x1.html)
- Bootloader: Custom bootloader([C4NDY_BTL](https://github.com/yamamo2shun1/C4NDY/tree/main/STM32CubeIDE/C4NDY_BTL))
- USB Stack: [TinyUSB](https://github.com/hathach/tinyusb)

![c4ndy_brd_explain](https://github.com/yamamo2shun1/C4NDY/assets/96638/89944889-bca0-41cf-98d1-02606702823f)

<img width="250" alt="c4ndy_revD_front" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/d9490441-fa41-4fb6-88b6-fedd4f7f2d4a"> <img width="250" alt="c4ndy_revD_back" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/f3e677f7-a3df-4a09-87be-e421eff5cd60"> <img width="500" alt="c4ndy_revD_overview" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/eb478440-ad95-4560-ae95-b7dd2733617f">


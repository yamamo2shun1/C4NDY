# Specifications

## Common

- MCU: [STM32G491](https://www.st.com/en/microcontrollers-microprocessors/stm32g4x1.html)
  - USB Stack: [TinyUSB](https://docs.tinyusb.org/en/latest/)
  - Debug Interface: SWD
- Codec: [Analog Devices ADAU1761](https://www.analog.com/jp/products/adau1761.html)
  - USB Audio Spec: 24bit 48kHz Input only
  - Analog Audio In/Out: Line/Phono In and HP Out, [Thonkiconn 3.5mm jack socket](https://www.thonk.co.uk/shop/thonkiconn/))
- Keyswitch: Gateron Low Profile 2.0(Implemented Hotswap socket KS-27)  

## KeyVLM 

- 65x Keys, 2x Pots(mix and main output gain), 9x GPIO Ports
- [Schematics](/pdf/KeyVLM-sch.pdf)
- [PCB](/pdf/KeyVLM-brd.pdf)

## STK

- 37x Keys, 34x RGB LEDs, 2x Analog JoySticks, 1x Pot(mix), 6x GPIO Ports
- [Schematics](/pdf/STK-sch.pdf)
- [PCB](/pdf/STK-brd.pdf)


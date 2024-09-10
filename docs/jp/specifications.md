# 主な仕様

## 共通

- メインボード:
  - MCU: [STM32G491](https://www.st.com/en/microcontrollers-microprocessors/stm32g4x1.html)
  - USBスタック: [TinyUSB](https://docs.tinyusb.org/en/latest/)
  - デバッグ・インターフェース: SWD
  - [Schematics](/pdf/main-sch.pdf)
  - [PCB](/pdf/main-brd.pdf)
- オーディオ・コーデック: [Analog Devices ADAU1761](https://www.analog.com/jp/products/adau1761.html)
  - USBオーディオ: 24bit 48kHz （入力のみ、フォノ/ラインの音声をホスト側に送ることは出来ません）
  - アナログ・オーディオ入出力: フォノ/ライン入力、 HP出力, [Thonkiconn 3.5mm jack socket](https://www.thonk.co.uk/shop/thonkiconn/))
- キースイッチ: Gateron Low Profile 2.0(Implemented Hotswap socket KS-27)  

## KeyVLM 

- 65個のキー、2個のボリューム(ミックスと出力ゲイン調整用), 9個のGPIOポート
- [Schematics](/pdf/KeyVLM-sch.pdf)
- [PCB](/pdf/KeyVLM-brd.pdf)

## STK

- 37個のキー、34個のフルカラーLED、2本のジョイスティック、1個のボリューム(ミックス調整用), 6個のGPIOポート
- [Schematics](/pdf/STK-sch.pdf)
- [PCB](/pdf/STK-brd.pdf)


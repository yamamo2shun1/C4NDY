# C4NDY Composite HID & UAC

サブモジュール化したTinyUSBそのままではPC接続時にUSBデバイスとして正しく認識しないので、以下の変更を行う。

## dcd_stm32_fsdev.c (src/portable/st/stm32_fsdev)
dcd_init_enable関数内にある割り込みを有効化する処理を2箇所、コメントアウトする。

```
#elif CFG_TUSB_MCU == OPT_MCU_STM32G4
  //NVIC_EnableIRQ(USB_HP_IRQn); <-- コメントアウト
  NVIC_EnableIRQ(USB_LP_IRQn);
  //NVIC_EnableIRQ(USBWakeUp_IRQn); <-- コメントアウト
```

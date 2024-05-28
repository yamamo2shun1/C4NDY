# C4NDY KeyVLM

## 特徴
C4NDY KeyVLMは65キー格子配列のUSBキーボードです。最大の特徴はターンテーブル（Phono出力のレコードプレーヤー）を接続出来ることで、レコードとスマホやPC/Macの音楽をミックスすることが出来ます。

- 対応キースイッチ: Gateron Low Profile 2.0(Hotswap socket KS-27実装済み)  
- USBオーディオ・スペック: 24bit 48kHz 入力のみ(Codec is [Analog Devices ADAU1761](https://www.analog.com/jp/products/adau1761.html))
- 対応オーディオ入出力: Line/Phono 入力 ＆ ヘッドホン出力, [Thonkiconn 3.5mm jack socket](https://www.thonk.co.uk/shop/thonkiconn/))

![1](https://github.com/yamamo2shun1/C4NDY/assets/96638/124d06b2-1729-456f-8eb0-5030e0e5dcab)
![2](https://github.com/yamamo2shun1/C4NDY/assets/96638/e7a72ce8-ff90-4a15-8e16-572257de4128)
![3](https://github.com/yamamo2shun1/C4NDY/assets/96638/33fd7535-f1e1-4e20-a99c-ce84f68deabb)

## サイドパネル
<img width="550" alt="C4NDY_KeyVLM_PanelIO_explain" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/d100fdbf-8fa1-45a7-951f-1ab9171c9ce2">

## キーレイアウト例
![keymap-layout](https://github.com/yamamo2shun1/C4NDY/assets/96638/bc8e57b4-f748-4dd8-b59a-7187f392ab16)

## キーマップコンフィグレーター
The keymap can be changed using the [KeyVLM Configurator](https://github.com/yamamo2shun1/KeyVLM_Configurator), a dedicated configuration tool developed in the Go language.

## ファームウェア・アップデート
### 概要
C4NDY KeyVLMはキーマップの変更でファームウェアを書き換える必要がありません。
バグフィックスや新機能が追加されるときのみファームウェアの更新が必要になり、そのためのブートローダーを実装しています。

### ブートローダーモードへの切替方法
LGUI（Win/Command）キーとESCキーを押し続け、サイドパネルの緑LEDが3回点滅すると、ブートローダーモードに切り替わります。

https://github.com/yamamo2shun1/C4NDY/assets/96638/79a93170-637f-4472-83ec-ebd615135d6c

### ドライバーのインストール
Windowsユーザーの場合、ブートローダーモードに初めて切り替えた際に [Zadig](https://zadig.akeo.ie/) でドライバーをインストールする必要があります。

<img width="431" alt="スクリーンショット 2024-01-19 095923" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/9a9aec08-cc1c-4b73-bf09-fa08c4a1bd21">

### STM32CubeProgのインストール
ファームウェアを更新するためには、STMicroelectronics社がリリースしている [STM32CubeProg](https://www.st.com/ja/development-tools/stm32cubeprog.html) インストールして使うことを推奨しています。
STM32CubeProgを使ってファームウェアを更新する手順は下記の通りです。

1. STM32CubeProgを起動します
2. 「PID」に「0x0001」と入力します
3. 「VID」に「0x31BF」と入力します
4. 「Port」の更新ボタンをクリックします
5. 「Port」が「No DFU detected」から「USB1」になったことを確認し、「Connect」ボタンをクリックします
6. ウィンドウ左端に並んでいる緑色アイコンの上から2つ目にある「Erasing & Programming」ボタンをクリックします
7. 「Verify programming」と「Run after programming」のチェックボックスにチェックを入れます
8. 「Full chip erase」にチェックが入っていないことを確認します
9．「File path」に更新する.hexファイルをセットします
10. 「Start Programming」をクリックし、9でセットした.hexを書き込みます

https://github.com/yamamo2shun1/C4NDY/assets/96638/fc8503f9-ba2c-4c98-849b-17fdc625cbd1

## メインボード(rev.D)
C4NDYはSTM32G491を搭載したプロトタイピングボードです。

- MCU: [STM32G491CET6](https://www.st.com/ja/microcontrollers-microprocessors/stm32g4x1.html)
- Bootloader: Custom bootloader([C4NDY_BTL](https://github.com/yamamo2shun1/C4NDY/tree/main/STM32CubeIDE/C4NDY_BTL))
- USB Stack: [TinyUSB](https://github.com/hathach/tinyusb)

![c4ndy_brd_explain](https://github.com/yamamo2shun1/C4NDY/assets/96638/89944889-bca0-41cf-98d1-02606702823f)

<img width="250" alt="c4ndy_revD_front" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/d9490441-fa41-4fb6-88b6-fedd4f7f2d4a"> <img width="250" alt="c4ndy_revD_back" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/f3e677f7-a3df-4a09-87be-e421eff5cd60"> <img width="500" alt="c4ndy_revD_overview" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/eb478440-ad95-4560-ae95-b7dd2733617f">


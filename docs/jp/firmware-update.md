# ファームウェアの更新
## はじめに
C4NDY KeyVLM/STKはキーマップの変更でファームウェアを書き換える必要がありません。通常のユーザーはバグ修正や新機能が追加されるときに必要に応じてファームエアを更新します。

## ブートローダーモードへの移行
C4NDY KeyVLM/STKはファームウェア書き換え用のライターを使わずにファームウェアの更新を置くなうためにブートローダーが内蔵さえています。
ブートローダーモードに移行するには、サイドパネルの緑LEDが3回点滅するまでLGUIとESCキーの2つ（もしくはRESETキー単独）を長押しします。

<video controls="controls" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/79a93170-637f-4472-83ec-ebd615135d6c" />

## ドライバのインストール
Windowsユーザーの場合、ブートローダーモードに初めて切り替えた時のみ、[Zadig](https://zadig.akeo.ie/)でドライバーをインストールする必要があります。

![zadig](/images/zadig.png)

## プログラマのインストール
ファームウェアの更新をするためには、STMicroelectronics社がリリースしている[STM32CubeProg](https://www.st.com/ja/development-tools/stm32cubeprog.html)をインストールしてお使いいただくことを推奨しています。
STM32CubeProgを使ってファームウェアを更新する手順は下記の通りです。

0. 最新のELFファイルを[ここ](https://github.com/yamamo2shun1/C4NDY/releases)からダウンロードします
1. STM32CubeProgを起動します
2. 「PID」に「0x0001」と入力します
3. 「VID」に「0x31BF」と入力します
4. 「Port」の更新ボタンをクリックします
5. 「Port」が「No DFU detected」から「USB1」になったことを確認し、「Connect」ボタンをクリックします
6. ウィンドウ左端に並んでいる緑色アイコンの上から2つ目にある「Erasing & Programming」ボタンをクリックします
7. 「Verify programming」と「Run after programming」のチェックボックスにチェックを入れます
8. 「Full chip erase」にチェックが入っていないことを確認します
9．「File path」にダウンロードした.elfファイルをセットします
10. 「Start Programming」をクリックし、9でセットした.elfを書き込みます

<video controls="controls" src="https://github.com/yamamo2shun1/C4NDY/assets/96638/fc8503f9-ba2c-4c98-849b-17fdc625cbd1" />


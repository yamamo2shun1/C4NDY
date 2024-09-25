# キーボードのリマップ

C4NDY KeyVLM/STKのキーをリマップするには、専用のコマンドライン・ツール[KeyConfigurator](https://github.com/yamamo2shun1/KeyConfigurator)を使います。

## 準備
キーマップを記述したTOMLを用意することで、ファームウェアの書き換え無しにリマップすることが可能です。

- [例：KeyVLM](https://github.com/yamamo2shun1/KeyConfigurator/blob/main/layouts_KeyVLM.toml)
```toml
[layout1]
	normal = [
		["Esc",      "1",    "2",      "3",     "4",  "5",     "6",     "7",      "8",         "9",      "0",     "Minus",      "Equal"],
		["Tab",      "Q",    "W",      "E",     "R",  "T",     "Y",     "U",      "I",         "O",      "P",     "O_SBracket", "C_SBracket"],
		["CapsLock", "A",    "S",      "D",     "F",  "G",     "H",     "J",      "K",         "L",      "Colon", "Apostrophe", "Yen"],
		["L_Shift",  "Z",    "X",      "C",     "V",  "B",     "N",     "M",      "Comma",     "Period", "Slash", "R_Shift",    "Backquote"],
		["L_Gui",    "LNPH", "LAYOUT", "L_Alt", "BS", "Enter", "Space", "Henkan", "R_Control", "Left",   "Down",  "Up",         "Right"]
	]

[layout2]
	normal = [
		["Esc",       "1",          "2",      "3",     "4",  "5",      "6",     "7",     "8",        "9",    "0",    "O_SBracket", "C_SBracket"],
		["Tab",       "Apostrophe", "Comma",  "O",     "U",  "Y",      "F",     "G",     "C",        "R",    "L",    "Slash",      "Equal"],
		["L_Control", "P",          "I",      "E",     "A",  "Period", "D",     "S",     "T",        "H",    "Z",    "Minus",      "Backslash"],
		["L_Shift",   "J",          "Q",      "Colon", "K",  "X",      "B",     "M",     "W",        "N",    "V",    "R_Shift",    "Backquote"],
		["L_Gui",     "LNPH",       "LAYOUT", "L_Alt", "BS", "Del",    "Enter", "Space", "CapsLock", "Left", "Down", "Up",         "Right"]
	]
```

- [例：STK](https://github.com/yamamo2shun1/KeyConfigurator/blob/main/layouts_STK.toml)
```toml
[layout1]
	normal = [
		["Q",    "W",     "E",     "R",    "T",       "Y",    "U",    "I",     "O",      "P"],
		["A",    "S",     "D",     "F",    "G",       "H",    "J",    "K",     "L",      "Colon"],
		["Z",    "X",     "C",     "V",    "B",       "N",    "M",    "Comma", "Period", "Slash"],
		["Null", "L_Gui", "R_Alt", "Null", "L_Shift", "Null", "Left", "Down",  "Up",     "Right"]
	]
	upper = [
		["1",    "2",         "3",      "4",     "5",       "6",    "7",          "8",          "9",          "0"],
		["Q",    "W",         "Null",   "L_Gui", "G",       "Null", "Minus",      "Equal",      "O_SBracket", "Apostrophe"],
		["Null", "R_Control", "Null",   "Null",  "B",       "Null", "C_SBracket", "Null",       "Backquote",  "Yen"],
		["Null", "LNPH",      "LAYOUT", "Null",  "L_Shift", "Null", "Null",       "MGain_Down", "MGain_Up",   "Reset"]
	]
	stick = [
		["BS",  "Tab",    "Esc",   "Upper"],
		["Del", "Henkan", "Space", "CapsLock"]
	]

[layout2]
	normal = [
		["Apostrophe", "Comma", "O",     "U",    "Y",       "F",    "G",    "C",    "R",  "L"],
		["P",          "I",     "E",     "A",    "Period",  "D",    "S",    "T",    "H",  "Z"],
		["J",          "Q",     "Colon", "K",    "X",       "B",    "M",    "W",    "N",  "V"],
		["Null",       "L_Gui", "L_Alt", "Null", "L_Shift", "Null", "Left", "Down", "Up", "Right"]
	]
	upper = [
		["1",          "2",        "3",      "4",     "5",       "6",    "7",          "8",          "9",         "0"],
		["Apostrophe", "Comma",    "Null",   "L_Gui", "Period",  "Null", "O_SBracket", "C_SBracket", "Slash",     "Minus"],
		["Null",       "CapsLock", "Null",   "Null",  "X",       "Null", "Equal",      "Null",       "Backquote", "Backslash"],
		["Null",       "LNPH",     "LAYOUT", "Null",  "L_Shift", "Null", "Null",       "MGain_Down", "MGain_Up",  "Reset"]
	]
	stick = [
		["BS", "Tab",    "Esc",   "Upper"],
		["Del", "Space", "Enter", "R_Control"]
	]
```

## 使い方
[こちら](https://github.com/yamamo2shun1/KeyConfigurator/releases)から最新版をダウンロードし、コマンドプロンプトやPowershell(Windowsの場合)、Terminal.app (macOSの場合)から利用します。
```shellscriput
-version
        Show the version of the tool installed.
        ex) keyconfig -version
-check
        Show information on C4NDY KeyVLM/STK connected to PC/Mac.
        ex) keyconfig -check
-list
        Show connected device name list.
        ex) keyconfig -list
-id [int]
        Select connected device ID(ID can be checked in -check/-list).
        This option is available when using the following command options.
        If ID is not specified, 0 is the default.
-load
        Show the current key names of the keyboard.
        ex) keyconfig -load
            keyconfig -load -id 1
-remap
        Apply the keymap infomation from layouts.toml by default.
        ex) keyconfig -remap
            keyconfig -remap -id 0
-file [string]
        Specify .toml file to be read.
        This option is available when using the '-remap' option.
        ex) keyconfig -remap -file layout_STK.toml
            keyconfig -id 0 -remap -file layout_KeyVLM.toml
-save
        Save the keymap written by "remap" to the memory area
        ex) keyconfig -save
            keyconfig -id 0 -save
```

## ツールのビルド
[Go言語](https://go.dev/)の開発環境を各自でご用意ください。
また、KeyConfiguratorではHIDでの通信を行うために[go-hid](https://github.com/sstallion/go-hid)を利用しており、OSごとに下記のような設定が必要になります。

### Windowsの場合
環境変数に`CGO_ENABLED=1`を追加し、[TDM-GCC](https://jmeubank.github.io/tdm-gcc/) などのCコンパイラをインストールしてください。

### macOS/Linuxの場合
ご利用のシェルの設定ファイル（zshの場合は.zshrcなど）に`CGO_ENABLED=1`を加えてください。

## ソースコードからビルドする場合
以下のようにして、ご利用のOS用の実行ファイルを生成してください。
```
> go build -o keyconfig
```

# How to remap

[Confiseur](https://github.com/yamamo2shun1/Confiseur) is a command line tool for key remapping dedicated to C4NDY KeyVLM/STK.

## Preparing
By preparing a keymap file written in TOML and reading it, it is possible to remap without rewriting the firmware.

- [Example for KeyVLM](https://github.com/yamamo2shun1/Confiseur/blob/main/example-layout/layouts_KeyVLM.toml)
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

- [Example for STK](https://github.com/yamamo2shun1/Confiseur/blob/main/example-layout/layouts_STK.toml)
```toml
[layout1]
	normal = [
		["Q",    "W",     "E",     "R",    "T",       "Y",    "U",    "I",     "O",      "P"],
		["A",    "S",     "D",     "F",    "G",       "H",    "J",    "K",     "L",      "Colon"],
		["Z",    "X",     "C",     "V",    "B",       "N",    "M",    "Comma", "Period", "Slash"],
		["Null", "L_Gui", "R_Alt", "Null", "CapsLock", "Null", "Left", "Down",  "Up",     "Right"]
	]
	upper = [
		["1",    "2",         "3",      "4",      "5",       "6",    "7",          "8",          "9",          "0"],
		["Q",    "W",         "Null",   "L_Gui",  "G",       "Null", "Minus",      "Equal",      "O_SBracket", "Apostrophe"],
		["Null", "R_Control", "M_LBTN", "M_RBTN", "M_WHEEL", "Null", "C_SBracket", "Null",       "Backquote",  "Yen"],
		["Null", "LNPH",      "LAYOUT", "Null",   "CapsLock",   "Null", "Null",       "MGain_Down", "MGain_Up",   "Reset"]
	]
	stick = [
		["BS",  "Tab",    "Space",   "Upper"],
		["Del", "Henkan", "Esc", "R_Shift"]
	]
	led = [
		[0xEC, 0x80, 0x8C],
		[0xEC, 0x00, 0x8C],
		[0x2C, 0x00, 0x8C]
	]

[layout2]
	normal = [
		["Apostrophe", "Comma", "O",     "U",    "Y",       "F",    "G",    "C",    "R",  "L"],
		["P",          "I",     "E",     "A",    "Period",  "D",    "S",    "T",    "H",  "Z"],
		["J",          "Q",     "Colon", "K",    "X",       "B",    "M",    "W",    "N",  "V"],
		["Null",       "L_Gui", "L_Alt", "Null", "Upper", "Null", "Left", "Down", "Up", "Right"]
	]
	upper = [
		["1",          "2",        "3",      "4",      "5",       "6",    "7",          "8",          "9",         "0"],
		["Apostrophe", "Comma",    "Null",   "L_Gui",  "Period",  "Null", "O_SBracket", "C_SBracket", "Slash",     "Minus"],
		["Null",       "CapsLock", "M_LBTN", "M_RBTN", "M_WHEEL", "Null", "Equal",      "Null",       "Backquote", "Backslash"],
		["Null",       "LNPH",     "LAYOUT", "Null",   "Upper",   "Null", "Null",       "MGain_Down", "MGain_Up",  "Reset"]
	]
	stick = [
		["BS", "Tab",    "Esc",   "L_Shift"],
		["Del", "Space", "Enter", "R_Control"]
	]
	led = [
		[0xFF, 0x00, 0x8C],
		[0x00, 0xFF, 0xFF],
		[0x00, 0x00, 0xFF]
	]
```

## Installation
The `go install` is available.
```shellscript
$ go install github.com/yamamo2shun1/Confiseur/cmd/confiseur@latest
```

## Usage

```Less
-version
        Show the version of the tool installed.
        ex) confiseur -version
-check
        Show information on C4NDY KeyVLM/STK connected to PC/Mac.
        ex) confiseur -check
-list
        Show connected device name list.
        ex) confiseur -list
-id [int]
        Select connected device ID(ID can be checked in -check/-list).
        This option is available when using the following command options.
        If ID is not specified, 0 is the default.
-load
        Show the current key names of the keyboard.
        ex) confiseur -load
            confiseur -load -id 1
-remap [string]
        Apply the keyboard settings from specified toml file.
        ex) confiseur -remap examples/layout_STK.toml
            confiseur -remap examples/layout_KeyVLM.toml -id 0
-save
        Save the keymap written by "remap" to the memory area.
        ex) confiseur -save
            confiseur -id 0 -save
-led [int(0x000000-0xFFFFFF)]
        Set LED RGB value for checking color.
        ex) confiseur -led 0xFF0000 # red
            confiseur id 0 -led 0x00FFFF # cyan
-restart
        Restart the keyboard immediately.
        ex) confiseur -restart
            confiseur -restart -id 1
```

## Preparation to build
First, install the [Go language](https://go.dev/) development environment.  
Some preparation is required to build the code because we are using [go-hid](https://github.com/sstallion/go-hid).

### for Windows
Add `CGO_ENABLED=1` to the environment variable and install a C compiler such as [TDM-GCC](https://jmeubank.github.io/tdm-gcc/) beforehand.

### for macOS/Linux
Add `CGO_ENABLED=1` to your shell configuration file, such as .zshrc.

## How to build from Source Code
```shellscript
$ go build -o confiseur
```

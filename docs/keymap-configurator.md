# How to remap

[KeyConfigurator](https://github.com/yamamo2shun1/KeyConfigurator) is a command line tool for key remapping dedicated to C4NDY KeyVLM/STK.

## Preparing
By preparing a keymap file written in TOML and reading it, it is possible to remap without rewriting the firmware.

- [Example for KeyVLM](https://github.com/yamamo2shun1/KeyConfigurator/blob/main/layouts_KeyVLM.toml)
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

- [Example for STK](https://github.com/yamamo2shun1/KeyConfigurator/blob/main/layouts_STK.toml)
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

## How to use
Download the latest version from [here](https://github.com/yamamo2shun1/KeyConfigurator/releases) and run it from Command Prompt (Windows) or Terminal.app (macOS).
```shellscriput
> keyconfig -version                 // Show the version of the tool installed
> keyconfig -check                   // Show information on C4NDY KeyVLM/STK connected to PC/Mac
> keyconfig -load                    // Show the current key names of the keyboard
> keyconfig -remap                   // Write the keyboard with the keymap set in layouts.toml
> keyconfig -remap -file custom.toml // Write the keymap set in the specified .toml to the keydoad
> keyconfig -save                    // Save the keymap written by "remap" to the memory area
```

## Preparation to build
First, install the [Go language](https://go.dev/) development environment.  
Some preparation is required to build the code because we are using [go-hid](https://github.com/sstallion/go-hid).

### for Windows
Add `CGO_ENABLED=1` to the environment variable and install a C compiler such as [TDM-GCC](https://jmeubank.github.io/tdm-gcc/) beforehand.

### for macOS/Linux
Add `CGO_ENABLED=1` to your shell configuration file, such as .zshrc.

## How to build from Source Code
```
> go build -o keyconfig
```

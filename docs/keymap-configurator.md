# How to remap

[Confiseur](https://github.com/yamamo2shun1/Confiseur) is a command line tool for key remapping dedicated to C4NDY KeyVLM/STK.

## Preparing
By preparing a keymap file written in TOML and reading it, it is possible to remap without rewriting the firmware.

- [Example for KeyVLM](https://github.com/yamamo2shun1/Confiseur/blob/main/example-layout/layouts_KeyVLM.toml)
```toml
[layout1]
	normal = [
		[["Esc", "00000000"],  ["1", "00000000"],    ["2", "00000000"],      ["3", "00000000"],    ["4", "00000000"],  ["5", "00000000"],   ["6", "00000000"],     ["7", "00000000"],     ["8", "00000000"],        ["9", "00000000"],      ["0", "00000000"],     ["Minus", "00000000"],      ["Equal", "00000000"]],
		[["Tab", "00000000"],  ["Q", "00000000"],    ["W", "00000000"],      ["E", "00000000"],    ["R", "00000000"],  ["T", "00000000"],   ["Y", "00000000"],     ["U", "00000000"],     ["V", "00000000"],        ["O", "00000000"],      ["P", "00000000"],     ["O_SBracket", "00000000"], ["C_SBracket", "00000000"]],
		[["Null", "00000001"], ["A", "00000000"],    ["S", "00000000"],      ["D", "00000000"],    ["F", "00000000"],  ["G", "00000000"],   ["H", "00000000"],     ["J", "00000000"],     ["K", "00000000"],        ["L", "00000000"],      ["Colon", "00000000"], ["Apostrophe", "00000000"], ["Yen", "00000000"]],
		[["Null", "00000010"], ["Z", "00000000"],    ["X", "00000000"],      ["C", "00000000"],    ["I", "00000000"],  ["B", "00000000"],   ["N", "00000000"],     ["M", "00000000"],     ["Comma", "00000000"],    ["Period", "00000000"], ["Slash", "00000000"], ["Null", "00100000"],       ["Backquote", "00000000"]],
		[["Null", "00001000"], ["LNPH", "00000000"], ["LAYOUT", "00000000"], ["Null", "00000100"], ["BS", "00000000"], ["Del", "00000000"], ["Enter", "00000000"], ["Space", "00000000"], ["CapsLock", "00000000"], ["Left", "00000000"],   ["Down", "00000000"],  ["Up", "00000000"],         ["Right", "00000000"]]

	]

[layout2]
	normal = [
		[["Esc", "00000000"],  ["1", "00000000"],          ["2", "00000000"],      ["3", "00000000"],     ["4", "00000000"],  ["5", "00000000"],      ["6", "00000000"],     ["7", "00000000"],     ["8", "00000000"],        ["9", "00000000"],    ["0", "00000000"],    ["O_SBracket", "00000000"], ["C_SBracket", "00000000"]],
		[["Tab", "00000000"],  ["Apostrophe", "00000000"], ["Comma", "00000000"],  ["O", "00000000"],     ["U", "00000000"],  ["Y", "00000000"],      ["F", "00000000"],     ["G", "00000000"],     ["C", "00000000"],        ["R", "00000000"],    ["L", "00000000"],    ["Slash", "00000000"],      ["Equal", "00000000"]],
		[["Null", "00000001"], ["P", "00000000"],          ["I", "00000000"],      ["E", "00000000"],     ["A", "00000000"],  ["Period", "00000000"], ["D", "00000000"],     ["S", "00000000"],     ["T", "00000000"],        ["H", "00000000"],    ["Z", "00000000"],    ["Minus", "00000000"],      ["Backslash", "00000000"]],
		[["Null", "00000010"], ["J", "00000000"],          ["Q", "00000000"],      ["Colon", "00000000"], ["K", "00000000"],  ["X", "00000000"],      ["B", "00000000"],     ["M", "00000000"],     ["W", "00000000"],        ["N", "00000000"],    ["V", "00000000"],    ["Null", "00100000"],       ["Backquote", "00000000"]],
		[["Null", "00001000"], ["LNPH", "00000000"],       ["LAYOUT", "00000000"], ["Null", "00000100"],  ["BS", "00000000"], ["Del", "00000000"],    ["Enter", "00000000"], ["Space", "00000000"], ["CapsLock", "00000000"], ["Left", "00000000"], ["Down", "00000000"], ["Up", "00000000"],         ["Right", "00000000"]]
	]
```

- [Example for STK](https://github.com/yamamo2shun1/Confiseur/blob/main/example-layout/layouts_STK.toml)
```toml
[layout1]
	normal = [
		[["Q", "00000000"],    ["W", "00000000"],    ["E", "00000000"],    ["R", "00000000"],    ["T", "00000000"],    ["Y", "00000000"],    ["U", "00000000"],    ["I", "00000000"],     ["O", "00000000"],      ["P", "00000000"]],
		[["A", "00000000"],    ["S", "00000000"],    ["D", "00000000"],    ["F", "00000000"],    ["G", "00000000"],    ["H", "00000000"],    ["J", "00000000"],    ["K", "00000000"],     ["L", "00000000"],      ["Colon", "00000000"]],
		[["Z", "00000000"],    ["X", "00000000"],    ["C", "00000000"],    ["V", "00000000"],    ["B", "00000000"],    ["N", "00000000"],    ["M", "00000000"],    ["Comma", "00000000"], ["Period", "00000000"], ["Slash", "00000000"]],
		[["Null", "00000000"], ["Null", "00001000"], ["Null", "00000100"], ["Null", "00000000"], ["Null", "00000001"], ["Null", "00000000"], ["Left", "00000000"], ["Down", "00000000"],  ["Up", "00000000"],     ["Right", "00000000"]]
	]
	upper = [
		[["1", "00000000"],          ["2", "00000000"],         ["3", "00000000"],      ["4", "00000000"],      ["5", "00000000"],       ["6", "00000000"],    ["7", "00000000"],          ["8", "00000000"],          ["9", "00000000"],        ["0", "00000000"]],
		[["Apostrophe", "00000000"], ["Backquote", "00000000"], ["Null", "00000000"],   ["Null", "00001000"],   ["Null", "00000000"],    ["Null", "00000000"], ["O_SBracket", "00000000"], ["C_SBracket", "00000000"], ["Minus", "00000000"],    ["Equal", "00000000"]],
		[["Null", "00000000"],       ["CapsLock", "00000000"],  ["M_LBTN", "00000000"], ["M_RBTN", "00000000"], ["M_WHEEL", "00000000"], ["Null", "00000000"], ["Null", "00000000"],       ["Comma", "00000000"],      ["Period", "00000000"],   ["Backslash", "00000000"]],
		[["Null", "00000000"],       ["LNPH", "00000000"],      ["LAYOUT", "00000000"], ["Null", "00000000"],   ["Null", "00000001"],    ["Null", "00000000"], ["Null", "00000000"],       ["MGain_Down", "00000000"], ["MGain_Up", "00000000"], ["Reset", "00000000"]]
	]
	stick = [
		[["BS", "00000000"],  ["Tab", "00000000"],   ["Enter", "00000000"], ["Upper", "00000000"]],
		[["Del", "00000000"], ["Space", "00000000"], ["Esc", "00000000"],   ["Null", "00100000"]]
	]
	led = [
		[0x00, 0xAE, 0xEF],
		[0xEF, 0x00, 0xAE],
		[0xAE, 0xEF, 0x00]
	]
    intensity = [
        1.0
    ]

[layout2]
	normal = [
		[["Apostrophe", "00000000"], ["Comma", "00000000"], ["O", "00000000"],     ["U", "00000000"],    ["Y", "00000000"],      ["F", "00000000"],    ["G", "00000000"],    ["C", "00000000"],    ["R", "00000000"],  ["L", "00000000"]],
		[["P", "00000000"],          ["I", "00000000"],     ["E", "00000000"],     ["A", "00000000"],    ["Period", "00000000"], ["D", "00000000"],    ["S", "00000000"],    ["T", "00000000"],    ["H", "00000000"],  ["Z", "00000000"]],
		[["J", "00000000"],          ["Q", "00000000"],     ["Colon", "00000000"], ["K", "00000000"],    ["X", "00000000"],      ["B", "00000000"],    ["M", "00000000"],    ["W", "00000000"],    ["N", "00000000"],  ["V", "00000000"]],
		[["Null", "00000000"],       ["Null", "00001000"],  ["Null", "00000100"],  ["Null", "00000000"], ["Null", "00000001"],   ["Null", "00000000"], ["Left", "00000000"], ["Down", "00000000"], ["Up", "00000000"], ["Right", "00000000"]]
	]
	upper = [
		[["1", "00000000"],          ["2", "00000000"],        ["3", "00000000"],      ["4", "00000000"],      ["5", "00000000"],       ["6", "00000000"],    ["7", "00000000"],          ["8", "00000000"],          ["9", "00000000"],         ["0", "00000000"]],
		[["Apostrophe", "00000000"], ["Comma", "00000000"],    ["Null", "00000000"],   ["Null", "00001000"],   ["Period", "00000000"],  ["Null", "00000000"], ["O_SBracket", "00000000"], ["C_SBracket", "00000000"], ["Slash", "00000000"],     ["Minus", "00000000"]],
		[["Null", "00000000"],       ["CapsLock", "00000000"], ["M_LBTN", "00000000"], ["M_RBTN", "00000000"], ["M_WHEEL", "00000000"], ["Null", "00000000"], ["Equal", "00000000"],      ["Null", "00000000"],       ["Backquote", "00000000"], ["Backslash", "00000000"]],
		[["Null", "00000000"],       ["LNPH", "00000000"],     ["LAYOUT", "00000000"], ["Null", "00000000"],   ["Null", "00000001"],    ["Null", "00000000"], ["Null", "00000000"],       ["MGain_Down", "00000000"], ["MGain_Up", "00000000"],  ["Reset", "00000000"]]
	]
	stick = [
		[["BS", "00000000"],  ["Tab", "00000000"],   ["Enter", "00000000"], ["Upper", "00000000"]],
		[["Del", "00000000"], ["Space", "00000000"], ["Esc", "00000000"],   ["Null", "00100000"]]
	]
	led = [
		[0xFF, 0x00, 0x8C],
		[0x00, 0xFF, 0xFF],
		[0x00, 0x00, 0xFF]
	]
    intensity = [
        1.0
    ]
```

## Installation
The `go install` is available.
```shellscript
$ go install github.com/yamamo2shun1/Confiseur/cmd/confiseur@v0.13.0
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
-intensity [float(0.0-1.0)]
        Set LED intensity.
        ex) confiseur -intensity 1.0
            confiseur -id 1 -intensity 0.5
-restart
        Restart the keyboard immediately.
        ex) confiseur -restart
            confiseur -restart -id 1
-factoryreset
        Reset all settings to factory defaults.
        ex) confiseur -factoryreset
            confiseur -id 0 -factoryreset
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
$ git clone https://github.com/yamamo2shun1/Confiseur
$ cd Confiseur/cmd/confiseur
$ go build -o confiseur
```

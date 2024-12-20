/*
 * keyboard.h
 *
 *  Created on: Aug 17, 2023
 *      Author: shun
 */

#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_

#define BASIC_PARAMS_NUM 5

#define MATRIX_ROWS    5
#define MATRIX_COLUMNS 13

#define KC_A        0x04  // a
#define KC_B        0x05  // b
#define KC_C        0x06  // c
#define KC_D        0x07  // d
#define KC_E        0x08  // e
#define KC_F        0x09  // f
#define KC_G        0x0A  // g
#define KC_H        0x0B  // h
#define KC_I        0x0C  // i
#define KC_J        0x0D  // j
#define KC_K        0x0E  // k
#define KC_L        0x0F  // l
#define KC_M        0x10  // m
#define KC_N        0x11  // n
#define KC_O        0x12  // o
#define KC_P        0x13  // p
#define KC_Q        0x14  // q
#define KC_R        0x15  // r
#define KC_S        0x16  // s
#define KC_T        0x17  // t
#define KC_U        0x18  // u
#define KC_V        0x19  // v
#define KC_W        0x1A  // w
#define KC_X        0x1B  // x
#define KC_Y        0x1C  // y
#define KC_Z        0x1D  // z
#define KC_1        0x1E  // 1!
#define KC_2        0x1F  // 2@
#define KC_3        0x20  // 3#
#define KC_4        0x21  // 4$
#define KC_5        0x22  // 5%
#define KC_6        0x23  // 6^
#define KC_7        0x24  // 7&
#define KC_8        0x25  // 8*
#define KC_9        0x26  // 9(
#define KC_0        0x27  // 0)
#define KC_ENTER    0x28  // Enter
#define KC_ESC      0x29  // Escape
#define KC_BS       0x2A  // Backspace
#define KC_TAB      0x2B  // Tab
#define KC_SPACE    0x2C  // Space
#define KC_MINUS    0x2D  // -_
#define KC_EQUAL    0x2E  // =+
#define KC_LSB      0x2F  // [{
#define KC_RSB      0x30  // ]}
#define KC_BSLASH   0x31  // \|
#define KC_SC       0x33  // ;:
#define KC_APS      0x34  // '"
#define KC_GA       0x35  // `~
#define KC_COMMA    0x36  // ,<
#define KC_PERIOD   0x37  // .>
#define KC_SLASH    0x38  // /?
#define KC_CAPSLOCK 0x39  // Caps Lock
#define KC_F1       0x3A  // F1
#define KC_F2       0x3B  // F2
#define KC_F3       0x3C  // F3
#define KC_F4       0x3D  // F4
#define KC_F5       0x3E  // F5
#define KC_F6       0x3F  // F6
#define KC_F7       0x40  // F7
#define KC_F8       0x41  // F8
#define KC_F9       0x42  // F9
#define KC_F10      0x43  // F10
#define KC_F11      0x44  // F11
#define KC_F12      0x45  // F12
#define KC_PRNTSCRN 0x46  // Print Screen
#define KC_SCRLLOCK 0x47  // Scroll Lock
#define KC_PAUSE    0x48  // Pause
#define KC_INSERT   0x49  // Insert
#define KC_HOME     0x4A  // Home
#define KC_PAGEUP   0x4B  // Page Up
#define KC_DELETE   0x4C  // Delete
#define KC_END      0x4D  // End
#define KC_PAGEDOWN 0x4E  // Page Down
#define KC_RIGHT    0x4F  // Right
#define KC_LEFT     0x50  // Left
#define KC_DOWN     0x51  // Down
#define KC_UP       0x52  // Up
#define KC_NUMLOCK  0x53  // Num Lock
#define KC_KATAKANA 0x88  // カタカナ ひらがな
#define KC_YEN      0x89  // ￥|
#define KC_HENKAN   0x8A  // 変換
#define KC_MUHENKAN 0x8B  // 無変換
#define KC_LCONTROL 0xE0  // Left Control
#define KC_LSHIFT   0xE1  // Left Shift
#define KC_LALT     0xE2  // Left Alt
#define KC_LGUI     0xE3  // Left GUI(Win/Command)
#define KC_RCONTROL 0xE4  // Right Control
#define KC_RSHIFT   0xE5  // Right Shift
#define KC_RALT     0xE6  // Right Alt
#define KC_RGUI     0xE7  // Right GUI(Win/Command)

// original code
#define KC_MGAIN_UP   0xFA  // Master Gain Up
#define KC_MGAIN_DOWN 0xFB  // Master Gain Down
#define KC_UPPER      0xFC  // Upper
#define KC_LNPH       0xFD  // Line Phono Swtich
#define KC_LAYOUT     0xFE  // Layout Switch
#define KC_NULL       0xFF

// modifier keys
#define M_NO 0x00  // No modifiers
#define M_LC 0x01  // Left Control
#define M_LS 0x02  // Left Shift
#define M_LA 0x04  // Left Alt
#define M_LG 0x08  // Left GUI(Win/Command)
#define M_RC 0x10  // Right Control
#define M_RS 0x20  // Right Shift
#define M_RA 0x40  // Right Alt
#define M_RG 0x80  // Right GUI(Win/Command)

#define H 0
#define V 1

void setKeymapID(const uint8_t val);
uint8_t getKeymapID(void);
void setLinePhonoSW(const uint8_t val);
uint8_t getLinePhonoSW(void);
void factoryReset(void);
void writeAllKeyboardSettings(void);
void loadKeyboardSettingsFromFlash(void);
uint8_t getKeyCode(const uint8_t keymapId, const uint8_t x, const uint8_t y);
void setKeyCode(const uint8_t keymapId, const uint8_t x, const uint8_t y, const uint8_t code);
uint8_t getModifiers(const uint8_t keymapId, const uint8_t x, const uint8_t y);
void setModifiers(const uint8_t keymapId, const uint8_t x, const uint8_t y, const uint8_t modifiers);
void resetKeys(void);
void clearKeys(const uint8_t code, const uint8_t modifiers);
void setKeys(const uint8_t code, const uint8_t modifiers);
void hid_keyscan_task(void);

#endif /* INC_KEYBOARD_H_ */

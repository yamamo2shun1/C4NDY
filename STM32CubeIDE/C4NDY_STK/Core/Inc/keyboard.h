/*
 * keyboard.h
 *
 *  Created on: Aug 17, 2023
 *      Author: shun
 */

#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_

#define MATRIX_ROWS    4
#define MATRIX_COLUMNS 10

#define SC_A        0x04  // a
#define SC_B        0x05  // b
#define SC_C        0x06  // c
#define SC_D        0x07  // d
#define SC_E        0x08  // e
#define SC_F        0x09  // f
#define SC_G        0x0A  // g
#define SC_H        0x0B  // h
#define SC_I        0x0C  // i
#define SC_J        0x0D  // j
#define SC_K        0x0E  // k
#define SC_L        0x0F  // l
#define SC_M        0x10  // m
#define SC_N        0x11  // n
#define SC_O        0x12  // o
#define SC_P        0x13  // p
#define SC_Q        0x14  // q
#define SC_R        0x15  // r
#define SC_S        0x16  // s
#define SC_T        0x17  // t
#define SC_U        0x18  // u
#define SC_V        0x19  // v
#define SC_W        0x1A  // w
#define SC_X        0x1B  // x
#define SC_Y        0x1C  // y
#define SC_Z        0x1D  // z
#define SC_1        0x1E  // 1!
#define SC_2        0x1F  // 2@
#define SC_3        0x20  // 3#
#define SC_4        0x21  // 4$
#define SC_5        0x22  // 5%
#define SC_6        0x23  // 6^
#define SC_7        0x24  // 7&
#define SC_8        0x25  // 8*
#define SC_9        0x26  // 9(
#define SC_0        0x27  // 0)
#define SC_ENTER    0x28  // Enter
#define SC_ESC      0x29  // Escape
#define SC_BS       0x2A  // Backspace
#define SC_TAB      0x2B  // Tab
#define SC_SPACE    0x2C  // Space
#define SC_MINUS    0x2D  // -_
#define SC_EQUAL    0x2E  // =+
#define SC_LSB      0x2F  // [{
#define SC_RSB      0x30  // ]}
#define SC_BSLASH   0x31  // \|
#define SC_SC       0x33  // ;:
#define SC_APS      0x34  // '"
#define SC_GA       0x35  // `~
#define SC_COMMA    0x36  // ,<
#define SC_PERIOD   0x37  // .>
#define SC_SLASH    0x38  // /?
#define SC_CAPSLOCK 0x39  // Caps Lock
#define SC_F1       0x3A  // F1
#define SC_F2       0x3B  // F2
#define SC_F3       0x3C  // F3
#define SC_F4       0x3D  // F4
#define SC_F5       0x3E  // F5
#define SC_F6       0x3F  // F6
#define SC_F7       0x40  // F7
#define SC_F8       0x41  // F8
#define SC_F9       0x42  // F9
#define SC_F10      0x43  // F10
#define SC_F11      0x44  // F11
#define SC_F12      0x45  // F12
#define SC_PRNTSCRN 0x46  // Print Screen
#define SC_SCRLLOCK 0x47  // Scroll Lock
#define SC_PAUSE    0x48  // Pause
#define SC_INSERT   0x49  // Insert
#define SC_HOME     0x4A  // Home
#define SC_PAGEUP   0x4B  // Page Up
#define SC_DELETE   0x4C  // Delete
#define SC_END      0x4D  // End
#define SC_PAGEDOWN 0x4E  // Page Down
#define SC_RIGHT    0x4F  // Right
#define SC_LEFT     0x50  // Left
#define SC_DOWN     0x51  // Down
#define SC_UP       0x52  // Up
#define SC_NUMLOCK  0x53  // Num Lock
#define SC_KATAKANA 0x88  // カタカナ ひらがな
#define SC_YEN      0x89  // ￥|
#define SC_HENKAN   0x8A  // 変換
#define SC_MUHENKAN 0x8B  // 無変換
#define SC_LCONTROL 0xE0  // Left Control
#define SC_LSHIFT   0xE1  // Left Shift
#define SC_LALT     0xE2  // Left Alt
#define SC_LGUI     0xE3  // Left GUI(Win/Command)
#define SC_RCONTROL 0xE4  // Right Control
#define SC_RSHIFT   0xE5  // Right Shift
#define SC_RALT     0xE6  // Right Alt
#define SC_RGUI     0xE7  // Right GUI(Win/Command)

// original code
#define SC_RESET      0xF9  // Reset
#define SC_MGAIN_UP   0xFA  // Master Gain Up
#define SC_MGAIN_DOWN 0xFB  // Master Gain Down
#define SC_UPPER      0xFC  // Upper
#define SC_LNPH       0xFD  // Line Phono Swtich
#define SC_LAYOUT     0xFE  // Layout Switch
#define SC_NULL       0xFF

#define H 0
#define V 1

#define MAX_LONG_PRESS_COUNT 500

void setLinePhonoSW(uint8_t val);
uint8_t getLinePhonoSW(void);
uint8_t getKeyCode(uint8_t keymapId, uint8_t x, uint8_t y);
uint8_t getUpperKeyCode(uint8_t keymapId, uint8_t x, uint8_t y);
uint8_t getStickKeyCode(uint8_t keymapId, uint8_t id, uint8_t direction);
void setKeyCode(uint8_t keymapId, uint8_t x, uint8_t y, uint8_t code);
void setUpperKeyCode(uint8_t keymapId, uint8_t x, uint8_t y, uint8_t code);
void setStickKeyCode(uint8_t keymapId, uint8_t id, uint8_t direction, uint8_t code);
void resetKeys(void);
void clearKeys(uint8_t code);
void setKeys(uint8_t code);
void hid_keyscan_task(void);

#endif /* INC_KEYBOARD_H_ */

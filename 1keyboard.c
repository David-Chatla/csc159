#include <kernel/logging.h>
#include <kernel/ports.h>
#include <stdint.h>

#define KEY_NULL 0
#define KEY_PRESSED(c) (!(c & 0x80))
#define KEY_STATUS_CTRL 0x01
#define KEY_STATUS_ALT 0x02
#define KEY_STATUS_SHIFT 0x04
#define KEY_STATUS_CAPS 0x08
#define KEY_STATUS_NUMLOCK 0x10
#define KEY_ESCAPE 0x01
#define KEY_CTRL_L 0x1D
#define KEY_CTRL_R 0x1D
#define KEY_ALT_L 0x38
#define KEY_ALT_R 0x38
#define KEY_SHIFT_L 0x2A
#define KEY_SHIFT_R 0x36
#define KEY_CAPS 0x3A
#define KEY_NUMLOCK 0x45

static uint8_t kbd_status = 0;
static uint8_t esc_status = 0;

static const char keyboard_map_primary[] = {
    // Mappings for primary keymap
};

static const char keyboard_map_secondary[] = {
    // Mappings for secondary keymap
};

void keyboard_init() {
    kernel_log_info("Initializing keyboard");
}

unsigned char keyboard_status(void) {
    return inportb(0x64);
}

unsigned int keyboard_scan(void) {
    unsigned int c = KEY_NULL;

    // Check if data is available
    if (keyboard_status() & 0x01) {
        c = inportb(0x60);
        kernel_log_trace("keyboard_scan() returns : %10x\n", c);
    }

    return c;
}

unsigned int keyboard_poll(void) {
    unsigned int c = KEY_NULL;

    if ((c = keyboard_scan()) != KEY_NULL) {
        c = keyboard_decode(c);
    }

    return c;
}

unsigned int keyboard_getc(void) {
    unsigned int c = KEY_NULL;
    while ((c = keyboard_poll()) == KEY_NULL);
    return c;
}

unsigned int keyboard_decode(unsigned int c) {
    unsigned int key_pressed = KEY_PRESSED(c);

    switch (c & ~0x80) {
        case KEY_CTRL_L:
        case KEY_CTRL_R:
            if (key_pressed) {
                if ((kbd_status & KEY_STATUS_CTRL) == 0) {
                    kernel_log_trace("keyboard: CTRL pressed");
                }
                kbd_status |= KEY_STATUS_CTRL;
            } else {
                if ((kbd_status & KEY_STATUS_CTRL) != 0) {
                    kernel_log_trace("keyboard: CTRL released");
                }
                kbd_status &= ~KEY_STATUS_CTRL;
            }
            break;

        case KEY_ALT_L:
        case KEY_ALT_R:
            if (key_pressed) {
                if ((kbd_status & KEY_STATUS_ALT) == 0) {
                    kernel_log_trace("keyboard: ALT pressed");
                }
                kbd_status |= KEY_STATUS_ALT;
            } else {
                if ((kbd_status & KEY_STATUS_ALT) != 0) {
                    kernel_log_trace("keyboard: ALT released");
                }
                kbd_status &= ~KEY_STATUS_ALT;
            }
            break;

        case KEY_SHIFT_L:
        case KEY_SHIFT_R:
            if (key_pressed) {
                if ((kbd_status & KEY_STATUS_SHIFT) == 0) {
                    kernel_log_trace("keyboard: SHIFT pressed");
                }
                kbd_status |= KEY_STATUS_SHIFT;
            } else {
                if ((kbd_status & KEY_STATUS_SHIFT) != 0) {
                    kernel_log_trace("keyboard: SHIFT released");
                }
                kbd_status &= ~KEY_STATUS_SHIFT;
            }
            break;

        case KEY_CAPS:
            if (key_pressed) {
                kernel_log_trace("keyboard: CAPS pressed");
                kbd_status ^= KEY_STATUS_CAPS;
            } else {
                kernel_log_trace("keyboard: CAPS released");
            }
            break;

        case KEY_NUMLOCK:
            if (key_pressed) {
                kbd_status ^= KEY_STATUS_NUMLOCK;
            }
            break;

        default:
            if (!key_pressed) {
                break;
            }

            if (c >= 0x47 && c <= 0x53) {
                if ((kbd_status & KEY_STATUS_NUMLOCK) != 0) {
                    c = keyboard_map_secondary[c];
                } else {
                    c = keyboard_map_primary[c];
                }
            } else if ((((kbd_status & KEY_STATUS_SHIFT) != 0)
               ^ ((kbd_status & KEY_STATUS_CAPS) != 0)) != 0) {
                c = keyboard_map_secondary[c];
            } else {
                c = keyboard_map_primary[c];
            }

            if (kbd_status & KEY_STATUS_ALT) {
                if (c >= '0' && c <= '9') {
                    int n = c - '0';
                    n++;
                    return KEY_NULL;
                }
            }

            if (c == KEY_ESCAPE) {
                esc_status++;
                if (esc_status == 3) {
                    kernel_exit();
                }
                return KEY_NULL;
            } else {
                esc_status = 0;
            }

            if (kbd_status & KEY_STATUS_CTRL) {
                if (c == '+' || c == '=') {
                    kernel_set_log_level(kernel_get_log_level() + 1);
                    return KEY_NULL;
                } else if (c == '-' || c == '_') {
                    kernel_set_log_level(kernel_get_log_level() - 1);
                    return KEY_NULL;
                }
            }

            if (c) {
                if (c >= 0x20 && c <= 0x7f) {
                    kernel_log_trace("keyboard: character [0x%02x] '%c'", c, c);
                } else {
                    kernel_log_trace("keyboard: character [0x%02x]", c);
                }
                return c;
            }
            break;
    }
    return KEY_NULL;
}

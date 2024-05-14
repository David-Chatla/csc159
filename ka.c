#include "io.h"

#include "kernel.h"

#include "keyboard.h"

// Define constants for special keys

#define KEY_PRESS_BIT 0x80

#define KEY_CODE_MASK 0x7F

/**

 * Initializes keyboard data structures and variables

 */

void keyboard_init() {

    kernel_log_info("Initializing keyboard driver");

}

/**

 * Scans for keyboard input and returns the raw character data

 * @return raw character data from the keyboard

 */

unsigned int keyboard_scan(void) {

    unsigned int c = inportb(0x60);  // Read the keyboard data port (0x60)

    return c;

}

/**

 * Polls for a keyboard character to be entered.

 *

 * If a keyboard character data is present, will scan and return

 * the decoded keyboard output.

 *

 * @return decoded character or KEY_NULL (0) for any character

 * that cannot be decoded

 */

unsigned int keyboard_poll(void) {

    unsigned int c = keyboard_scan();

    // Check if the key is being pressed (bit 7 set)

    if (c & KEY_PRESS_BIT) {

        // Extract the key code (bits 0-6)

        unsigned int key_code = c & KEY_CODE_MASK;

        

        // Decode the key code and return the corresponding character

        return keyboard_decode(key_code);

    }

    // Key is not being pressed, return KEY_NULL

    return KEY_NULL;

}

/**

 * Blocks until a keyboard character has been entered

 * @return decoded character entered by the keyboard or KEY_NULL

 * for any character that cannot be decoded

 */

unsigned int keyboard_getc(void) {

    unsigned int c = KEY_NULL;

    while ((c = keyboard_poll()) == KEY_NULL);

    return c;

}

/**

 * Processes raw keyboard input and decodes it.

 *

 * Should keep track of the keyboard status for the following keys:

 * SHIFT, CTRL, ALT, CAPS, NUMLOCK

 *

 * For all other characters, they should be decoded/mapped to ASCII

 * or ASCII-friendly characters.

 *

 * For any character that cannot be mapped, KEY_NULL should be returned.

 *

  If all* of the status keys defined in KEY_KERNEL_DEBUG are pressed,

 * while another character is entered, the kernel_debug_command()

 * function should be called.

 */

unsigned int keyboard_decode(unsigned int key_code) {

    // Add your keyboard decoding logic here

    // You need to handle special keys and map other keys to ASCII characters

    

    // Example: mapping key code 'A' (0x1E) to ASCII 'a'

    if (key_code == 0x1E) {

        return 'a';

    }

    

    // Handle other key codes here...

    return KEY_NULL;  // Default: key not mapped

}

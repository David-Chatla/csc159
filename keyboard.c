/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Keyboard Functions
 */
#include "io.h"
#include "kernel.h"
#include "keyboard.h"

char ascii_values[] = {                                                 
      '\0', '\001', '\002', '\003', '\004', '\005', '\006', '\007',                         
     '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',                       
      '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',                    
     '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',                        
       ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',      
     '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',              
       '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',            
      'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',                 
       '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',              
      'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', '\177'                                        
  }; 

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
    unsigned int c = KEY_NULL;        
    c = inportb(0x60);                
     kernel_log_info("keyboard_scan() returns: %10x\n",c);                  
     return c;                         
 }    

/**
 * Polls for a keyboard character to be entered.
 *
 * If a keyboard character data is present, will scan and return
 * the decoded keyboard output.
 *
 * @return decoded character or KEY_NULL (0) for any character
 *         that cannot be decoded
 */
unsigned int keyboard_poll(void) {   
   unsigned int c = KEY_NULL;        
    if((c = keyboard_scan()) != KEY_NULL)                                           
   {                                
        c = keyboard_decode(c);                                   
    }                                                             
    return c;                                                     
}

/**
 * Blocks until a keyboard character has been entered
 * @return decoded character entered by the keyboard or KEY_NULL
 *         for any character that cannot be decoded
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
 *   SHIFT, CTRL, ALT, CAPS, NUMLOCK
 *
 * For all other characters, they should be decoded/mapped to ASCII
 * or ASCII-friendly characters.
 *
 * For any character that cannot be mapped, KEY_NULL should be returned.
 *
 * If *all* of the status keys defined in KEY_KERNEL_DEBUG are pressed,
 * while another character is entered, the kernel_debug_command()
 * function should be called.
 */
unsigned int keyboard_decode(unsigned int c) {
    // Check if the key pressed is a special function key
    if (c & 0x80) {
        // Key release event, ignore for now
        return KEY_NULL;
    }

    // Check for modifier keys (SHIFT, CTRL, ALT)
    // For now, we'll ignore these and simply return the ASCII value
    if (c >= 0x80 && c <= 0x9C) {
        // Key is a special key, return KEY_NULL
        return KEY_NULL;
    }

    // Check for non-alphanumeric keys
    switch (c) {
        case 0x1C: return '\n';  // Enter key
        case 0x0E: return '\b';  // Backspace key
        // Add more cases for special keys as needed
    }

    // Check if it's a printable ASCII character
    if (c < sizeof(ascii_values)) {
        return ascii_values[c];
    }

    // Key is not mapped to a printable ASCII character
    return KEY_NULL;
}

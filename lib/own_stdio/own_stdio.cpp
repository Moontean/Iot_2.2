#include <own_stdio.h> // Include custom stdio interface

static FILE stream = {0}; // перенаправление сериала функции // Custom FILE stream used for stdio redirection

LiquidCrystal_I2C lcd(  LCD_ADDRESS, // LCD I2C device address
                        LCD_COLUMNS, // LCD number of columns
                        LCD_ROWS // LCD number of rows
); // Construct LCD object with configured parameters

char keys[KEYPAD_ROWS][KEYPAD_COLS] ={ // Keymap for 4x4 keypad matrix
                {'1','2','3','A'}, // Row 1 characters
                {'4','5','6','B'}, // Row 2 characters
                {'7','8','9','C'}, // Row 3 characters
                {'*','0','#','D'} // Row 4 characters
}; // End of keypad keymap

byte rowPins[KEYPAD_ROWS] = { // Physical pins for keypad rows
                    KEYPAD_ROW_1_PIN, // Row 1 pin
                    KEYPAD_ROW_2_PIN, // Row 2 pin
                    KEYPAD_ROW_3_PIN, // Row 3 pin
                    KEYPAD_ROW_4_PIN // Row 4 pin
}; // End of row pin array

byte colPins[KEYPAD_COLS] = { // Physical pins for keypad columns
                    KEYPAD_COL_1_PIN, // Column 1 pin
                    KEYPAD_COL_2_PIN, // Column 2 pin
                    KEYPAD_COL_3_PIN, // Column 3 pin
                    KEYPAD_COL_4_PIN // Column 4 pin
}; // End of column pin array

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS ); // Create keypad object with keymap and pins

int keypad_getchar(FILE *f) // getchar implementation using keypad input
{
    char key = kpd.getKey(); // Read a key from keypad
    while(key == NO_KEY) // Wait until a key is pressed
    {
        key = kpd.getKey(); // Keep polling keypad
    }
    return key; // Return the pressed key code
}

// Non-blocking keypad poll: returns key or 0
char keypad_peek()
{
    char k = kpd.getKey();
    if (k == NO_KEY) return 0;
    return k;
}

int lcd_putchar(char ch, FILE *f) // putchar implementation for LCD output
{
    static uint8_t nr_of_displayed_chars = 0; // Track characters written in the current row
    static uint8_t current_row = 0; // Track current LCD row
    if (ch == '\n') // Handle newline: move to next row
    {
        current_row++; // Increment row counter
        if (current_row >= LCD_ROWS) // If past last row, clear and wrap
        {
            lcd.clear(); // Clear screen
            current_row = 0; // Wrap to first row
        }
        nr_of_displayed_chars = 0; // Reset column counter
        lcd.setCursor(0, current_row); // Move cursor to start of new row
        return 0; // Indicate success
    }
    else if (ch == '\r') // Ignore carriage return
    {
        return 0; // Do nothing
    }
    else if (ch == '\f') // Form feed: clear display and reset position
    {
        lcd.clear(); // Clear LCD content
        current_row = 0; // Reset row to 0
        nr_of_displayed_chars = 0; // Reset column counter
        lcd.setCursor(0, current_row); // Move cursor to start
        return 0; // Indicate success

    }
    if (nr_of_displayed_chars >= LCD_COLUMNS || ch == '\n') // If row is full or newline
    {
        current_row++; // Move to next row
        if (current_row >= LCD_ROWS) // Wrap if beyond last row
        {
            lcd.clear(); // Clear display
            current_row = 0; // Reset to first row
        }
        nr_of_displayed_chars = 0; // Reset column count
        lcd.setCursor(0, current_row); // Move cursor to beginning of row
    }
    lcd.write(ch); // Write character to LCD
    nr_of_displayed_chars++; // Increment column count

    return 0; // Indicate success
}
int serial_putchar(char c, FILE *f) // putchar for Serial output
{
    Serial.write(c); // Send character over Serial
    return 0; // Indicate success
}

int serial_getchar(FILE *f) // getchar for Serial input
{
    while (!Serial.available()); // Wait until data is available
    return Serial.read(); // Read and return one byte
}

void own_stdio_init(const uint32_t baudrate) // Initialize LCD, Serial, and stdio redirection
{
    lcd.init();                      // initialize the lcd  // Initialize LCD hardware
    lcd.backlight(); // Turn on LCD backlight

    fdev_setup_stream ( &stream, // Configure stdio stream binding
                        serial_putchar, // Set putchar handler (Serial)
                        serial_getchar, // Set getchar handler (Serial)
                        _FDEV_SETUP_RW // указание, того, что мы хотим совершить чтение и запись // Indicate read/write mode for stream
    ); // End of stream setup
    stdout = &stream; // Redirect stdout to configured stream
    stdin = &stream; // Redirect stdin to configured stream
}


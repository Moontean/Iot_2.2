#include <own_stdio.h>

static FILE stream = {0}; //перенаправление сериала функции

LiquidCrystal_I2C lcd(  LCD_ADDRESS,
                        LCD_COLUMNS,
                        LCD_ROWS
);

char keys[KEYPAD_ROWS][KEYPAD_COLS] ={
                {'1','2','3','A'},
                {'4','5','6','B'},
                {'7','8','9','C'},
                {'*','0','#','D'}
};

byte rowPins[KEYPAD_ROWS] = {
                    KEYPAD_ROW_1_PIN,
                    KEYPAD_ROW_2_PIN,
                    KEYPAD_ROW_3_PIN,
                    KEYPAD_ROW_4_PIN
};

byte colPins[KEYPAD_COLS] = {
                    KEYPAD_COL_1_PIN,
                    KEYPAD_COL_2_PIN,
                    KEYPAD_COL_3_PIN,
                    KEYPAD_COL_4_PIN
};

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS );

int keypad_getchar(FILE *f)
{
    char key = kpd.getKey();
    while(key == NO_KEY)
    {
        key = kpd.getKey();
    }
    return key;
}

int lcd_putchar(char ch, FILE *f)
{
    static uint8_t nr_of_displayed_chars = 0;
    static uint8_t current_row = 0;
    if (ch == '\n')
    {
        current_row++;
        if (current_row >= LCD_ROWS)
        {
            lcd.clear();
            current_row = 0;
        }
        nr_of_displayed_chars = 0;
        lcd.setCursor(0, current_row);
        return 0;
    }
    else if (ch == '\r') 
    {
        return 0;
    }
    else if (ch == '\f')
    {
        lcd.clear();
        current_row = 0;
        nr_of_displayed_chars = 0;
        lcd.setCursor(0, current_row);
        return 0;

    }
    if (nr_of_displayed_chars >= LCD_COLUMNS || ch == '\n')
    {
        current_row++;
        if (current_row >= LCD_ROWS)
        {
            lcd.clear();
            current_row = 0;
        }
        nr_of_displayed_chars = 0;
        lcd.setCursor(0, current_row);
    }
    lcd.write(ch);
    nr_of_displayed_chars++;

    return 0;
}
int serial_putchar(char c, FILE *f)
{
    Serial.write(c);
    return 0;
}

int serial_getchar(FILE *f)
{
    while (!Serial.available());
    return Serial.read();
}

void own_stdio_init(const uint32_t baudrate)
{
    lcd.init();                      // initialize the lcd 
    lcd.backlight();

    fdev_setup_stream ( &stream,
                        serial_putchar,
                        keypad_getchar,
                        _FDEV_SETUP_RW //указание, того, что мы хотим совершить чтение и запись
    );
    stdout = &stream;
    stdin = &stream;
}


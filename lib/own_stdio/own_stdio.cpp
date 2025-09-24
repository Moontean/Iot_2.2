#include <own_stdio.h>

static FILE stream = {0}; //перенаправление сериала функции


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
    Serial.begin(baudrate);

    fdev_setup_stream ( &stream,
                        serial_putchar, 
                        serial_getchar,
                        _FDEV_SETUP_RW //указание, того, что мы хотим совершить чтение и запись
    );
    stdout = &stream;
    stdin = &stream;
}

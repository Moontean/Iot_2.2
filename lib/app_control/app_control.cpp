#include <app_control.h>

void app_control_init()
{
    own_stdio_init(BAUDRATE);
    led_control_init(RED_LED_PIN);
    led_control_init(GREEN_LED_PIN);
    printf("App control is initialized.\n\r");

}

void app_control_run()
{
    uint32_t timeout = 0;
    uint8_t input_chars_count = 0;
    char ch = 0;
    char input_buffer[INPUT_BUFFER_SIZE] = {0};
    printf("\fEnter a password: ");
    while (input_chars_count < PASSWORD_LENGTH)
    {
        scanf("%c", &ch);
        if (ch == '#')
        {
            return;
        }
        
        printf("*");

        input_buffer[input_chars_count] = ch;
        input_chars_count++;
    }
    if (strncmp(input_buffer, PASSWORD, PASSWORD_LENGTH) == 0 )
    {
        printf("\n Access granted!");
        led_on(GREEN_LED_PIN);
        timeout = millis() + LED_GREEN_TIMEOUT;
    }
    else{
        printf("\n Access denied!");
        led_on(RED_LED_PIN);
        timeout = millis() + LED_RED_TIMEOUT;
    }

    while (millis () < timeout)
    {
        
    }
    led_off(RED_LED_PIN);
    led_off(GREEN_LED_PIN);
}

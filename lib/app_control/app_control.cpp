#include <app_control.h>

static char *commands[] = {
    "led_on",
    "led_off",
};

static void (*command_functions[])(const uint8_t) = {
    led_on,
    led_off
};

void app_control_init()
{
    own_stdio_init(BAUDRATE);
    led_control_init(LED_PIN);
    printf("App control is initialized.\n\r");
    uint8_t num_commands = sizeof(commands) / sizeof(commands[0]);
    for (uint8_t i = 0; i < num_commands; i++)
    {
        printf("Available command: %s\n\r", commands[i]);
    }
}

void execute_command(const char* command)
{
    uint8_t num_commands = sizeof(commands) / sizeof(commands[0]);
    for (uint8_t i = 0; i < num_commands; i++)
    {
        if (strcmp(command, commands[i]) == 0)
        {
            command_functions[i](LED_PIN);
            printf("Executed command: %s\n\r", command);
            return;
        }
    }
    printf("Unknown command: %s\n\r", command);
}

void app_control_run()
{
    char str[INPUT_BUFFER_SIZE] = {0};
    printf("Enter a string: %s\n\r");

    scanf("%9s", str);
    execute_command(str);
}
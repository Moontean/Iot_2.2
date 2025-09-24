#include <Arduino.h>
#include <own_stdio.h>
#include <app_control.h>


void setup()
{
  // own_stdio_init(BAUDRATE);
  // printf("System initialized.\n\r");

  app_control_init();
}

void loop()
{
  // char str[INPUT_BUFFER_SIZE] = {0};
  // printf("Enter a string: ");

  // scanf("%9s", str);
  // printf("You entered: %s\n\r", str);

  app_control_run();
}
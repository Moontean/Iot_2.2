#include <Arduino.h>
#include <own_stdio.h>
#include <app_control.h>


void setup()
{
  app_control_init();
}

void loop()
{
  app_control_run();
}
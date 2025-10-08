#include <Arduino.h>
#include <tasks.h>

void setup()
{
  tasks_init();
}

void loop()
{
  // Empty loop as tasks are handled by timer interrupts
}
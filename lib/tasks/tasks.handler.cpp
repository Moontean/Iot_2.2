#include "tasks.h"


void tasks_init(void)
{
   xTaskCreate(first_task, 
                "First Task", 
                128, 
                NULL,
                1,
                NULL);
    xTaskCreate(second_task,
                "Second Task",
                128,
                NULL,
                1,
                NULL);
}

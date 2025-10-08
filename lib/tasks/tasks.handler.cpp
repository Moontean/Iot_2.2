#include "tasks.h"


void tasks_init(void)
{
   timer_init_ISR_1KHz(TIMER_DEFAULT);
}

void tasks_update(void)
{
    static uint32_t first_task_counter = FIRST_TASK_OFFSET_MS;
    static uint32_t second_task_counter = SECOND_TASK_OFFSET_MS;

    if (--first_task_counter == 0)
    {
        first_task_counter = FIRST_TASK_RECURRENCE_MS;
        first_task(NULL);
    }
    if (--second_task_counter == 0)
    {
        second_task_counter = SECOND_TASK_RECURRENCE_MS;
        second_task(NULL);
    }
}

void timer_handle_interrupts(int timer)
{
    tasks_update();
}
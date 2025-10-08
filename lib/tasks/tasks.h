#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>
#include <led_control.h>
#include <button_control.h>
#include <config.hpp>
#include <timer_setup.h>

void tasks_init(void);
void tasks_update(void);

void first_task(void* args);
void second_task(void* args);
void third_task(void* args);
void idle_task(void* args);

#endif //TASKS_H
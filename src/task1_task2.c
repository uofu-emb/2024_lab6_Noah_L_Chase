#include "FreeRTOS.h"
#include "task.h"
#include "task_functions.h"

void busy_busy(void)
{
    for (int i = 0; ; i++);
}

void busy_yield(void)
{
    for (int i = 0; ; i++) {
        taskYIELD();
    }
}

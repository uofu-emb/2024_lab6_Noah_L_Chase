#include "FreeRTOSConfig.h"

#include "FreeRTOS.h"
#include "task.h"

#include "semphr.h"

#include "Threads.h"

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"

#define THREAD1_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define THREAD2_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2UL )
#define THREAD3_TASK_PRIORITY     ( tskIDLE_PRIORITY + 3UL )
#define SUPERVISOR_TASK_PRIORITY ( tskIDLE_PRIORITY + 4UL )

#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

int main(void)
{
    stdio_init_all();

    // Create shared semaphore
    SemaphoreHandle_t sharedSem = xSemaphoreCreateBinary();

    if( sharedSem == NULL )
    {
        printf("Problem creating semaphore\n");
    }

    const char *rtos_name;
    rtos_name = "FreeRTOS";
    TaskHandle_t taskThread1;
    TaskHandle_t taskThread2;
    TaskHandle_t taskThread3;
    TaskHandle_t taskSupervisor;
    xTaskCreate(Thread1, "Thread1",
                MAIN_TASK_STACK_SIZE, sharedSem, THREAD1_TASK_PRIORITY, &taskThread1);
    xTaskCreate(Thread2, "Thread2",
                MAIN_TASK_STACK_SIZE, sharedSem, THREAD2_TASK_PRIORITY, &taskThread2);
    xTaskCreate(Thread3, "Thread3",
                MAIN_TASK_STACK_SIZE, sharedSem, THREAD3_TASK_PRIORITY, &taskThread3);
    xTaskCreate(Supervisor, "Supervisor",
                MAIN_TASK_STACK_SIZE, NULL, SUPERVISOR_TASK_PRIORITY, &taskSupervisor);
    vTaskStartScheduler();
    return 0;
}
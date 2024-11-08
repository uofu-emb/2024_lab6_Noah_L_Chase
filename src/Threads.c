#include "FreeRTOSConfig.h"

#include "FreeRTOS.h"
#include "task.h"

#include "semphr.h"

#include "Threads.h"

#include "stdio.h"

void Thread1(void * args)
{
    SemaphoreHandle_t* sharedSem = (SemaphoreHandle_t*)args;

    xSemaphoreTake( *sharedSem, portMAX_DELAY );
    printf("Got semaphore %s\n", __func__);
    vTaskDelay(1500);

    printf("Giving semaphore %s\n", __func__);
    xSemaphoreGive(*sharedSem);

    printf("Leaving %s\n", __func__);

    vTaskDelete(NULL);
    return;
}

void Thread2(void * args)
{
    vTaskDelay(500);

    volatile int counter = 0;
    while(1)
    {
        counter++;

        if( counter > 5000000 )
        {
            printf("Reseting counter\n");
            counter = 0;
        }
    }

    printf("Leaving %s\n", __func__);

    vTaskDelete(NULL);
    return;
}

void Thread3(void * args)
{
    vTaskDelay(1000);
    SemaphoreHandle_t* sharedSem = (SemaphoreHandle_t*)args;

    printf("Attempting to get semaphore %s\n", __func__);
    xSemaphoreTake( *sharedSem, portMAX_DELAY );
    printf("Got semaphore %s\n", __func__);

    printf("Giving semaphore %s\n", __func__);
    xSemaphoreGive(*sharedSem);

    printf("Leaving %s\n", __func__);
    // while(1);
    vTaskDelete(NULL);
    return;
}

void Supervisor(void* args)
{
    const UBaseType_t uxArraySize = 5;  // Adjust based on the number of tasks
    TaskStatus_t xTaskDetails[uxArraySize];
    UBaseType_t uxArrayUsed;
    uint32_t ulTotalRunTime;

    while (1) {
        // Get task statistics
        uxArrayUsed = uxTaskGetSystemState(xTaskDetails, uxArraySize, &ulTotalRunTime);

        printf("Task Statistics:\n");
        for (UBaseType_t i = 0; i < uxArrayUsed; i++) {
            printf("Task Name: %s\n", xTaskDetails[i].pcTaskName);
            printf("  State: %d\n", xTaskDetails[i].eCurrentState);
            printf("  Priority: %d\n", xTaskDetails[i].uxCurrentPriority);
            printf("  Run Time: %u\n", xTaskDetails[i].ulRunTimeCounter);
            printf("  Stack High Water Mark: %u\n", xTaskDetails[i].usStackHighWaterMark);
        }

        printf("Total Run Time: %u\n\n", ulTotalRunTime);

        // Delay to prevent excessive printing
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    printf("Leaving %s\n", __func__);
    vTaskDelete(NULL);
    return;
}
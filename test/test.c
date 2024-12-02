#include <pico/stdlib.h>

#include "FreeRTOSConfig.h"

#include "FreeRTOS.h"
#include "task.h"

#include "semphr.h"

#include "Threads.h"

#include "pico/time.h"

#include <stdio.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"

TaskHandle_t priorityInversionThread;
TaskHandle_t priorityInversionThreadMutex;
TaskHandle_t testRunnerTask;

void setUp(void) {}

void tearDown(void) {}

void PriorityInversionTest(void)
{
    printf("Starting the priority inversion test with binary semaphore\n");
    // Create shared semaphore
    SemaphoreHandle_t sharedSem = xSemaphoreCreateBinary();

    if( sharedSem == NULL )
    {
        printf("Problem creating semaphore\n");
    }

    xSemaphoreGive(sharedSem);

    TaskHandle_t taskThread1;
    TaskHandle_t taskThread2;
    TaskHandle_t taskThread3;
    TaskHandle_t taskSupervisor;
    xTaskCreate(Thread1, "Thread1",
                MAIN_TASK_STACK_SIZE, &sharedSem, THREAD1_TASK_PRIORITY, &taskThread1);
    xTaskCreate(Thread2, "Thread2",
                MAIN_TASK_STACK_SIZE, &sharedSem, THREAD2_TASK_PRIORITY, &taskThread2);
    xTaskCreate(Thread3, "Thread3",
                MAIN_TASK_STACK_SIZE, &sharedSem, THREAD3_TASK_PRIORITY, &taskThread3);
    xTaskCreate(Supervisor, "Supervisor",
                MAIN_TASK_STACK_SIZE, NULL, SUPERVISOR_TASK_PRIORITY, &taskSupervisor);

    vTaskDelay(5000);

    // Test that the semaphore is NOT available
    TEST_ASSERT_EQUAL(0, uxSemaphoreGetCount(sharedSem));

    vTaskDelete(taskThread1);
    vTaskDelete(taskThread2);
    vTaskDelete(taskThread3);
    vTaskDelete(taskSupervisor);
    vSemaphoreDelete(sharedSem);
}

void PriorityInversionTestMutex(void)
{
    printf("Starting the priority inversion test with mutex semaphore\n");
    // Create shared semaphore
    SemaphoreHandle_t sharedSem = xSemaphoreCreateMutex();

    if( sharedSem == NULL )
    {
        printf("Problem creating semaphore\n");
    }

    TaskHandle_t taskThread1;
    TaskHandle_t taskThread2;
    TaskHandle_t taskThread3;
    TaskHandle_t taskSupervisor;
    xTaskCreate(Thread1, "Thread1",
                MAIN_TASK_STACK_SIZE, &sharedSem, THREAD1_TASK_PRIORITY, &taskThread1);
    xTaskCreate(Thread2, "Thread2",
                MAIN_TASK_STACK_SIZE, &sharedSem, THREAD2_TASK_PRIORITY, &taskThread2);
    xTaskCreate(Thread3, "Thread3",
                MAIN_TASK_STACK_SIZE, &sharedSem, THREAD3_TASK_PRIORITY, &taskThread3);
    xTaskCreate(Supervisor, "Supervisor",
                MAIN_TASK_STACK_SIZE, NULL, SUPERVISOR_TASK_PRIORITY, &taskSupervisor);

    vTaskDelay(5000);

    // Test that the semaphore is available
    TEST_ASSERT_EQUAL(1, uxSemaphoreGetCount(sharedSem));

    vTaskDelete(taskThread1);
    vTaskDelete(taskThread2);
    vTaskDelete(taskThread3);
    vTaskDelete(taskSupervisor);
    vSemaphoreDelete(sharedSem);
}

void testRunner(void* args)
{
    while(1)
    {
        sleep_ms(5000); // Give time for TTY to attach.
        printf("Start tests\n");
        UNITY_BEGIN();
        RUN_TEST(PriorityInversionTest);
        vTaskDelay(100);
        RUN_TEST(PriorityInversionTestMutex);
        UNITY_END();
    }
}

int main (void)
{
    stdio_init_all();
    xTaskCreate(testRunner, "testRunnerThread",
                MAIN_TASK_STACK_SIZE, NULL, 10, &testRunnerTask);

    vTaskStartScheduler();
    return 0;
}

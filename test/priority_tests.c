#include <pico/stdlib.h>

#include "Threads.h"
#include "unity.h"

#include "FreeRTOSConfig.h"

#include "FreeRTOS.h"
#include "task.h"

#include "semphr.h"

#include "task_functions.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unity.h>
#include "unity_config.h"

// Define task priorities using macros
#define TASK_PRIORITY_1 (tskIDLE_PRIORITY + 1UL)
#define TASK_PRIORITY_2 (tskIDLE_PRIORITY + 2UL)

// Function declarations
void testRunner2(void* args);

// Task handles
TaskHandle_t xTask1, xTask2;
TaskHandle_t testRunner2Task;

void setUp(void) {}

void tearDown(void) {}

// Helper function to print which task is running
void print_task_info(const char *task_name) {
    printf("%s is running\n", task_name);
}

// Task wrapper for busy_busy function
void vBusyBusyTask1(void *args) {
    print_task_info("Task1: busy_busy");
    for (int i = 0; ; i++);
    // busy_busy();
}

void vBusyBusyTask2(void *args) {
    print_task_info("Task2: busy_busy");
    for (int i = 0; ; i++);
    // busy_busy();
}

// Task wrapper for busy_yield function
void vBusyYieldTask1(void *args) {
    print_task_info("Task1: busy_yield");
    // busy_yield();
    for (int i = 0; ; i++) {
        taskYIELD();
    }
}

void vBusyYieldTask2(void *args) {
    print_task_info("Task2: busy_yield");
    // busy_yield();
    for (int i = 0; ; i++) {
        taskYIELD();
    }
}

// Helper function to gather and print runtime statistics and test each case
void gather_runtime_stats(const char *test_name, bool equal, bool task1Larger, bool skip) {
    UBaseType_t numTasks = uxTaskGetNumberOfTasks();
    UBaseType_t arraySize = 20;
    TaskStatus_t xTaskDetails[arraySize];
    UBaseType_t uxArraySize = uxTaskGetSystemState(xTaskDetails, arraySize, NULL);

    uint32_t variance = 5;

    uint32_t task1Runtime = 0;
    uint32_t task2Runtime = 0;


    for(int i = 0; i < uxArraySize; i++)
    {
        if( strcmp(xTaskDetails[i].pcTaskName, "Task1") == 0 )
        {
            task1Runtime = xTaskDetails[i].ulRunTimeCounter;
        }

        if( strcmp( xTaskDetails[i].pcTaskName, "Task2") == 0 )
        {
            task2Runtime = xTaskDetails[i].ulRunTimeCounter;
        }
    }

    if( skip == true )
    {
        bool testPassed;
        if( equal == true )
        {
            TEST_ASSERT_EQUAL(task1Runtime, task2Runtime);
            TEST_ASSERT_INT_WITHIN(150, task1Runtime, task2Runtime);
        }
        else if( task1Larger == true )
        {
            TEST_ASSERT_TRUE( task1Runtime > task2Runtime );
        }
        else
        {
            TEST_ASSERT_TRUE( task2Runtime > task1Runtime );
        }
    }
}

// Test scenarios
void test_same_priority_busy_busy(void) {
    if (xTask1 != NULL) vTaskDelete(xTask1);
    if (xTask2 != NULL) vTaskDelete(xTask2);
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask1);
    xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_same_priority_busy_busy", true, false, false);

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
    xTask1 = NULL;
    xTask2 = NULL;
}

void test_same_priority_busy_yield(void) {
    if (xTask1 != NULL) vTaskDelete(xTask1);
    if (xTask2 != NULL) vTaskDelete(xTask2);
    xTaskCreate(vBusyYieldTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask1);
    xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_same_priority_busy_yield", true, false, false);

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
    xTask1 = NULL;
    xTask2 = NULL;
}

void test_same_priority_mixed(void) {
    if (xTask1 != NULL) vTaskDelete(xTask1);
    if (xTask2 != NULL) vTaskDelete(xTask2);
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask1);
    xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_same_priority_mixed", false, true, false);

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
    xTask1 = NULL;
    xTask2 = NULL;
}

void test_different_priority_busy_busy_high_first(void) {
    if (xTask1 != NULL) vTaskDelete(xTask1);
    if (xTask2 != NULL) vTaskDelete(xTask2);
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_2, &xTask1);
    xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_different_priority_busy_busy_high_first", false, true, false);

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
    xTask1 = NULL;
    xTask2 = NULL;
}

void test_different_priority_busy_busy_low_first(void) {
    if (xTask1 != NULL) vTaskDelete(xTask1);
    if (xTask2 != NULL) vTaskDelete(xTask2);
    xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_2, &xTask1);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_different_priority_busy_busy_low_first", false, true, false);

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
    xTask1 = NULL;
    xTask2 = NULL;
}

void test_different_priority_busy_yield_high_first(void) {
    if (xTask1 != NULL) vTaskDelete(xTask1);
    if (xTask2 != NULL) vTaskDelete(xTask2);
    xTaskCreate(vBusyYieldTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_2, &xTask1);
    xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_different_priority_busy_yield_high_first", false, true, false);

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
    xTask1 = NULL;
    xTask2 = NULL;
}

// Test runner to execute all tests
void testRunner2(void* args) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000)); // Give time for TTY to attach
        printf("Start tests\n");
        UNITY_BEGIN();

        // gather_runtime_stats("nothing");

        RUN_TEST(test_same_priority_busy_busy);
        vTaskDelay(pdMS_TO_TICKS(100));
        RUN_TEST(test_same_priority_busy_yield);
        vTaskDelay(pdMS_TO_TICKS(100));
        RUN_TEST(test_same_priority_mixed);
        vTaskDelay(pdMS_TO_TICKS(100));
        RUN_TEST(test_different_priority_busy_busy_high_first);
        vTaskDelay(pdMS_TO_TICKS(100));
        RUN_TEST(test_different_priority_busy_busy_low_first);
        vTaskDelay(pdMS_TO_TICKS(100));
        RUN_TEST(test_different_priority_busy_yield_high_first);

        UNITY_END();
        vTaskDelete(NULL); // Stop this task after tests complete
    }
}

int main(void) {
    stdio_init_all();

    xTaskCreate(testRunner2, "trThread",
                configMINIMAL_STACK_SIZE, NULL, 10, &testRunner2Task);

    vTaskStartScheduler();

    return 0;
}
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
    busy_busy();
}

void vBusyBusyTask2(void *args) {
    print_task_info("Task2: busy_busy");
    busy_busy();
}

// Task wrapper for busy_yield function
void vBusyYieldTask1(void *args) {
    print_task_info("Task1: busy_yield");
    busy_yield();
}

void vBusyYieldTask2(void *args) {
    print_task_info("Task2: busy_yield");
    busy_yield();
}

// Helper function to gather and print runtime statistics and test each case
void gather_runtime_stats(const char *test_name) {
    UBaseType_t numTasks = uxTaskGetNumberOfTasks();
    UBaseType_t arraySize = 20;
    TaskStatus_t xTaskDetails[arraySize];
    UBaseType_t uxArraySize = uxTaskGetSystemState(xTaskDetails, arraySize, NULL);
    printf("Size of stats == %u\n", uxArraySize);
    printf("Num actual tasks == %u\n", numTasks);

    for(int i = 0; i < uxArraySize; i++)
    {
        printf("Thread running: %s\n", xTaskDetails[i].pcTaskName);
        printf("Task Run Time: %u\n", xTaskDetails[i].ulRunTimeCounter);
    }
}

// Test scenarios
void test_same_priority_busy_busy(void) {
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask1);
    xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_same_priority_busy_busy");

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
}

void test_same_priority_busy_yield(void) {
    xTaskCreate(vBusyYieldTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask1);
    xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_same_priority_busy_yield");

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
}

void test_same_priority_mixed(void) {
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask1);
    xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_same_priority_mixed");

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
}

void test_different_priority_busy_busy_high_first(void) {
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_2, &xTask1);
    xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_different_priority_busy_busy_high_first");

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
}

void test_different_priority_busy_busy_low_first(void) {
    xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_2, &xTask1);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_different_priority_busy_busy_low_first");

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
}

void test_different_priority_busy_yield_high_first(void) {
    xTaskCreate(vBusyYieldTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_2, &xTask1);
    xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
    vTaskDelay(pdMS_TO_TICKS(1000));

    gather_runtime_stats("test_different_priority_busy_yield_high_first");

    vTaskDelete(xTask1);
    vTaskDelete(xTask2);
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
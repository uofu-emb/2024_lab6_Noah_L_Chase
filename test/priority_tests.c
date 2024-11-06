#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

void busy_busy(void);
void busy_yield(void);

TaskHandle_t xTask1, xTask2;

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

// Test scenarios
void test_same_priority_busy_busy(void) {
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, &xTask1);
    xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, &xTask2);
    vTaskStartScheduler();
}

void test_same_priority_busy_yield(void) {
    xTaskCreate(vBusyYieldTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, &xTask1);
    xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, &xTask2);
    vTaskStartScheduler();
}

void test_same_priority_mixed(void) {
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, &xTask1);
    xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, &xTask2);
    vTaskStartScheduler();
}

void test_different_priority_busy_busy_high_first(void) {
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, 2, &xTask1);
    xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, &xTask2);
    vTaskStartScheduler();
}

void test_different_priority_busy_busy_low_first(void) {
    xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, &xTask2);
    xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, 2, &xTask1);
    vTaskStartScheduler();
}

void test_different_priority_busy_yield_high_first(void) {
    xTaskCreate(vBusyYieldTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, 2, &xTask1);
    xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, &xTask2);
    vTaskStartScheduler();
}

int main(void) {
    // Select test case by uncommenting one of the following lines

    // Test cases for same priority
    // test_same_priority_busy_busy();
    // test_same_priority_busy_yield();
    // test_same_priority_mixed();

    // Test cases for different priorities
    // test_different_priority_busy_busy_high_first();
    // test_different_priority_busy_busy_low_first();
    // test_different_priority_busy_yield_high_first();

    return 0;
}

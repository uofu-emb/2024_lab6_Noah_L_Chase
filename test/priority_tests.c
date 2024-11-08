#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

#include "task_functions.h"
#include "Threads.h"
#include "unity.h"

// Define task priorities using macros
#define TASK_PRIORITY_1 (tskIDLE_PRIORITY + 1UL)
#define TASK_PRIORITY_2 (tskIDLE_PRIORITY + 2UL)

// Function declarations
void busy_busy(void);
void busy_yield(void);
void testRunner2(void* args);

// Task handles
TaskHandle_t xTask1, xTask2;
TaskHandle_t testRunner2Task;

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
    TaskStatus_t xTaskDetails[2];
    UBaseType_t uxArraySize = uxTaskGetSystemState(xTaskDetails, 2, NULL);

    // Assert that exactly two tasks were retrieved
    TEST_ASSERT_EQUAL(2, uxArraySize);
    printf("== %s ==\n", test_name);
    printf("Task1 Run Time: %u\n", xTaskDetails[0].ulRunTimeCounter);
    printf("Task2 Run Time: %u\n", xTaskDetails[1].ulRunTimeCounter);
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
        sleep_ms(5000); // Give time for TTY to attach
        printf("Start tests\n");
        UNITY_BEGIN();

        RUN_TEST(test_same_priority_busy_busy);
        RUN_TEST(test_same_priority_busy_yield);
        RUN_TEST(test_same_priority_mixed);
        RUN_TEST(test_different_priority_busy_busy_high_first);
        RUN_TEST(test_different_priority_busy_busy_low_first);
        RUN_TEST(test_different_priority_busy_yield_high_first);

        UNITY_END();
        vTaskDelete(NULL); // Stop this task after tests complete
    }
}

int main(void) {
    stdio_init_all();

    xTaskCreate(testRunner2, "testRunner2Thread",
                configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &testRunner2Task);

    vTaskStartScheduler();

    return 0;
}


// #include "FreeRTOS.h"
// #include "task.h"
// #include <stdio.h>

// #include "task_functions.h"
// #include "Threads.h"

// // Define task priorities using macros
// #define TASK_PRIORITY_1 (tskIDLE_PRIORITY + 1UL)
// #define TASK_PRIORITY_2 (tskIDLE_PRIORITY + 2UL)

// void busy_busy(void);
// void busy_yield(void);

// TaskHandle_t xTask1, xTask2;

// TaskHandle_t testRunner2Task;
// TaskHandle_t test_same_priority_busy_busy_Task;
// TaskHandle_t test_same_priority_busy_yield_Task;
// TaskHandle_t test_same_priority_mixed_Task;
// TaskHandle_t test_different_priority_busy_busy_high_first_Task;
// TaskHandle_t test_different_priority_busy_busy_low_first_Task;
// TaskHandle_t test_different_priority_busy_yield_high_first_Task;

// // Helper function to print which task is running
// void print_task_info(const char *task_name) {
//     printf("%s is running\n", task_name);
// }

// // Task wrapper for busy_busy function
// void vBusyBusyTask1(void *args) {
//     print_task_info("Task1: busy_busy");
//     busy_busy();
// }

// void vBusyBusyTask2(void *args) {
//     print_task_info("Task2: busy_busy");
//     busy_busy();
// }

// // Task wrapper for busy_yield function
// void vBusyYieldTask1(void *args) {
//     print_task_info("Task1: busy_yield");
//     busy_yield();
// }

// void vBusyYieldTask2(void *args) {
//     print_task_info("Task2: busy_yield");
//     busy_yield();
// }

// // Test scenarios
// void test_same_priority_busy_busy(void) {
//     xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask1);
//     xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
// }

// void test_same_priority_busy_yield(void) {
//     xTaskCreate(vBusyYieldTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask1);
//     xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
// }

// void test_same_priority_mixed(void) {
//     xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask1);
//     xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
// }

// void test_different_priority_busy_busy_high_first(void) {
//     xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_2, &xTask1);
//     xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
// }

// void test_different_priority_busy_busy_low_first(void) {
//     xTaskCreate(vBusyBusyTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
//     xTaskCreate(vBusyBusyTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_2, &xTask1);
// }

// void test_different_priority_busy_yield_high_first(void) {
//     xTaskCreate(vBusyYieldTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_2, &xTask1);
//     xTaskCreate(vBusyYieldTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY_1, &xTask2);
// }

// void testRunner2(void* args)
// {
//     while(1)
//     {
//         sleep_ms(5000); // Give time for TTY to attach.
//         printf("Start tests\n");
//         UNITY_BEGIN();
//         RUN_TEST(test_same_priority_busy_busy);
//         RUN_TEST(test_same_priority_busy_yield);
//         RUN_TEST(test_same_priority_mixed);
//         RUN_TEST(test_different_priority_busy_busy_high_first);
//         RUN_TEST(test_different_priority_busy_busy_low_first);
//         RUN_TEST(test_different_priority_busy_yield_high_first);
//         UNITY_END();
//     }
// }


// int main(void) {
//     // Select test case by uncommenting one of the following lines

//     // Test cases for same priority
//     // test_same_priority_busy_busy();
//     // test_same_priority_busy_yield();
//     // test_same_priority_mixed();

//     // Test cases for different priorities
//     // test_different_priority_busy_busy_high_first();
//     // test_different_priority_busy_busy_low_first();
//     // test_different_priority_busy_yield_high_first();

//     stdio_init_all();

//     xTaskCreate(testRunner2, "testRunner2Thread",
//                 MAIN_TASK_STACK_SIZE, NULL, 30, &testRunner2Task);

//     vTaskStartScheduler();

//     return 0;
// }

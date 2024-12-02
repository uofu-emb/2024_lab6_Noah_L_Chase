#ifndef THREADS_H
#define THREADS_H

#define THREAD1_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define THREAD2_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2UL )
#define THREAD3_TASK_PRIORITY     ( tskIDLE_PRIORITY + 3UL )
#define SUPERVISOR_TASK_PRIORITY ( tskIDLE_PRIORITY + 4UL )

#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

void Thread1(void * args);

void Thread2(void* args);

void Thread3(void* args);

void Supervisor(void* args);

#endif
//
// Created by 23391 on 2024/7/24.
//

#include "rtthread.h"


static rt_sem_t dynamic_sem = RT_NULL;

static rt_thread_t task1;
static rt_thread_t task2;

void task_produce_chocolate(void * parameter)
{
    while(1){
        rt_kprintf("task_produce_chocolate\r\n");
        rt_sem_release(dynamic_sem);
        rt_thread_delay(2000);
    }
}


void task_consumer(void * parameter)
{
    while(1){
        rt_kprintf("current Number %d\r\n", dynamic_sem->value);
        if (rt_sem_take(dynamic_sem, 300))
        {
            rt_kprintf("task_consumer\r\n");
        }
        rt_thread_delay(500);
    }
}


int task_entry(){
    dynamic_sem = rt_sem_create("sem1", 10, RT_IPC_FLAG_PRIO);
    if (dynamic_sem !=RT_NULL){
        task1 =  rt_thread_create("task_produce_chocolate",task_produce_chocolate, RT_NULL,  256, 20, 50);
        rt_thread_startup(task1);
        task2 =  rt_thread_create("task_consumer",task_consumer, RT_NULL,  256, 20, 50);
        rt_thread_startup(task2);
    }
    return 0;
}

MSH_CMD_EXPORT(task_entry, Producer and consumer )


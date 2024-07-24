
#include "rtthread.h"


static rt_mutex_t mutex = RT_NULL;

static rt_thread_t task1;
static rt_thread_t task2;
static rt_thread_t task3;

static int index = 0;

void currentIndex(void * parameter)
{
    while(1){
        rt_kprintf("current value %d \r\n", index);
        rt_thread_delay(1000);
    }
}



void task1_index_increase(void * parameter)
{
    while(1){
        if (rt_mutex_take(mutex,100)){
            index ++;
            rt_mutex_release(mutex);
            rt_thread_delay(500);
        }
    }
}


void task2_index_increase(void * parameter)
{
    if (rt_mutex_take(mutex,100)){
        index ++;
        rt_mutex_release(mutex);
        rt_thread_delay(500);
    }
}


int mutex_entry(){
    mutex = rt_mutex_create("sem1", RT_IPC_FLAG_PRIO);
    if (mutex !=RT_NULL){
        task1 =  rt_thread_create("task_produce_chocolate",task1_index_increase, RT_NULL,  256, 20, 50);
        rt_thread_startup(task1);
        task2 =  rt_thread_create("task_consumer",task2_index_increase, RT_NULL,  256, 20, 50);
        rt_thread_startup(task2);
        task3 =  rt_thread_create("currentIndex",currentIndex, RT_NULL,  256, 20, 50);
        rt_thread_startup(task3);
    }
    return 0;
}

MSH_CMD_EXPORT(mutex_entry, mutex test)


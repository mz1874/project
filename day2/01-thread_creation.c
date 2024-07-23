#include "rtthread.h"

/**
 * This task will be crated by a static way
 * @param parameter
 */
static void staticTask(void * parameter)
{
    while(1){
        rt_kprintf("This is a static Task\r\n");
        rt_thread_delay(500);
    }
}

/**
 * This task will be crated dynamically
 * @param parameter
 */
void dynamicTask(void * parameter)
{
    while(1){
        rt_kprintf("This is a dynamicTask Task\r\n");
        rt_thread_delay(1000);
    }
}

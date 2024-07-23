/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2018-11-19     flybreak     add stm32f429-fire-challenger bsp
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


static struct rt_thread thread1;

rt_thread_t thread2;

rt_err_t err;

static uint8_t static_stack[256];


/**
 * This task will  deleted by rt_thread_detach once i == 10
 * @param parameter
 */
static void staticTask(void * parameter)
{
    int i =0;
    while(1){
        if(i==10){
            rt_kprintf("Deleted staticTask Task\r\n");
            rt_thread_t t = rt_thread_self();
            rt_thread_detach(t);
        }
        rt_kprintf("This is a staticTask Task\r\n");
        i++;
        rt_thread_delay(500);
    }
}

/**
 * This task will be crated dynamically and deleted by rt_thread_delete once i == 10
 * @param parameter
 */
void dynamicTask(void * parameter)
{
    int i =0;
    while(1){
        if(i==10){
            rt_kprintf("Deleted dynamicTask Task\r\n");
            rt_thread_t t = rt_thread_self();
            rt_thread_delete(t);
        }
        rt_kprintf("This is a dynamicTask Task\r\n");
        i++;
        rt_thread_delay(500);
    }
}

int main(void) {
    err = rt_thread_init(&thread1,
                         "staticTask",
                         staticTask,
                         RT_NULL,
                         static_stack,
                         sizeof(static_stack),
                         25, 5);
    if (err == RT_EOK) {
        rt_thread_startup(&thread1);
    }

    thread2 = rt_thread_create("dynamicTask", dynamicTask, RT_NULL, 512, 25, 5);
    rt_thread_startup(thread2);

    return 0;
}

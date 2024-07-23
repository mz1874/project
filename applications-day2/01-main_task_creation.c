

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "01-thread_creation.c"




static struct rt_thread thread1;

rt_thread_t thread2;

rt_err_t err;

static uint8_t static_stack[256];


extern void staticTask(void *parameter);

extern void dynamicTask(void *parameter);


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

    thread2 = rt_thread_create("dynamicTask", dynamicTask, RT_NULL, 512, 30, 20);
    rt_thread_startup(thread2);

    return 0;
}

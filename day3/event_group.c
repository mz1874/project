#include "rtthread.h"

static rt_event_t event = RT_NULL;

static rt_thread_t task1;
static rt_thread_t task2;
static rt_thread_t task3;

static rt_uint32_t received;
static int index = 0;

void currentIndex_event(void *parameter) {
    while (1) {
        rt_kprintf("current value %d \r\n", index);
        rt_thread_delay(1000);
    }
}

void task1_send_event(void *parameter) {
    while (1) {
        // 发送0x01事件
        rt_event_send(event, 0x01);
        rt_thread_delay(500);
    }
}

void task2_receive_event(void *parameter) {
    while (1) {
        if (rt_event_recv(event, 0x01, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, 1000, &received) == RT_EOK) {
            index++;
            rt_kprintf("received %d\r\n", received);
        }
        rt_thread_delay(500);
    }
}

int event_entry() {
    event = rt_event_create("sem1", RT_IPC_FLAG_PRIO);
    if (event != RT_NULL) {
        task1 = rt_thread_create("task_produce_chocolate", task1_send_event, RT_NULL, 256, 20, 50);
        rt_thread_startup(task1);
        task2 = rt_thread_create("task_consumer", task2_receive_event, RT_NULL, 256, 20, 50);
        rt_thread_startup(task2);
        task3 = rt_thread_create("currentIndexEvent", currentIndex_event, RT_NULL, 256, 20, 50);
        rt_thread_startup(task3);
    }
    return 0;
}

MSH_CMD_EXPORT(event_entry, event test)

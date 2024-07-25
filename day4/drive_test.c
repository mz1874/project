//
// Created by 23391 on 2024/7/25.
//
#include "rtthread.h"

static int test_drive_entry(void *args) {
    rt_device_t dev = rt_device_find("test_dev");
    if (dev == RT_NULL) {
        rt_kprintf("init failed!");
        return -RT_ERROR;
    }

    rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
    rt_device_control(dev, RT_DEVICE_CTRL_CONFIG, RT_NULL);
    rt_device_write(dev, 100, RT_NULL, 1024);
    rt_device_read(dev, 20, RT_NULL, 128);
    rt_device_close(dev);

    return RT_EOK;
}

MSH_CMD_EXPORT(test_drive_entry, test_drive_entry);
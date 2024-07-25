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
    const rt_uint8_t buffer[1] ={0x00};
    rt_kprintf("read----------------------------------");
    rt_uint8_t receive = 0x00;
    rt_device_write(dev, 0, &buffer, sizeof (buffer)/sizeof (rt_uint8_t));
    rt_device_read(dev, 13, &receive, 128);
    rt_device_close(dev);
    rt_kprintf("buffer--------------------- %d", receive);
    return RT_EOK;
}

MSH_CMD_EXPORT(test_drive_entry, test_drive_entry);
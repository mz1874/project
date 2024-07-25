#include "rtthread.h"
#include "rtdevice.h"

#ifndef RTTHREAD_DRV_TEST_H
#define RTTHREAD_DRV_TEST_H

#endif //RTTHREAD_DRV_TEST_H

#if defined(BSP_USING_TEST)
static rt_err_t dev_test_init(rt_device_t dev)
{
    //初始化按键
    rt_pin_mode(GET_PIN(C,13), PIN_MODE_INPUT_PULLDOWN);
    rt_kprintf("dev_test_init\r\n");
    return RT_EOK;
}

static rt_err_t dev_test_open(rt_device_t dev, rt_uint16_t oflag)
{
    //初始化LED
    rt_pin_mode(GET_PIN(H,11), PIN_MODE_OUTPUT);
    rt_kprintf("dev_test_open\r\n");
    return RT_EOK;
}



static rt_err_t dev_test_close(rt_device_t dev)
{
    rt_kprintf("dev_test_close\r\n");
    return RT_EOK;
}


static rt_ssize_t dev_test_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    if (size >= sizeof(int)) // Ensure there is enough space in the buffer
    {
        int pin_value = rt_pin_read(GET_PIN(C, pos));
        *((int *)buffer) = pin_value;
    }
    else
    {
        return -RT_ERROR; // Return an error if the buffer size is not sufficient
    }
    rt_kprintf("dev_test_read\r\n");
    return RT_EOK;
}


static rt_ssize_t dev_test_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{

    int value = *((int *)buffer);
    rt_pin_write(GET_PIN(H, 11), (rt_uint8_t)value);

    rt_kprintf("dev_test_write\r\n");
    return RT_EOK;
}



static rt_err_t dev_test_control(rt_device_t dev, int cmd, void *args)
{
    rt_kprintf("dev_test_control\r\n");
    return RT_EOK;
}


int rt_drv_test_init(void) {
    rt_device_t test_dev = rt_device_create(RT_Device_Class_Char, 0);
    if (!test_dev) {
        rt_kprintf("Test dev create failed!");
        return -RT_ERROR;
    }
    test_dev->init = dev_test_init;
    test_dev->open = dev_test_open;
    test_dev->close = dev_test_close;
    test_dev->read = dev_test_read;
    test_dev->write = dev_test_write;
    test_dev->control = dev_test_control;

    if (rt_device_register(test_dev, "test_dev", RT_DEVICE_FLAG_RDWR) != RT_EOK) {
        rt_kprintf("Test dev register failed!");
        return -RT_ERROR;
    }
    return RT_EOK;
}

INIT_BOARD_EXPORT(rt_drv_test_init);

#endif
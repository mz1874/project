#include "rtthread.h"
#include "rtdevice.h"

static rt_device_t device;

static int register_device()
{
    device = rt_device_create(RT_Device_Class_Char,256);
    if (device == RT_NULL){
        rt_kprintf("init unsuccessful !\r\n");
        return RT_ERROR;
    }
    if (RT_EOK !=rt_device_register(device, "device", RT_DEVICE_FLAG_RDWR))
    {
        rt_kprintf("register unsuccessful !\r\n");
        return RT_ERROR;
    }
    return RT_EOK;
}

MSH_CMD_EXPORT(register_device, register_device)
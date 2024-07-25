# 设备和驱动

- [官方文档](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/device)写的太详细了。因此我这里仅仅从官方手册里摘录出来一部分
同时写一下我对这些的理解
---
### 设备驱动模型

![io-dev.png](..%2Fass%2Fday4%2Fio-dev.png)

通常我们在裸机开发的时候一般都是直接操作设备驱动层的函数，这些函数分别由各个芯片的厂商进行提供。那么就会出现了一个问题，如果你更换了一个单片机型号
那么很有可能像原本的设备驱动的函数都发生了变换，所以对于用户而言，那就是重复造轮子。这个问题在RT-thread 中就得到了一个很好的解决。RT-Thread官方
提供了很多BSP的支持包，在这些支持包里面，RT-thread 对不同单片机型号的驱动库进行了封装。 也就是通过设备驱动层的框架里，把对应的外设作为一个设备
注册到RT-thread的设备管理器中实现了通一的管理。这样就是相对于对所有的设备驱动做了一层抽象封装。那么对于用户而言，用户只需要关心和学习如何使用RT-
thread 抽象出来的这一层就可以了。复杂的工作RT-thread官方团队已经为我们做好了。我们可以查看对应开发板上的README.md来查看具体支持了什么板载的驱动

**简单设备注册时序图**

![io-call.png](..%2Fass%2Fday4%2Fio-call.png)


设备的注册函数如下
```c
rt_err_t rt_device_register(rt_device_t dev, const char* name, rt_uint8_t flags);
```

我们在设备注册这一块应该着重关心于这个flags参数，它分别支持以下类型
```c
#define RT_DEVICE_FLAG_RDONLY       0x001 /* 只读 */
#define RT_DEVICE_FLAG_WRONLY       0x002 /* 只写  */
#define RT_DEVICE_FLAG_RDWR         0x003 /* 读写  */
#define RT_DEVICE_FLAG_REMOVABLE    0x004 /* 可移除  */
#define RT_DEVICE_FLAG_STANDALONE   0x008 /* 独立   */
#define RT_DEVICE_FLAG_SUSPENDED    0x020 /* 挂起  */
#define RT_DEVICE_FLAG_STREAM       0x040 /* 流模式  */
#define RT_DEVICE_FLAG_INT_RX       0x100 /* 中断接收 */
#define RT_DEVICE_FLAG_DMA_RX       0x200 /* DMA 接收 */
#define RT_DEVICE_FLAG_INT_TX       0x400 /* 中断发送 */
#define RT_DEVICE_FLAG_DMA_TX       0x800 /* DMA 发送 */
```


当我们把这个设备注册到设备管理器中之后，可以在finsh中通过命令行 `list device` 来查看所有的注册设备
![list_device.jpg](..%2Fass%2Fday4%2Flist_device.jpg)

用户可以在工程的root path 下使用 Conemn 工具使用 menuconfig 命令来打开或者关闭对应的设备驱动

![menu-config.jpg](..%2Fass%2Fday4%2Fmenu-config.jpg)

下述代码将演示如何注册一个设备到设备管理器中，我这里注册的是一个char设备

```c
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
```

注册成功后的设备管理器中的设备如下所示：

![register-device.jpg](..%2Fass%2Fday4%2Fregister-device.jpg)


通过拿SPI举例。 我们可以轻松的使用如下函数向SPI BUS上挂载一个SPI设备，并且执行SPI 的CS引脚

```c
static int spi_attach(void )
{
    return rt_hw_spi_device_attach("spi5","spi5-test", GET_PIN(F, 6));
}
```

但是上述需要注意的一点是，RTT的最新版本这个Function 做了一点轻微的调整，虽然上述function还可以被使用，但是仍然推荐使用下面的方式进行挂载。

```c
rt_err_t rt_spi_bus_attach_device_cspin(struct rt_spi_device *device,
                                        const char           *name,
                                        const char           *bus_name,
                                        rt_base_t            cs_pin,
                                        void                 *user_data)
```


同理，下述为一个使用GPIO设备的代码

```c
static void key_down(void *args)
{
    if(rt_pin_read(GET_PIN(C,13)))
    {
        rt_kprintf("KEY 13 has been pressed\r\n");
        LOG_D("SSS\r\n");
    }
}

static int key_entry(void *args)
{
    // 配置引脚模式为下拉输入
    rt_pin_mode(GET_PIN(C,13), PIN_MODE_INPUT_PULLDOWN);

    // 配置上升沿触发中断
    rt_pin_attach_irq(GET_PIN(C,13), PIN_IRQ_MODE_RISING, key_down, NULL);

    // 启用引脚中断
    rt_pin_irq_enable(GET_PIN(C,13), PIN_IRQ_ENABLE);

    return RT_EOK;
}

MSH_CMD_EXPORT(key_entry, key_entry);

```


## 驱动开发

那么我们如何来自己编写一个板载的驱动函数呢？

1- 在Library HAL driver下 创建一个命名前缀为drv_的.c 文件
2- 通过下面的函数来注册IO设备

```C
rt_device_t rt_device_create(int type, int attach_size);
```

注意现在编写的代码为下图中的设备驱动框架层
![io-dev.png](..%2Fass%2Fday4%2Fio-dev.png)

3-设备创建完之后我们需要，给上述创建的rt_device 对象指定它的各个方法
```c
struct rt_device
{
    struct rt_object          parent;        /* 内核对象基类 */
    enum rt_device_class_type type;          /* 设备类型 */
    rt_uint16_t               flag;          /* 设备参数 */
    rt_uint16_t               open_flag;     /* 设备打开标志 */
    rt_uint8_t                ref_count;     /* 设备被引用次数 */
    rt_uint8_t                device_id;     /* 设备 ID,0 - 255 */

    /* 数据收发回调函数 */
    rt_err_t (*rx_indicate)(rt_device_t dev, rt_size_t size);
    rt_err_t (*tx_complete)(rt_device_t dev, void *buffer);

    const struct rt_device_ops *ops;    /* 设备操作方法 */

    /* 设备的私有数据 */
    void *user_data;
};
typedef struct rt_device *rt_device_t;
```

如下所示
```c
struct rt_device_ops
{
    /* common device interface */
    rt_err_t  (*init)   (rt_device_t dev);
    rt_err_t  (*open)   (rt_device_t dev, rt_uint16_t oflag);
    rt_err_t  (*close)  (rt_device_t dev);
    rt_size_t (*read)   (rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size);
    rt_size_t (*write)  (rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size);
    rt_err_t  (*control)(rt_device_t dev, int cmd, void *args);
};
```

分别是初始化，打开， 关闭，读，写 和控制方法。

如下代码所示
```c
#include "rtthread.h"
#include "rtdevice.h"

#ifndef RTTHREAD_DRV_TEST_H
#define RTTHREAD_DRV_TEST_H

#endif //RTTHREAD_DRV_TEST_H

#if defined(BSP_USING_TEST)
static rt_err_t dev_test_init(rt_device_t dev)
{
    rt_kprintf("dev_test_init\r\n");
    return RT_EOK;
}

static rt_err_t dev_test_open(rt_device_t dev, rt_uint16_t oflag)
{
    rt_kprintf("dev_test_open\r\n");
    return RT_EOK;
}



static rt_err_t dev_test_close(rt_device_t dev)
{
    rt_kprintf("dev_test_close\r\n");
    return RT_EOK;
}


static rt_ssize_t dev_test_read(rt_device_t dev, rt_off_t pos, void * buffer , rt_size_t size)
{
    rt_kprintf("dev_test_read\r\n");
    return RT_EOK;
}

static rt_ssize_t dev_test_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
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
```

4- 在对应的驱动函数完成之后呢，便可以修改当前目录下的Sconscript脚本，使其Scons可以找到当前新增的驱动库
```c
if GetDepend(['BSP_USING_USBH']):
    src += ['drv_test.c']
```

5 - 修改根目录board 下的Kconfig, 配置在使用menuconfig时对驱动函数的状态
```c
config BSP_USING_TEST
        bool "ENABLE test driver"
        default n
```

<p style="color: red">注意这里使用的宏BSP_USING_TEST， 一定要和你代码中定义的宏保持一直。 Menuconfig会自动帮我们定义这个宏</p>

6 - 使用menuconfig 开启这个自定义的驱动
![drive.jpg](..%2Fass%2Fday4%2Fdrive.jpg)

7-在上层应用层使用创建好的自定义驱动
```c
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
```

8 - 观察实验现象

![Effect.jpg](..%2Fass%2Fday4%2FEffect.jpg)


##自定义驱动，定义一个function可以开启LED和读取按键的状态

```c
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
```

上述代码在驱动函数的基础上做了一点小小的修改， 1- 初始化了LED 2- 初始化了KEY。当上层程序执行的时候可以控制LED的开关和读取LED的状态。

```c
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
```
上述代码找到了我们之前初始化的设备并且向LED（低电平点亮）寄存器的写0，即打开LED. 并且读取Key的状态并且打印
![read.jpg](..%2Fass%2Fday4%2Fread.jpg)

**现象**

![img.jpg](..%2Fass%2Fday4%2Fimg.jpg)
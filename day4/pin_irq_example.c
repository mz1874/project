#include "rtthread.h"
#include "rtdevice.h"
#include "drv_gpio.h"
#include "ulog.h"
#define LOG_LEL LOG_LVL_DBG
#define LOG_TAG "pin_irq_example"
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

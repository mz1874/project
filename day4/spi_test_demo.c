//
// Created by 23391 on 2024/7/25.
//
#include "rtthread.h"
#include "rtdevice.h"
#include <drv_spi.h>
#include <drv_gpio.h>
#include <drv_test.c>

static int spi_attach(void )
{
    return rt_hw_spi_device_attach("spi5","spi5-test", GET_PIN(F, 6));
}

INIT_DEVICE_EXPORT(spi_attach);


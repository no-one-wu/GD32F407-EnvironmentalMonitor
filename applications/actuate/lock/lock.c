/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-24     lzh28       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "lock.h"

rt_base_t lock_pin = RT_NULL;
void Lock_Init(void)
{
    lock_pin = rt_pin_get("PD.15");  //输出
    rt_pin_mode(lock_pin, PIN_MODE_OUTPUT);
}

void Lock_Control(rt_bool_t level)
{
    if(lock_pin == RT_NULL)
        Lock_Init();
    if (level == RT_TRUE)
        rt_pin_write(lock_pin, PIN_HIGH);
    else
        rt_pin_write(lock_pin, PIN_LOW);
}

/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-27     wcj          PA.0 alarm light with 2s auto-off
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "alarm.h"

static rt_base_t alarm_pin = RT_NULL;
static rt_tick_t alarm_tick = 0;
static rt_bool_t alarm_on = RT_FALSE;

void Alarm_Init(void)
{
    alarm_pin = rt_pin_get("PA.0");
    rt_pin_mode(alarm_pin, PIN_MODE_OUTPUT);
    rt_pin_write(alarm_pin, PIN_LOW);
}

void Alarm_Trigger(void)
{
    if (alarm_pin == RT_NULL)
        Alarm_Init();
    rt_pin_write(alarm_pin, PIN_HIGH);
    alarm_tick = rt_tick_get();
    alarm_on = RT_TRUE;
}

void Alarm_Update(void)
{
    if (!alarm_on)
        return;
    if (rt_tick_get() - alarm_tick >= RT_TICK_PER_SECOND * 2)
    {
        rt_pin_write(alarm_pin, PIN_LOW);
        alarm_on = RT_FALSE;
    }
}

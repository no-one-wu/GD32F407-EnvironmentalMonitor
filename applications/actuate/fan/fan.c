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
#include "fan.h"

#define PWM_DEV_NAME        "pwm2"  /* PWM设备名称 */
#define PWM_DEV_CHANNEL     1       /* PWM通道 */
#define FANfrequency 20000000         //周期
#define FANpwd(x) (FANfrequency/100*x)  //舵机角度
static struct rt_device_pwm *pwm_dev = RT_NULL;      /* PWM设备句柄 */

void Fan_init(int argc)
{
    if(argc<0&&argc>100)
        return;
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
    }
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, FANfrequency, FANpwd(argc));
    rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
}
void seg_Fan_pwd(int argc)
{
    if(argc<0&&argc>100)
        return;
    else if(pwm_dev==RT_NULL)
        return;
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, FANfrequency, FANpwd(argc));
}

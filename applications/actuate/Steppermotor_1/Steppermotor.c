/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-07-14     Administrator       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "Steppermotor.h"
#include "bkrc_voice.h"
#include <math.h>
#include <rtdbg.h>
#include "led.h"


char Stepper_mode = 0; // 1 = 清洁模式 ，2 滑台复位模式
rt_base_t EN_PIN = RT_NULL, DIR_PIN = RT_NULL, SDA_PIN = RT_NULL, SCL_PIN = RT_NULL, HightV_PIN = RT_NULL;

static void GPIO_IRQHandler(void *param)
{

    if (rt_pin_read(SCL_PIN) == 1&&rt_pin_read(DIR_PIN)==0)
    {
        rt_pin_write(DIR_PIN, 1); // dir  0向右 ，1向左
    }
    else if (rt_pin_read(SDA_PIN) == 1&&rt_pin_read(DIR_PIN)==1)
    {

        rt_pin_write(DIR_PIN, 0); // dir  0向右 ，1向左
        rt_pin_write(EN_PIN, 1); // EN   1除能，0使能
        rt_pin_write(HightV_PIN, 1); // 1除能 0使能
        if (Stepper_mode == 1)
        {
            voice_broadcast(4);

        }
        Stepper_mode = 0;
    }
}

/*
    函数作用：初始化PWM芯片
    参数：1.PWM频率
          2.初始化舵机角度
*/
void Stepper_motor_Init(float hz, int angle)
{
    EN_PIN = rt_pin_get("PC.0");//50    01
    DIR_PIN = rt_pin_get("PC.1");//51   02
    SDA_PIN = rt_pin_get("PA.6");//38   05   左
    SCL_PIN = rt_pin_get("PA.7");//39   06   右
    HightV_PIN = rt_pin_get("PD.15");//45

    rt_pin_mode(EN_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(DIR_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(SDA_PIN,PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(SCL_PIN,PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(HightV_PIN,PIN_MODE_OUTPUT);

    rt_pin_detach_irq(SDA_PIN);
    rt_pin_attach_irq(SDA_PIN,PIN_IRQ_MODE_RISING,GPIO_IRQHandler,NULL);
    rt_pin_irq_enable(SDA_PIN, PIN_IRQ_ENABLE);

    rt_pin_detach_irq(SCL_PIN);
    rt_pin_attach_irq(SCL_PIN,PIN_IRQ_MODE_RISING,GPIO_IRQHandler,NULL);
    rt_pin_irq_enable(SCL_PIN, PIN_IRQ_ENABLE);

    rt_pin_write(HightV_PIN,PIN_HIGH);// 1除能 0使能
    rt_pin_write(DIR_PIN,PIN_LOW);    // dir  0向右 ，1向左
    rt_pin_write(EN_PIN,PIN_HIGH);    // EN   1除能，0使能

//    set_frequency(hz);
    pwm_blue_init(hz);
//    gpio_enable(50, DIR_OUT); // EN
//    gpio_enable(51, DIR_OUT); // DIR
//    gpio_enable(38, DIR_IN);  //  左边 SDA
//    gpio_enable(39, DIR_IN);  //  右边 SCL
//    gpio_enable(45, DIR_OUT); // 高压模块

//    ls1x_disable_gpio_interrupt(38);
//    ls1x_install_gpio_isr(38, INT_TRIG_EDGE_UP, gpio_interrput_isr, NULL);
//    ls1x_enable_gpio_interrupt(38);
//
//    ls1x_disable_gpio_interrupt(39);
//    ls1x_install_gpio_isr(39, INT_TRIG_EDGE_UP, gpio_interrput_isr, NULL);
//    ls1x_enable_gpio_interrupt(39);

//    gpio_write(45, 1); // 1除能 0使能
//    gpio_write(51, 0); // dir  0向右 ，1向左
//    gpio_write(50, 1); // EN   1除能，0使能

//    pca_write(pca_mode1, 0x0);
//    set_frequency(hz); // 设置PWM频率
//    set_Duty_cycle(0, 0, angle);
//    set_Duty_cycle(1, 0, angle);
//    set_Duty_cycle(2, 0, angle);
//    set_Duty_cycle(3, 0, angle);
//    set_Duty_cycle(4, 0, angle);
//    set_Duty_cycle(5, 0, angle);
//    set_Duty_cycle(6, 0, angle);
//    set_Duty_cycle(7, 0, angle);
//    set_Duty_cycle(8, 0, angle);
//    set_Duty_cycle(9, 0, angle);
//    set_Duty_cycle(10, 0, angle);
//    set_Duty_cycle(11, 0, angle);
//    set_Duty_cycle(12, 0, angle);
//    set_Duty_cycle(13, 0, angle);
//    set_Duty_cycle(14, 0, angle);
//    set_Duty_cycle(15, 0, angle);
//    set_Duty_cycle(4, 0, angle);
}

void Stepper_motor_open(void)
{
    if (Stepper_mode == 0)
    {
        Stepper_mode = 1;
        if (rt_pin_read(SCL_PIN) != 1)
            rt_pin_write(DIR_PIN, 0); // dir  0向右 ，1向左
        else
            rt_pin_write(DIR_PIN, 1); // dir  0向右 ，1向左
        rt_pin_write(HightV_PIN, 0);     // 1除能 0使能
        rt_pin_write(EN_PIN, 0);     // EN   1除能，0使能
    }
}

void Stepper_motor_reset(void)
{
    if (Stepper_mode == 0)
    {
        if (rt_pin_read(SDA_PIN) != 1)
        {
            rt_pin_write(DIR_PIN, 1); // dir  0向右 ，1向左
            rt_pin_write(EN_PIN, 0); // EN   1除能，0使能
            Stepper_mode = 2;
        }
    }
}


/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-21     lzh28       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "StepperMotor.h"
#include "actuate/task/task_7.h"
static rt_base_t IN1_pin = RT_NULL, IN2_pin = RT_NULL, IN3_pin = RT_NULL, IN4_pin = RT_NULL;


/* 定义全局标志位，默认0表示无中断请求 */
volatile int new_command_flag = 0;

void StepperMotor_init(void)
{
    IN1_pin = rt_pin_get("PB.2");  //输出
    IN2_pin = rt_pin_get("PB.3");  //输出
    IN3_pin = rt_pin_get("PB.4");  //输出
    IN4_pin = rt_pin_get("PB.5");  //输出
    rt_pin_mode(IN1_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(IN2_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(IN3_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(IN4_pin, PIN_MODE_OUTPUT);
    SM_IN1(0);
    SM_IN2(0);
    SM_IN3(0);
    SM_IN4(0);
}
/******
 step：为拍数 0-3对应1-4拍
 dir：方向 0为正方向 1为反方向
 ******/
void step_28byj48_control(unsigned char step, unsigned char dir)  //步进电机驱动函数
{
    unsigned char temp = step;

    if (dir == 1)  // 反方向
        temp = 7 - step;
    switch (temp)
    {
    case 0:
        //A组
            SM_IN1(1);
            SM_IN2(0);
            SM_IN3(0);
            SM_IN4(0);
        break;
    case 1:
        //AC组
        SM_IN1(1);
        SM_IN2(0);
        SM_IN3(1);
        SM_IN4(0);
        break;
    case 2:
        //C组
          SM_IN1(0);
          SM_IN2(0);
          SM_IN3(1);
          SM_IN4(0);
        break;
    case 3:
        //CB组
        SM_IN1(0);
        SM_IN2(1);
        SM_IN3(1);
        SM_IN4(0);
        break;
    case 4:
        //B组
        SM_IN1(0);
        SM_IN2(1);
        SM_IN3(0);
        SM_IN4(0);
        break;
    case 5:
        //BD组
        SM_IN1(0);
        SM_IN2(1);
        SM_IN3(0);
        SM_IN4(1);
        break;
    case 6:
        //D组
        SM_IN1(0);
        SM_IN2(0);
        SM_IN3(0);
        SM_IN4(1);
        break;
    case 7:
        //DA组
        SM_IN1(1);
        SM_IN2(0);
        SM_IN3(0);
        SM_IN4(1);
        break;
    }
}

/******
 angles :角度
 dir：方向 0为正方向 1为反方向
 ******/
void step_28byj48_angles(unsigned int angles, unsigned char dir)
{
    if (IN1_pin == RT_NULL || IN2_pin == RT_NULL || IN3_pin == RT_NULL || IN4_pin == RT_NULL)
    {
        StepperMotor_init();
        if (IN1_pin == RT_NULL || IN2_pin == RT_NULL || IN3_pin == RT_NULL || IN4_pin == RT_NULL)
            return;
    }

    /******
     计算步数
     28BYJ-48 采用8步换相法步进角为 5.625°，每圈需要 64 步，采用4步换相法步进角为 11.25，每圈需要32 步
     meter = angles * 5.68 是一种减少计算量的方法
     目标角度：100°
     计算所需步数的公式：
     步数 = 目标角度 × (360° / (步进角 / 齿比))

     每步角度：11.25°
     齿比：64
     每步的实际转动角度 = 11.25° / 64 = 0.17578125°
     每圈步数 = 360° / 0.17578125° ≈ 2050.08 步

     最终步数 = 100 × (2050.08 / 360) ≈ 569.4 步（(2050.08 / 360)=5.68，简化后:步数 = 目标角度*5.68）
     该值用于控制电机转动到指定角度。
     ******/
    int meter = angles * (36.0f/360.f); // 计算步数，这里需要根据你的电机和齿比进行调整
    int j = 0;
    for (int i = 0; i < meter; i++)
    {
        step_28byj48_control(j, dir);
        j++;
        if (j >= 7)  // 4步换相
            j = 0;
        rt_thread_mdelay(12);  //28BYJ-48电机启动频率>=550hz
    }
    SM_IN1(0);
    SM_IN2(0);
    SM_IN3(0);
    SM_IN4(0);
}



/******
 * angles     : 旋转角度（单位：度）
 * dir        : 方向，0为正方向，1为反方向
 * step_delay : 每步之间的延时（单位：毫秒），延时越短转速越快
 *
 * 此函数在控制旋转角度和方向的同时，通过设置步延时实现对电机速度的控制。
 ******/
void step_28byj48_angles_speed(unsigned int angles, unsigned char dir, unsigned int step_delay)
{
    if (IN1_pin == RT_NULL || IN2_pin == RT_NULL || IN3_pin == RT_NULL || IN4_pin == RT_NULL)
    {
        StepperMotor_init();
        if (IN1_pin == RT_NULL || IN2_pin == RT_NULL || IN3_pin == RT_NULL || IN4_pin == RT_NULL)
            return;
    }

    int steps = (int)(angles * 5.6875f);

    unsigned char j = 0;
    for (int i = 0; i < steps; i++)
    {
        if(new_command_flag == 1)
        {
            rt_kprintf("检测到全局标志位，提前退出步进操作\n");
            break;
        }
        step_28byj48_control(j, dir);
        j++;
        if (j >= 8)
            j = 0;
        rt_thread_mdelay(step_delay);  // 用传入的延时控制转速
    }
    SM_IN1(0);
    SM_IN2(0);
    SM_IN3(0);
    SM_IN4(0);
}


void step_28byj48_stop()
{
    SM_IN1(0);
    SM_IN2(0);
    SM_IN3(0);
    SM_IN4(0);
}




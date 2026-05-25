/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-23     lzh28       the first version
 */
//LED1~4，RGB 高电平开，
#include <rtthread.h>
#include <rtdevice.h>
#include "led.h"
#include <board.h>



//    初始化PWM
//    呼吸灯参数
//   const char *pwm_name = "pwm3"; // PWM 设备名称，根据开发板修改
//   rt_uint32_t channel = 2;       // PWM 通道
//   rt_uint32_t period = 500000;   // PWM 周期（单位：纳秒）
//   rt_uint32_t max_pulse = 250000; // 最大脉冲宽度（单位：纳秒）
//   rt_uint32_t delay_ms = 20;     // 延时（单位：毫秒）


struct rt_pwm_device *pwm_dev;
struct rt_pwm_device *pwm_dev2;
struct rt_pwm_device *pwm_dev3;



rt_base_t LED1_pin = RT_NULL, LED2_pin = RT_NULL, LED3_pin = RT_NULL, LED4_pin = RT_NULL,LED_R_pin = RT_NULL, LED_G_pin = RT_NULL, LED_B_pin = RT_NULL,usrt_LED_pin = RT_NULL;
void LED_RGB_init(void){
    LED1_pin = rt_pin_get("PE.12");  //输出
    LED2_pin = rt_pin_get("PE.13");  //输出
    LED3_pin = rt_pin_get("PE.14");  //输出
    LED4_pin = rt_pin_get("PE.15");  //输出
//    LED_B_pin = rt_pin_get("PB.3");  //输出
//    LED_G_pin = rt_pin_get("PB.4");  //输出
//    LED_R_pin = rt_pin_get("PB.5");  //输出
    LED_B_pin = rt_pin_get("PC.1");  //输出
    LED_G_pin = rt_pin_get("PC.2");  //输出
    LED_R_pin = rt_pin_get("PC.3");  //输出
    usrt_LED_pin = rt_pin_get("PE.12");  //输出
    rt_pin_mode(usrt_LED_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(LED1_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(LED4_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_R_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_G_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_B_pin, PIN_MODE_OUTPUT);
    LED1_reset
    LED2_reset
    LED3_reset
    LED4_reset
    LEDG_reset
    LEDB_reset
    LEDR_reset
    user_LED_set
}

/*******************************************************************
 **函数名：LEDx_Status
 **函数功能：设置单个LED灯的状态
 **形参：
    status  :选择灯和状态
 **返回值：无
 **说明：
 *******************************************************************/
 void LEDx_Set_Status(u8 status)
 {
    /**
     * @brief RGB颜色配置
     * @param 1 颜色
     * @return void
     */
    if (status != OFF)
    {
        if (status == redRGB_ON)
            LEDR_set
        else if (status == redRGB_OFF)
            LEDR_reset
        else if (status == greenRGB_ON)
            LEDG_set
        else if (status == greenRGB_OFF)
            LEDG_reset
        else if (status == blueRGB_ON)
            LEDB_set
        else if (status == blueRGB_OFF)
            LEDB_reset
        else if (status == yellowRGB) //黄
        {
            LEDR_set
            LEDG_set
            LEDB_reset
        }
        else if (status == whiteRGB)  //白
        {
            LEDR_set
            LEDG_set
            LEDB_set
        }
        else if (status == purpleRGB) //紫
        {
            LEDR_set
            LEDG_reset
            LEDB_set
        }
        else if (status == cyanRGB)  // 青
        {
            LEDR_reset
            LEDG_set
            LEDB_set
        }
    }

    else
    {
        LEDR_reset
        LEDG_reset
        LEDB_reset
    }
 }

//设置红灯的亮度
 void pwm_red_init(uint8_t brightness)
 {
     // 查找PWM设备
     pwm_dev = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME);
     if (!pwm_dev)
     {
         rt_kprintf("PWM device not found!\n");
         return;
     }

     rt_uint32_t period = 1000000;  // 周期为1ms
     rt_uint32_t pulse = brightness * (period / 100);  // 占空比 = brightness%

     rt_pwm_set(pwm_dev, PWM_CHANNEL, period, pulse);

     // 启用PWM通道
     rt_pwm_enable(pwm_dev, PWM_CHANNEL);
 }

 //设置绿灯的亮度

 void pwm_green_init(uint8_t brightness2)
 {
     // 查找PWM设备
     pwm_dev2 = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME2);
     if (!pwm_dev2)
     {
         rt_kprintf("PWM device not found!\n");
         return;
     }

     rt_uint32_t period = 1000000;  // 周期为1ms
     rt_uint32_t pulse = brightness2 * (period / 100);  // 占空比 = brightness%

     rt_pwm_set(pwm_dev2, PWM_CHANNEL2, period, pulse);

     // 启用PWM通道
     rt_pwm_enable(pwm_dev2, PWM_CHANNEL2);
 }



 //设置蓝灯的亮度
 void pwm_blue_init(uint8_t brightness3)
 {
     // 查找PWM设备
     pwm_dev3 = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME3);
     if (!pwm_dev3)
     {
         rt_kprintf("PWM device not found!\n");
         return;
     }

     rt_uint32_t period = 1000000;  // 周期为1ms
     rt_uint32_t pulse = brightness3 * (period / 100);  // 占空比 = brightness%

     rt_pwm_set(pwm_dev3, PWM_CHANNEL3, period, pulse);

     // 启用PWM通道
     rt_pwm_enable(pwm_dev3, PWM_CHANNEL3);
 }


void pwm_white_init(uint8_t brightness4)
{
    // 查找PWM设备
    struct rt_pwm_device *pwm_red = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME);
    struct rt_pwm_device *pwm_green = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME2);
    struct rt_pwm_device *pwm_blue = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME3);

    if ( !pwm_red || !pwm_green || !pwm_blue )
    {
        rt_kprintf("PWM device not found!\n");
        return;
    }


    rt_uint32_t period = 1000000;  // 周期为1ms
    rt_uint32_t pulse = brightness4 * (period / 100);  // 占空比 = brightness%


    rt_pwm_set(pwm_red, PWM_CHANNEL, period, pulse);
    rt_pwm_set(pwm_green, PWM_CHANNEL2, period, pulse);
    rt_pwm_set(pwm_blue, PWM_CHANNEL3,  period, pulse);

    rt_pwm_enable(pwm_red, PWM_CHANNEL);
    rt_pwm_enable(pwm_green, PWM_CHANNEL2);
    rt_pwm_enable(pwm_blue, PWM_CHANNEL3);


}

void pwm_white_reset(void)
{
    // 查找PWM设备
    struct rt_pwm_device *pwm_red = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME);
    struct rt_pwm_device *pwm_green = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME2);
    struct rt_pwm_device *pwm_blue = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME3);

    if ( !pwm_red || !pwm_green || !pwm_blue )
    {
        rt_kprintf("PWM device not found!\n");
        return;
    }


    rt_uint32_t period = 1000000;  // 周期为1ms
//    rt_uint32_t pulse = brightness4 * (period / 100);  // 占空比 = brightness%


    rt_pwm_set(pwm_red, PWM_CHANNEL, period, 0);
    rt_pwm_set(pwm_green, PWM_CHANNEL2, period, 0);
    rt_pwm_set(pwm_blue, PWM_CHANNEL3,  period, 0);

    rt_pwm_enable(pwm_red, PWM_CHANNEL);
    rt_pwm_enable(pwm_green, PWM_CHANNEL2);
    rt_pwm_enable(pwm_blue, PWM_CHANNEL3);


}



// 呼吸灯效果函数
//void breath_led(const char *pwm_name, rt_uint32_t channel, rt_uint32_t period, rt_uint32_t max_pulse, rt_uint32_t delay_ms)
//{
//    struct rt_device_pwm *pwm_dev; // PWM 设备句柄
//    rt_uint32_t pulse = 0;         // PWM 脉冲宽度初始值
//    int dir = 1;                   // 脉冲宽度的增减方向
//
//    // 查找 PWM 设备
//    pwm_dev = (struct rt_device_pwm *)rt_device_find(pwm_name);
//    if (pwm_dev == RT_NULL)
//    {
//        rt_kprintf("PWM device not found!\n");
//        return;
//    }
//
//    // 设置 PWM 周期
//    rt_pwm_set(pwm_dev, channel, period, pulse);
//
//    // 使能 PWM 设备
//    rt_pwm_enable(pwm_dev, channel);
//
//
//    while (1)
//    {
//        rt_thread_mdelay(delay_ms); // 延时
//
//        if (dir)
//        {
//            pulse += max_pulse / 100; // 增加脉冲宽度
//        }
//        else
//        {
//            pulse -= max_pulse / 100; // 减少脉冲宽度
//        }
//
//        if (pulse >= max_pulse) // 当脉冲宽度达到最大值时，反转方向
//        {
//            dir = 0;
//        }
//        if (pulse == 0) // 当脉冲宽度为 0 时，反转方向
//        {
//            dir = 1;
//        }
//
//        // 更新 PWM 脉冲宽度
//        rt_pwm_set(pwm_dev, channel, period, pulse);
//
//
//    }
//}



void breath_led(const char *pwm_name, rt_uint32_t channel, rt_uint32_t period, rt_uint32_t max_pulse, rt_uint32_t delay_ms)
{
    struct rt_device_pwm *pwm_dev; // PWM 设备句柄
    rt_uint32_t pulse = 0;         // PWM 脉冲宽度初始值
    int dir = 1;                   // 脉冲宽度的增减方向

    // 查找 PWM 设备
    pwm_dev = (struct rt_device_pwm *)rt_device_find(pwm_name);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("PWM device not found!\n");
        return;
    }

    // 设置 PWM 周期
    rt_pwm_set(pwm_dev, channel, period, pulse);

    // 使能 PWM 设备
    rt_pwm_enable(pwm_dev, channel);


    for(int i = 0; i < 200; i++)
    {
        rt_thread_mdelay(delay_ms); // 延时

        if (dir)
        {
            pulse += max_pulse / 100; // 增加脉冲宽度
        }
        else
        {
            pulse -= max_pulse / 100; // 减少脉冲宽度
        }

        if (pulse >= max_pulse) // 当脉冲宽度达到最大值时，反转方向
        {
            dir = 0;
        }
        if (pulse == 0) // 当脉冲宽度为 0 时，反转方向
        {
            dir = 1;
        }

        // 更新 PWM 脉冲宽度
        rt_pwm_set(pwm_dev, channel, period, pulse);


    }
}


void breath_white_led(rt_uint32_t period,rt_uint32_t max_pulse)
{
    rt_uint32_t pulse = 0;
    int dir = 1;


    struct rt_pwm_device *pwm_red = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME);
    struct rt_pwm_device *pwm_green = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME2);
    struct rt_pwm_device *pwm_blue = (struct rt_pwm_device *)rt_device_find(PWM_DEVICE_NAME3);

    if ( !pwm_red || !pwm_green || !pwm_blue )
    {
        rt_kprintf("PWM device not found!\n");
        return;
    }

    rt_pwm_set(pwm_red, PWM_CHANNEL, period, max_pulse);
    rt_pwm_set(pwm_green, PWM_CHANNEL2, period, max_pulse);
    rt_pwm_set(pwm_blue, PWM_CHANNEL3,  period, max_pulse);
    rt_thread_mdelay(50);
    // 启动 PWM
    rt_pwm_enable(pwm_red, PWM_CHANNEL);
    rt_pwm_enable(pwm_green, PWM_CHANNEL2);
    rt_pwm_enable(pwm_blue, PWM_CHANNEL3);



    while(1)
    {
        rt_thread_mdelay(20);
        if (dir)
        {
            pulse += max_pulse/100;
        }
        else
        {
            pulse -= max_pulse/100;
        }


        if (pulse >= max_pulse)
        {
            dir = 0;
        }

        if (pulse == 0)
        {
            dir = 1;
        }

        rt_pwm_set(pwm_red, PWM_CHANNEL, period, pulse);
        rt_pwm_set(pwm_green, PWM_CHANNEL2, period, pulse);
        rt_pwm_set(pwm_blue, PWM_CHANNEL3, period, pulse);
    }


}





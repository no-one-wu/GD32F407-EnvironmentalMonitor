/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-25     lzh28       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "smg.h"
#include "board.h"
static rt_thread_t SegThread;
rt_base_t SMG1_pin = RT_NULL, SMG2_pin = RT_NULL, HC595_RCK_pin = RT_NULL, HC595_SCK_pin = RT_NULL, HC595_SOE_pin =
RT_NULL, HC595_DATA_pin = RT_NULL;
int SMG[16] = { 0X3F, 0X06, 0X5B, 0X4F, 0X66, 0X6D, 0X7D, 0X07, 0X7F, 0X6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };
char smg_Refresh_data[2] = { 0 };
/* 线程 1 入口 */
//void SegThread_entry(void* parameter);
void Smg_init(void)
{
    /* 创建线程 2 */
    /* 线程的入口是 thread2_entry, 参数是 RT_NULL
     * 栈空间是 512，优先级是 250，时间片是 25 个 OS Tick
     */
//    SegThread = rt_thread_create("SegThread", SegThread_entry, RT_NULL, 512, 20, 1);
//
//    /* 启动线程 */
//    if (SegThread != RT_NULL)
//        rt_thread_startup(SegThread);
    SMG1_pin = rt_pin_get("PA.6");  //输出
    SMG2_pin = rt_pin_get("PA.7");  //输出
    HC595_RCK_pin = rt_pin_get("PC.0");  //输出
    HC595_SCK_pin = rt_pin_get("PC.1");  //输出
    HC595_SOE_pin = rt_pin_get("PC.2");  //输出
    HC595_DATA_pin = rt_pin_get("PC.3");  //输出
    rt_pin_mode(SMG1_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(SMG2_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(HC595_RCK_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(HC595_SCK_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(HC595_SOE_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(HC595_DATA_pin, PIN_MODE_OUTPUT);
    SEG1_SET
    SEG2_SET
    SOE_RESET
}

void LED595SendData(unsigned char OutData)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        if ((OutData << i) & 0x80)
        {
            LED595_DATA_SET
        }
        else
        {
            LED595_DATA_RESET
        }
        LED595_SCK_RESET
        rt_hw_us_delay(2);
        LED595_SCK_SET
    }
    LED595_RCK_RESET
    rt_hw_us_delay(2);
    LED595_RCK_SET
}


void SMG_DRIVE(unsigned char a, unsigned char b)
{
    /**
     * @brief 更改数码管显示
     * @param 1 数据(必须为8位数据)SMG[]
     * @param 2 数码管显示位数
     * @return void
     */
    SEG1_RESET
    SEG2_RESET
    LED595SendData(a);
    switch (b)
    {
    case 1:
        SEG1_SET
        break;
    case 2:
        SEG2_SET
        break;
    }
}

void SMG_data(unsigned char a, unsigned char b, unsigned char c)
{
    /**
     * @brief 更改数码管显示
     * @param 1 数据(必须为8位数据)
     * @param 2 数码管显示位数
     * @param 3 数码管显不显示小数点
     * @return void
     */
    if (a == '-')
        smg_Refresh_data[b - 1] = c ? 0xc0 : 0x40;
    else
        smg_Refresh_data[b - 1] = c ? (SMG[a] | 0x80) : SMG[a];
}
void SMG_OFF(void)
{
    /**
     * @brief 关闭数码管            //但是下次打开数码管还会继续显示上次的数据
     * @param 1 void
     * @return void
     */
    SOE_SET

    rt_thread_mdelay(10);                  //延时不要取消
    SEG1_RESET
    SEG2_RESET
}
void SMG_ON(void)
{
    /**
     * @brief 开启数码管
     * @param 1 void
     * @return void
     */
    SOE_RESET
}
void hc595_Test(unsigned short num)
{
    /**
     * @brief 数码管测试
     * @param 1 void
     * @return void
     */
    if (num > 99)
        num = 0;
    // 选择数据
    smg_Refresh_data[0] = SMG[(int) (num / 10 % 10)];
    // 选择数据
    smg_Refresh_data[1] = SMG[(int) (num % 10)];
}

//void SegThread_entry(void* parameter)
//{
//
//    while (1)
//    {
//        /* 延时 100ms */
//        SMG_DRIVE(smg_Refresh_data[0],1);
//        rt_thread_mdelay(12);
//        SMG_DRIVE(smg_Refresh_data[1],2);
//        rt_thread_mdelay(12);
//    }
//}

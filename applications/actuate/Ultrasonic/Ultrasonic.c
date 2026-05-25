/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-18     lzh28       the first version
 */
#include <rtdevice.h>
#include <rtthread.h>
#include "Ultrasonic.h"
unsigned int Ultrasonic_Num = 0;         // 计数值
float Ultrasonic_Value = 0;
static int dis = 0;
static rt_hwtimerval_t timeout_s; /* 定时器超时值 */
static rt_device_t hw_dev = RT_NULL; /* 定时器设备句柄 */
static rt_base_t pin_INC = RT_NULL;
static rt_base_t pin_INT0 = RT_NULL;

void rt_hw_us_delay(rt_uint32_t us);
/* 定时器超时回调函数 */
static rt_err_t timeout_cb(rt_device_t dev, rt_size_t size)
{
    Ultrasonic_Num++;
    return 0;
}

static void GPIO_IRQHandler(void *args)
{

    rt_device_control(hw_dev, HWTIMER_CTRL_STOP, RT_NULL);  //停止定时器
    Ultrasonic_Value = (float) Ultrasonic_Num;
    Ultrasonic_Value = Ultrasonic_Value * 1.715f -15;       // 计算距离定时10us，S=Vt/2（减47是误差补尝）
    dis =  (int )Ultrasonic_Value; //(int)

}
void ultrasonic_init(void)
{
    rt_err_t ret = RT_EOK;

    rt_hwtimer_mode_t mode; /* 定时器模式 */
    rt_uint32_t freq = 84000000; /* 计数频率 */
    /* 查找定时器设备 */
    hw_dev = rt_device_find(HWTIMER_DEV_NAME);
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("没有找到 %s device!\n", HWTIMER_DEV_NAME);
        return;
    }

    /* 以读写方式打开设备 */
    ret = rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("打开 %s device failed!\n", HWTIMER_DEV_NAME);
        return;
    }

    /* 设置超时回调函数 */
    rt_device_set_rx_indicate(hw_dev, timeout_cb);

//    /* 设置计数频率(若未设置该项，默认为1Mhz 或 支持的最小计数频率) */
    rt_device_control(hw_dev, HWTIMER_CTRL_FREQ_SET, &freq);

    /* 设置模式为周期性定时器（若未设置，默认是HWTIMER_MODE_ONESHOT）*/
    mode = HWTIMER_MODE_PERIOD;
    ret = rt_device_control(hw_dev, HWTIMER_CTRL_MODE_SET, &mode);
    if (ret != RT_EOK)
    {
        hw_dev = RT_NULL;
        rt_kprintf("set mode failed! ret is :%d\n", ret);
        return;
    }

    timeout_s.sec = 0; /* 秒 */
    timeout_s.usec = 10; /* 微秒 */
    if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        hw_dev = RT_NULL;
        rt_kprintf("set timeout value failed\n");
        return;
    }
    rt_device_control(hw_dev, HWTIMER_CTRL_STOP, RT_NULL);  //停止定时器
    //rt_device_close(hw_dev);
    pin_INC = rt_pin_get("PA.11");  //输出
    rt_pin_mode(pin_INC, PIN_MODE_OUTPUT);

    pin_INT0 = rt_pin_get("PA.12");  //输入
    rt_pin_mode(pin_INT0, PIN_MODE_INPUT);
    /* 下降沿触发 */
    rt_pin_attach_irq(pin_INT0, PIN_IRQ_MODE_FALLING, GPIO_IRQHandler, RT_NULL);
    rt_pin_irq_enable(pin_INT0, PIN_IRQ_ENABLE);
    return;
}
int ranging_once(void)
{
    int i;
    if (hw_dev == RT_NULL)
        ultrasonic_init();
    for (i = 0; i < 4; i++)
    {
        rt_pin_write(pin_INC, PIN_HIGH);
        rt_hw_us_delay(12);
        rt_pin_write(pin_INC, PIN_LOW);
        rt_hw_us_delay(12);
    }
    rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s));  //设置定时器超时时间，同时启动定时器
    Ultrasonic_Num = 0;        // 定时器清零
    dis = 0;
    i = 0;

    while(i<100)
    {
        if(dis!=0)
        {

            break;
        }
        rt_thread_mdelay(1);
        i++;
    }


    rt_device_control(hw_dev, HWTIMER_CTRL_STOP, RT_NULL);  //停止定时器
    if (dis >= 2000)
    {
        dis = 2000;
    }
    if (dis <= 0)
    {
        dis = 0;
    }
    return dis;
}

/* 功能：10个数据去掉最大和最小然后求中间三个数的平均值
 参数：distance需要计算的数组
 */
int average(int *distance)       //去最大与最小，8个中位数取平均值
{
    int num;
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10 - i; j++)
        {
            if (distance[j] > distance[j + 1])
            {
                num = distance[j];
                distance[j] = distance[j + 1];
                distance[j + 1] = num;
            }
        }
    }

    num = (distance[2] + distance[3] + distance[4] + distance[5] + distance[6] + distance[7]) / 6;
    return num;
}

int ranging_more(void)   //一般情况下还要额外+25ms
{
    int distance[10] = { 0 };
    for (int i = 0; i < 10; i++)
    {
        rt_thread_delay(200);
        distance[i] = ranging_once();
    }
    /*
    //printf("距离1:%dmm,2:%dmm,3:%dmm,4:%dmm,5:%dmm,6:%dmm,7:%dmm,8:%dmm,9:%dmm,10:%dmm",distance[0],distance[1]\
            ,distance[2],distance[3],distance[4],distance[5],distance[6],distance[7],distance[8],distance[9]);
            */
    return average(distance);       //去最大与最小，3个中位数取平均值
}

/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-20     lzh28       the first version
 */
#include "rtthread.h"
#include "MQ2.h"
#include <rtdevice.h>
#include <math.h>
#define ADC_DEV_NAME        "adc1"      /* ADC 设备名称 */
#define ADC_DEV_CHANNEL     1           /* ADC 通道 */
#define REFER_VOLTAGE       330         /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS        (1 << 12)   /* 转换位数为12位 */

static rt_adc_device_t adc_dev;
static void MQ2_init(void){


    rt_err_t ret = RT_EOK;

    adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
    if (adc_dev == RT_NULL)
    {
        rt_kprintf("adc sample run failed! can't find %s device!\n", ADC_DEV_NAME);
    }
    /* 使能设备 */
    ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);
    if(ret == RT_EOK)
    {
        rt_kprintf("adc sample run success!  find %s device!\n", ADC_DEV_NAME);
    }
}


static int read_mq2(void){
    rt_uint32_t value,vol;
    /* 读取采样值 */
    if(adc_dev==RT_NULL){
        MQ2_init();
        if(adc_dev==RT_NULL)
            return 0;
    }
    value = rt_adc_read(adc_dev, ADC_DEV_CHANNEL);
    //rt_kprintf("the value is :%d \n", value);
    /* 转换为对应电压值 */
    vol = value * REFER_VOLTAGE / CONVERT_BITS;
    //rt_kprintf("the voltage is :%d.%02d \n", vol / 100, vol % 100);
    return vol;
}

/* 功能：10个数据去掉最大和最小然后求中间三个数的平均值
 参数：distance需要计算的数组
 */
static float average(int *distance)       //去最大与最小，8个中位数取平均值
{
    float num;
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
    num = (distance[1]+distance[2] + distance[3] + distance[4] + distance[5] + distance[6] + distance[7]+distance[8]) / 8.0f;
    return num;
}
//在家庭环境中，通常可燃气体如天然气（甲烷）的浓度应低于1000 ppm，最佳是保持在0-100 ppm之间，以确保安全。超过此浓度可能会引起安全隐患。？
float get_gas_real_value()
{
    int distance[10] = { 0 };
    for (int i = 0; i < 10; i++)
    {
        distance[i] = read_mq2();
    }
    float Vol = average(distance)/100;
    Vol*=2;
    float RS = (5-Vol)*20000/Vol;
//    rt_kprintf("RO = %d,电压:%d.%d\n",(int)RS,(int)(average(distance)/100),(int)average(distance)%100);
    float R0 = 17313; //正常环境下1小时后测量出来的电阻
    float ppm = pow(11.5428*R0/RS,0.6549f);
    return ppm;

//    float Vol = average(distance)/100;
//    Vol*=2;
//    float RS = (5-Vol)/(Vol*0.5);
//    float R0 = 6.64;
//    float ppm = pow(11.5428*R0/RS,0.6549f);
//    return ppm;
    //return (average(distance)/100)*210+ 10;//燃气值计算公式可以查看MQ-5数据手册
}

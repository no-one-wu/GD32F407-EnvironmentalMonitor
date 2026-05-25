/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-23     lzh28       the first version
 */
#include "rtthread.h"
#include "Alcohol.h"
#include <rtdevice.h>
#include <math.h>
//#define R0      24329
#define ADC_DEV_NAME        "adc1"      /* ADC 设备名称 */
#define ADC_DEV_CHANNEL     1           /* ADC 通道 */
#define REFER_VOLTAGE       330         /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS        (1 << 12)   /* 转换位数为12位 */

static rt_adc_device_t adc_dev;
static void Alcohol_Init(void){


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


static int read_Alcohol(void){
    rt_uint32_t value,vol;
    /* 读取采样值 */
    if(adc_dev==RT_NULL){
        Alcohol_Init();
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

//大于20就可以认为有酒精了
float get_Alcohol_real_value()
{
    int distance[10] = { 0 };
    for (int i = 0; i < 10; i++)
    {
        distance[i] = read_Alcohol();
    }
    float Vol = average(distance)/100;
    Vol*=2;
    float RS = (5-Vol)*20000/Vol;
    rt_kprintf("RO = %d,电压:%d.%d\n",(int)RS,(int)(average(distance)/100),(int)average(distance)%100);
    float R0 = 20650; //正常环境下1小时后测量出来的电阻
    float ppm = pow(11.5428*R0/RS,0.6549f);
    return ppm;
//    float adcx_temp,vol,Rs,temp;
//    adcx_temp = average(distance);
//
//    //计算电压
//    vol = adcx_temp/100;
//    rt_kprintf("电压%d.%d\n",(int)(adcx_temp/100),(int)((int)vol%100));
//    //电压计算电阻值
//    Rs = 50000/vol-20000;
//    //计算rs/r0比值
//    temp = Rs / R0;
//    //比值与浓度转换
//    temp =  609.34f * pow(temp, -2.148f);
//    temp = vol * 2;
//    if(temp>0.6)
//    {
//            temp =(temp - 0.6) * 200;
//    }
//    else
//    {
//        temp = 0;
//    }
//    return temp;
}

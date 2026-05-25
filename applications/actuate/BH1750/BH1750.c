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
#include "BH1750.h"
#include <rtdevice.h>

unsigned char BUF[2]= {0};
unsigned int result1750=0;
#define bh1750_I2C_BUS_NAME          "i2c2"  /* 传感器连接的I2C总线设备名称 */
static struct rt_i2c_bus_device *i2c1_bus = RT_NULL;
/**************************************************************
*功  能：发送设备地址
*参  数: 无
*返回值: 无
**************************************************************/
void Cmd_Write_BH1750(unsigned char cmd)
{
    struct rt_i2c_msg msgs;
    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    if (i2c1_bus == RT_NULL)
    {
        i2c1_bus = (struct rt_i2c_bus_device *) rt_device_find(bh1750_I2C_BUS_NAME);
        if (i2c1_bus == RT_NULL){
            rt_kprintf("can't find %s device!\n", bh1750_I2C_BUS_NAME);
            return;
        }
    }
    msgs.addr = BH1750_Addr;
    msgs.flags = RT_I2C_WR;
    msgs.buf = &cmd;
    msgs.len = 1;
    /* 调用I2C设备接口传输数据 */
    rt_i2c_transfer(i2c1_bus, &msgs, 1);
}


/**************************************************************
*功  能：开启一次H分辨率模式
*参  数: 无
*返回值: 无
**************************************************************/
void Start_BH1750(void)
{
    Cmd_Write_BH1750(BH1750_ON);            //power on
    Cmd_Write_BH1750(BH1750_RSET);            //clear
    Cmd_Write_BH1750(BH1750_ONE2);          //一次H分辨率模式，至少120ms，之后自动断电模式
}

/**************************************************************
*功  能：读光照信号
*参  数: 无
*返回值: 无
**************************************************************/
void Read_BH1750(void)
{
    struct rt_i2c_msg msgs;
    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    if (i2c1_bus == RT_NULL)
    {
        i2c1_bus = (struct rt_i2c_bus_device *) rt_device_find(bh1750_I2C_BUS_NAME);
        if (i2c1_bus == RT_NULL){
            rt_kprintf("can't find %s device!\n", bh1750_I2C_BUS_NAME);
            return;
        }
    }
    msgs.addr = BH1750_Addr;
    msgs.flags = RT_I2C_RD;
    msgs.buf = BUF;
    msgs.len = 2;
    /* 调用I2C设备接口传输数据 */
    rt_i2c_transfer(i2c1_bus, &msgs, 1);
}

/**************************************************************
*功  能：合成光照数据
*参  数: 无
*返回值: 无
**************************************************************/
void Convert_BH1750(void)
{
    result1750=BUF[0];
    result1750=(result1750<<8)+BUF[1];      //合成数据，即光照数据
    result1750=((float)result1750*0.5)/1.2;
}

/**************************************************************
*功  能：显示光照数据
*参  数: 无
*返回值: 无
**************************************************************/
unsigned short BH1750_Test(void)
{
    Start_BH1750();                     //power on
    rt_thread_mdelay(200);               //延时120ms
    Read_BH1750();                      //连续读取结果 到BUF里面
    Convert_BH1750();                   //转换结果到result_lx
    return result1750; //(最大27306lx)
}

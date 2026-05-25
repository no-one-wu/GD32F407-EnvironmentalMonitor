/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-18     lzh28       the first version
 */

#include "rtthread.h"
#include "CH455.h"
#include <string.h>
#include <rtdevice.h>
#define CH455_I2C_BUS_NAME          "i2c2"  /* 传感器连接的I2C总线设备名称 */
static struct rt_i2c_bus_device *i2c1_bus = RT_NULL;
void CH455_Write( unsigned short cmd )  //写命令
{
    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    if (i2c1_bus == RT_NULL)
    {
        i2c1_bus = (struct rt_i2c_bus_device *) rt_device_find(CH455_I2C_BUS_NAME);
        if (i2c1_bus == RT_NULL){
            rt_kprintf("can't find %s device!\n", CH455_I2C_BUS_NAME);
            return;
        }
    }
    rt_uint8_t buf[2];
    struct rt_i2c_msg msgs[2];
    buf[0] = ((unsigned char)(cmd>>7)&CH455_I2C_MASK)|CH455_I2C_ADDR;
    buf[1] = cmd;
    msgs[0].addr = CH455_I2C_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &buf[0];
    msgs[0].len = 1;

    msgs[1].addr = CH455_I2C_ADDR;
    msgs[1].flags = RT_I2C_WR;
    msgs[1].buf = &buf[1];
    msgs[1].len = 1;
    /* 调用I2C设备接口传输数据 */
    rt_i2c_transfer(i2c1_bus, msgs, 2);
}




unsigned char CH455_Read( void )        //读取按键
{

    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    if (i2c1_bus == RT_NULL)
    {
        i2c1_bus = (struct rt_i2c_bus_device *) rt_device_find(CH455_I2C_BUS_NAME);
        if (i2c1_bus == RT_NULL){
            rt_kprintf("can't find %s device!\n", CH455_I2C_BUS_NAME);
            return 0;
        }
    }
    unsigned char temp_data = (((unsigned char)(CH455_GET_KEY >> 7) & CH455_I2C_MASK) | 0x01 | CH455_I2C_ADDR);
    struct rt_i2c_msg msgs[2];
    msgs[0].addr = CH455_I2C_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &temp_data;
    msgs[0].len = 1;
    msgs[1].addr = CH455_I2C_ADDR;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf = &temp_data;
    msgs[1].len = 1;
    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(i2c1_bus, msgs, 2) == 2)
    {
            return temp_data;
    }
    return 0;
}
//需要循环调用（100ms）

unsigned char Keyboard_proc(void)
{
    unsigned char keyID  = 0;
    unsigned char temp = 0;
    keyID = CH455_Read();
     if(keyID>0x40)
     {
        switch(keyID)
        {
        case 4+0x40:
            temp = 1;
            break;
        case 5+0x40:
            temp = 5;
            break;
        case 6+0x40:
            temp = 9;
            break;
        case 7+0x40:
            temp = 13;
            break;
        case 12+0x40:
            temp = 2;
            break;
        case 13+0x40:
            temp = 6;
            break;
        case 14+0x40:
            temp = 10;
            break;
        case 15+0x40:
            temp = 14;
            break;
        case 20+0x40:
            temp = 3;
            break;
        case 21+0x40:
            temp = 7;
            break;
        case 22+0x40:
            temp = 11;
            break;
        case 23+0x40:
            temp = 15;
            break;
        case 28+0x40:
            temp = 4;
            break;
        case 29+0x40:
            temp = 8;
            break;
        case 30+0x40:
            temp = 12;

            break;
        case 31+0x40:
            temp = 0;   //原本是16
            break;
        }
       while(CH455_Read()>0x40)rt_thread_mdelay(50);
     }
    return temp;
}

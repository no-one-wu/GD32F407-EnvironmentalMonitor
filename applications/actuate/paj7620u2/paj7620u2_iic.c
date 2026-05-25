/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-14     lp666       the first version
 */


#include "paj7620u2_iic.h"
#include "paj7620u2.h"
#include "rtdevice.h"
#include "rtthread.h"

// 产生IIC起始信号
void GS_IIC_Start(void)
{
    GS_SDA_OUT();  // SDA线输出
    GS_IIC_SDA(1);
    GS_IIC_SCL(1);
    rt_hw_us_delay(4);
    GS_IIC_SDA(0);  // START: 当CLK为高时，DATA从高变低
    rt_hw_us_delay(4);
    GS_IIC_SCL(0);  // 钳住I2C总线，准备发送或接收数据
}

// 产生IIC停止信号
void GS_IIC_Stop(void)
{
    GS_SDA_OUT();  // SDA线输出
    GS_IIC_SCL(0);
    GS_IIC_SDA(0);  // STOP: 当CLK为高时，DATA从低变高
    rt_hw_us_delay(4);
    GS_IIC_SCL(1);
    GS_IIC_SDA(1);  // 发送I2C总线结束信号
    rt_hw_us_delay(4);
}

// 等待应答信号到来
// 返回值：1，接收应答失败；0，接收应答成功
unsigned char GS_IIC_Wait_Ack(void)
{
    unsigned char ucErrTime = 0;
    GS_SDA_IN();  // SDA设置为输入
    GS_IIC_SDA(1);  //2
    rt_hw_us_delay(2);
    GS_IIC_SCL(1);
    rt_hw_us_delay(2);
    while (GS_READ_SDA)
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            GS_IIC_Stop();
            return 1;
        }
    }
    GS_IIC_SCL(0);  // 时钟输出0
    return 0;
}

// 产生ACK应答
void GS_IIC_Ack(void)
{
    GS_IIC_SCL(0);
    GS_SDA_OUT();
    GS_IIC_SDA(0);
    rt_hw_us_delay(3);
    GS_IIC_SCL(1);
    rt_hw_us_delay(3);
    GS_IIC_SCL(0);
}

// 不产生ACK应答
void GS_IIC_NAck(void)
{
    GS_IIC_SCL(0);
    GS_SDA_OUT();
    GS_IIC_SDA(1);
    rt_hw_us_delay(2);
    GS_IIC_SCL(1);
    rt_hw_us_delay(2);
    GS_IIC_SCL(0);
}

// IIC发送一个字节
void GS_IIC_Send_Byte(unsigned char txd)
{
    unsigned char t;
    GS_SDA_OUT();
    GS_IIC_SCL(0);  // 拉低时钟开始数据传输
    for (t = 0; t < 8; t++)
    {
        GS_IIC_SDA((txd & 0x80) >> 7);  // 依次发送数据位
        txd <<= 1;
        rt_hw_us_delay(5);  //5
        GS_IIC_SCL(1);
        rt_hw_us_delay(5);
        GS_IIC_SCL(0);
        rt_hw_us_delay(5);
    }
}

// 读1个字节，ack=1时，发送ACK，ack=0，发送nACK
//unsigned char GS_IIC_Read_Byte(unsigned char ack)
//{
    //unsigned char i, receive = 0;
    //GS_SDA_IN();  // SDA设置为输入
    //for (i = 0; i < 8; i++)
    //{
        //GS_IIC_SCL(0);
        //receive <<= 1;
        //GS_IIC_SCL(1);
        //delay_us(4);
        //if (GS_READ_SDA) receive++;
        //delay_us(4);
    //}
    //if (!ack)
        //GS_IIC_NAck();  // 发送nACK
    //else
        //GS_IIC_Ack();  // 发送ACK

    //return receive;
//}


unsigned char GS_IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    GS_SDA_IN();  // SDA设置为输入
    for (i = 0; i < 8; i++)
    {
        GS_IIC_SCL(0);
        rt_hw_us_delay(3);
        GS_IIC_SCL(1);
        receive <<= 1;
        GS_IIC_SCL(1);
        if (GS_READ_SDA) receive++;
        rt_hw_us_delay(1);
    }
    if (!ack)
        GS_IIC_NAck();  // 发送nACK
    else
        GS_IIC_Ack();  // 发送ACK

    return receive;
}


// PAJ7620U2写一个字节数据
unsigned char GS_Write_Byte(unsigned char REG_Address, unsigned char REG_data)
{
    // 关闭中断
    rt_base_t level = rt_hw_interrupt_disable();
    GS_IIC_Start();
    GS_IIC_Send_Byte(PAJ7620_ID);
    if (GS_IIC_Wait_Ack())
    {
        GS_IIC_Stop();  // 释放总线
        rt_hw_interrupt_enable(level);  // 恢复中断
        return 1;  // 没应答则退出
    }
    GS_IIC_Send_Byte(REG_Address);
    GS_IIC_Wait_Ack();
    GS_IIC_Send_Byte(REG_data);
    GS_IIC_Wait_Ack();
    GS_IIC_Stop();
    rt_hw_interrupt_enable(level);  // 恢复中断
    return 0;
}

// PAJ7620U2读一个字节数据
unsigned char GS_Read_Byte(unsigned char REG_Address)
{
    unsigned char REG_data;
    // 关闭中断
    rt_base_t level = rt_hw_interrupt_disable();

    GS_IIC_Start();
    GS_IIC_Send_Byte(PAJ7620_ID);  // 发写命令
    if (GS_IIC_Wait_Ack())
    {
        GS_IIC_Stop();  // 释放总线
        rt_hw_interrupt_enable(level);  // 恢复中断
        return 0;  // 没应答则退出
    }
    GS_IIC_Send_Byte(REG_Address);
    GS_IIC_Wait_Ack();
    GS_IIC_Start();
    GS_IIC_Send_Byte(PAJ7620_ID | 0x01);  // 发读命令
    GS_IIC_Wait_Ack();
    REG_data = GS_IIC_Read_Byte(0);
    GS_IIC_Stop();
    rt_hw_interrupt_enable(level);  // 恢复中断
    return REG_data;
}

// PAJ7620U2读n个字节数据
unsigned char GS_Read_nByte(unsigned char REG_Address, unsigned short len, unsigned char *buf)
{
    // 关闭中断
    rt_base_t level = rt_hw_interrupt_disable();
    GS_IIC_Start();
    GS_IIC_Send_Byte(PAJ7620_ID);  // 发写命令
    if (GS_IIC_Wait_Ack())
    {
        GS_IIC_Stop();  // 释放总线
        rt_hw_interrupt_enable(level);  // 恢复中断
        return 1;  // 没应答则退出
    }
    GS_IIC_Send_Byte(REG_Address);
    GS_IIC_Wait_Ack();

    GS_IIC_Start();
    GS_IIC_Send_Byte(PAJ7620_ID | 0x01);  // 发读命令
    GS_IIC_Wait_Ack();
    while (len)
    {
        if (len == 1)
        {
            *buf = GS_IIC_Read_Byte(0);
        }
        else
        {
            *buf = GS_IIC_Read_Byte(1);
        }
        buf++;
        len--;
    }
    GS_IIC_Stop();  // 释放总线
    rt_hw_interrupt_enable(level);  // 恢复中断
    return 0;
}

// PAJ7620U2唤醒
void GS_WakeUp(void)
{
    // 关闭中断
    rt_base_t level = rt_hw_interrupt_disable();
    GS_IIC_Start();
    GS_IIC_Send_Byte(PAJ7620_ID);  // 发写命令
    GS_IIC_Stop();  // 释放总线
    rt_hw_interrupt_enable(level);  // 恢复中断
}

// PAJ7620 I2C初始化
void GS_i2c_init(void)
{
//    gpio_enable(41, DIR_OUT);
//    gpio_enable(40, DIR_OUT);

    rt_pin_mode(rt_pin_get("PA.11"), PIN_MODE_OUTPUT); //41
    rt_pin_mode(rt_pin_get("PA.12"), PIN_MODE_OUTPUT); //40
}

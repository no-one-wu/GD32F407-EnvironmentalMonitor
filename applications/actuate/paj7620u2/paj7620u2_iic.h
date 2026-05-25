/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-14     lp666       the first version
 */
#ifndef APPLICATIONS_ACTUATE_PAJ7620U2_PAJ7620U2_IIC_H_
#define APPLICATIONS_ACTUATE_PAJ7620U2_PAJ7620U2_IIC_H_



//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32F10x开发板  PAJ7620传感器模块IIC驱动   （模块I2C引脚配置头文件）
//STM32学习有问题，请加入q群交流: 643807576

#include "rtdevice.h"

//GPIO引脚配置（本例程 PF7接模块的SCL脚、PF8接模块的SDA脚）
#define GS_IIC_SCL(IN)    rt_pin_write(rt_pin_get("PA.11"),IN)// 设置为输出模式         //SCL（SCL_OUT）rt_pin_write(rt_pin_get("PA.11"),IN)// 设置为输出模式
#define GS_IIC_SDA(IN)    rt_pin_write(rt_pin_get("PA.12"),IN)// 设置为输出模式         //SDA_OUT，用于发送SDA数据给IIC传感器模块
#define GS_READ_SDA       rt_pin_read(rt_pin_get("PA.12"))         //SDA_IN，用于读取IIC传感器模块的SDA数据

//I/O方向配置(寄存器操作).
#define GS_SDA_IN()  rt_pin_mode(rt_pin_get("PA.12"), PIN_MODE_INPUT);
#define GS_SDA_OUT() rt_pin_mode(rt_pin_get("PA.12"), PIN_MODE_OUTPUT);



//PS:  I/O方向不会配置? :傻瓜式操作 ---> https://xinso.blog.csdn.net/article/details/115862486

unsigned char  GS_Write_Byte(unsigned char  REG_Address,unsigned char  REG_data);
unsigned char  GS_Read_Byte(unsigned char  REG_Address);
unsigned char  GS_Read_nByte(unsigned char  REG_Address,unsigned short len,unsigned char  *buf);
void GS_WakeUp(void);
void GS_i2c_init();


void GS_IIC_Start(void);
void GS_IIC_Stop(void);
unsigned char GS_IIC_Wait_Ack(void);
void GS_IIC_Ack(void);
void GS_IIC_NAck(void);
void GS_IIC_Send_Byte(unsigned char txd);
unsigned char GS_IIC_Read_Byte(unsigned char ack);
#endif /* APPLICATIONS_ACTUATE_PAJ7620U2_PAJ7620U2_IIC_H_ */

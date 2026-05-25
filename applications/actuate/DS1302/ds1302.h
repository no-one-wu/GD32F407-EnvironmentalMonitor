/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-12     lp666       the first version
 */
#ifndef APPLICATIONS_ACTUATE_DS1302_DS1302_H_
#define APPLICATIONS_ACTUATE_DS1302_DS1302_H_

extern unsigned char DS1302_Time[];//声明设置时间的数组
void  DS1302_Init();//声明初始化函数
void  DS1302_WriteBety(unsigned char command,unsigned char Data);//声明单字节写入函数
unsigned char   DS1302_ReadBety(unsigned char command);//声明单字节读出函数
void DS1302_SetTime();//声明设置内部时间函数
void DS1302_ReadTime();//声明读取内部时间函数

#endif /* APPLICATIONS_ACTUATE_DS1302_DS1302_H_ */

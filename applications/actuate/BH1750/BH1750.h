/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-20     lzh28       the first version
 */
#ifndef APPLICATIONS_ACTUATE_BH1750_BH1750_H_
#define APPLICATIONS_ACTUATE_BH1750_BH1750_H_

#define BH1750_Addr 0x23                //0100 011
#define BH1750_ON   0x01
#define BH1750_CON  0x10
#define BH1750_ONE  0x20
#define BH1750_ONE2  0x21
#define BH1750_RSET 0x07

void BH1750_Init(void);                 //IO初始化，
void Start_BH1750(void);                //上电，设置清除数据寄存器
void Read_BH1750(void);                 //连续的读取内部寄存器数据
void Convert_BH1750(void);
unsigned short BH1750_Test(void);

#endif /* APPLICATIONS_ACTUATE_BH1750_BH1750_H_ */

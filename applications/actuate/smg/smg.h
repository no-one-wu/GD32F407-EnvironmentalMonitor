/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-25     lzh28       the first version
 */
#ifndef APPLICATIONS_ACTUATE_SMG_SMG_H_
#define APPLICATIONS_ACTUATE_SMG_SMG_H_
extern rt_base_t SMG1_pin, SMG2_pin, HC595_RCK_pin, HC595_SCK_pin,HC595_SOE_pin, HC595_DATA_pin;
void Smg_init(void);
void LED595SendData(unsigned char OutData);
void hc595_Test(unsigned short num);
void SMG_OFF(void);
void SMG_ON(void);
void SMG_DRIVE(unsigned char a, unsigned char b);
//74HC595口线高低电平宏定义

#define    LED595_DATA_SET      rt_pin_write(HC595_DATA_pin, PIN_HIGH);
#define    LED595_DATA_RESET    rt_pin_write(HC595_DATA_pin, PIN_LOW);

#define    LED595_RCK_SET       rt_pin_write(HC595_RCK_pin, PIN_HIGH);
#define    LED595_RCK_RESET     rt_pin_write(HC595_RCK_pin, PIN_LOW);

#define    LED595_SCK_SET       rt_pin_write(HC595_SCK_pin, PIN_HIGH);
#define    LED595_SCK_RESET     rt_pin_write(HC595_SCK_pin, PIN_LOW);

#define    SOE_SET              rt_pin_write(HC595_SOE_pin, PIN_HIGH);                       //高电平
#define    SOE_RESET            rt_pin_write(HC595_SOE_pin, PIN_LOW);                        //低电平

#define    SEG1_SET             rt_pin_write(SMG1_pin, PIN_HIGH);                       //高电平
#define    SEG1_RESET           rt_pin_write(SMG1_pin, PIN_LOW);                        //低电平

#define    SEG2_SET             rt_pin_write(SMG2_pin, PIN_HIGH);                       //高电平
#define    SEG2_RESET           rt_pin_write(SMG2_pin, PIN_LOW);                        //低电平
extern int SMG[16];
extern char smg_Refresh_data[2];
#endif /* APPLICATIONS_ACTUATE_SMG_SMG_H_ */

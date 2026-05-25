/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-16     lzh28       the first version
 */
#ifndef APPLICATIONS_ACTUATE_HX711_HX711_C_
#define APPLICATIONS_ACTUATE_HX711_HX711_C_

#define  HX711_DOUT     rt_pin_read(HX711_DOUT_pin)//GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)
#define  HX711_SCK(X)   rt_pin_write(HX711_SCK_pin,X)//GPIO_WriteBit(GPIOA, GPIO_Pin_0, ( BitAction)X)

#include "HX711.h"
#include "stdint.h"
#include "rtthread.h"
extern int usart_weight;
extern float GapValue;
void HX711_Init(void);
unsigned long HX711_Read(void);
void Get_Maopi(void);
long Get_Weight(void);

#endif /* APPLICATIONS_ACTUATE_HX711_HX711_C_ */

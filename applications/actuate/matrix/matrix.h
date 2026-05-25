/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-26     lzh28       the first version
 */
#ifndef APPLICATIONS_ACTUATE_MATRIX_MATRIX_H_
#define APPLICATIONS_ACTUATE_MATRIX_MATRIX_H_

//74HC595口线高低电平宏定义

#define    matrix_DATA_SET      rt_pin_write(matrix_DATA_pin, PIN_HIGH);
#define    matrix_DATA_RESET    rt_pin_write(matrix_DATA_pin, PIN_LOW);

#define    matrix_RCK_SET       rt_pin_write(matrix_RCK_pin, PIN_HIGH);
#define    matrix_RCK_RESET     rt_pin_write(matrix_RCK_pin, PIN_LOW);

#define    matrix_SCK_SET       rt_pin_write(matrix_SCK_pin, PIN_HIGH);
#define    matrix_SCK_RESET     rt_pin_write(matrix_SCK_pin, PIN_LOW);

#define    matrix_SOE_SET              rt_pin_write(matrix_SOE_pin, PIN_HIGH);                       //高电平
#define    matrix_SOE_RESET            rt_pin_write(matrix_SOE_pin, PIN_LOW);                        //低电平


void Dis_Char(unsigned char *data);
void matrix_init(void);
void Dis_Char_1(unsigned char *data);
void Dis_Dynamic(unsigned char *data,unsigned int len); //从下往上
void matrix_ON(void);


void Dis_Dynamic_Horiz_Extend(unsigned char (*pattern)[8], unsigned int num_chars);  //从右往左
void Dis_Dynamic_Horiz_L2R(unsigned char (*pattern)[8], unsigned int num_chars); //从左往右
void Dis_Dynamic_Vertical_TopDown(unsigned char (*pattern)[8], unsigned int num_chars);
#endif /* APPLICATIONS_ACTUATE_MATRIX_MATRIX_H_ */

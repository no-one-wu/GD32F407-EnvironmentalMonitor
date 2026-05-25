/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-13     lp666       the first version
 */
#ifndef APPLICATIONS_ACTUATE_AS608_AS608_H_
#define APPLICATIONS_ACTUATE_AS608_AS608_H_

#define C1016_NAME  "uart3"
#define C1016_OUT   "PA.15"

//P18端口 串口
//P13   OUT脚
#define C1016_COLOR_MODE_RESPIRE 1         //呼吸
#define C1016_COLOR_MODE_QUAICT_RESPIRE 2  //快闪
#define C1016_COLOR_MODE_NORMALLY_OPEN  3  //常开
#define C1016_COLOR_MODE_NORMAL_CLOSE 4    //常闭
#define C1016_COLOR_MODE_GRADUALLY_OPEN 5  //渐开
#define C1016_COLOR_MODE_GRADUALLY_CLOSE 6 //渐关
#define C1016_COLOR_MODE_SLOW_RESPIRE 7    //慢闪

#define C1016_COLOR_GREEN 1
#define C1016_COLOR_RED 2
#define C1016_COLOR_BLUE 4

typedef struct
{
    unsigned char color_mode;
    unsigned char color_start_value;
    unsigned char color_end_value;
    unsigned char color_loop;         //0 永久  N呼吸闪烁次数
} C1016_COLOR_Struct;



void C1016_Init(void);
void C1016_SetColor(C1016_COLOR_Struct *p);
uint16_t C1016_Check_id_Registration(uint8_t *rad, int len);

void C1016_Enroll(uint16_t id_number); //录入指纹
void C1016_cmd_Delete(void);  //清空指纹
//void C1016_Identify(void); //识别指纹
uint16_t C1016_Identify(void);//识别指纹
#endif /* APPLICATIONS_ACTUATE_AS608_AS608_H_ */

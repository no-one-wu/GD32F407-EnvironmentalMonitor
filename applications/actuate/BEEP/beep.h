/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-26     lzh28       the first version
 */
#ifndef APPLICATIONS_ACTUATE_BEEP_BEEP_H_
#define APPLICATIONS_ACTUATE_BEEP_BEEP_H_



#include <stdint.h>
void beep_init(void);
void BEEP_Control(rt_bool_t state);
#define TIMER_DEV_NAME  "timer11"  // 定时器设备名称
#define BEEP_OFF rt_pin_write(rt_pin_get("PC.12"), PIN_LOW);
void beep_music(int num,int32_t ms);
void off_beep(void);
#endif /* APPLICATIONS_ACTUATE_BEEP_BEEP_H_ */

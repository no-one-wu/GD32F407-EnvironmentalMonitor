/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-20     lzh28       the first version
 */
#ifndef APPLICATIONS_ACTUATE_BKRC_VOICE_BKRC_VOICE_H_
#define APPLICATIONS_ACTUATE_BKRC_VOICE_BKRC_VOICE_H_

#define VOICE_NAME  "uart3"

//P14端口
void Voice_Config_Init(void);
void voice_broadcast(unsigned char instruct);
char Voice_Drive(void);
void voice_figure( int instruct);
void voice_rouse(void);
void voice_broadcast(unsigned char instruct);
//int Voice_Drive(void);
extern volatile uint8_t voice_rx_flag;
extern unsigned char voice_buff[100];

// 语音设备
extern rt_device_t voice_dev;
#endif /* APPLICATIONS_ACTUATE_BKRC_VOICE_BKRC_VOICE_H_ */

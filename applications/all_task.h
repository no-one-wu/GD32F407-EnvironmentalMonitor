/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-01-19     lp666       the first version
 */


#include "rtthread.h"  // RT-Thread 的核心头文件
#include "rtdevice.h"  // 如果有设备相关操作，可能需要这个
#include "stdio.h"  // 如果有设备相关操作，可能需要这个
#include "SG90.h"
#include "CH455.h"
#include "DHT11.h"

#include <rtdbg.h>
#include "Ultrasonic.h"
#include "rc522.h"
#include "board.h"
#include "BH1750.h"
#include "bkrc_voice.h"
#include "MQ2.h"
#include "MPU6050.h"
#include "inv_mpu.h"

#include "StepperMotor.h"
#include "oled.h"
#include "HX711.h"
#include "led.h"
#include "ir.h"
#include "Alcohol.h"
#include "flame.h"
#include "lock.h"
#include "fan.h"
#include "smg.h"
#include "beep.h"

#include "string.h"
#include "matrix.h"
#include "actuate/ds1302/ds1302.h"
#include "actuate/as608/as608.h"
#include "actuate/paj7620u2/paj7620u2.h"
#include "actuate/HMC5883L/hmc5883l.h"
#include <math.h> // 引入数学库

//#include "N20.h"
#ifndef APPLICATIONS_ALL_TASK_H_
#define APPLICATIONS_ALL_TASK_H_

extern unsigned char Taskshutdown;
extern uint8_t task_buff[12];
extern uint8_t task_6buff[12];
extern uint8_t task_1buff[12];
extern rt_mailbox_t task1_thread_mailbox;
extern rt_device_t lcd_dev;
extern uint8_t lora;
//extern void send_to_nextion(rt_device_t lcd_dev, const char* text);
extern void send_to_nextion_all(rt_device_t lcd_dev, const char* text,const char* prefix);
void send_to_nextion_0(rt_device_t lcd_dev,const char* prefix);
#endif /* APPLICATIONS_ALL_TASK_H_ */

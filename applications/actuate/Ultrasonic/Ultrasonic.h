/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-18     lzh28       the first version
 */
#ifndef APPLICATIONS_ACTUATE_ULTRASONIC_ULTRASONIC_H_
#define APPLICATIONS_ACTUATE_ULTRASONIC_ULTRASONIC_H_

#define HWTIMER_DEV_NAME   "timer13"     /* 定时器名称 */
int ranging_once(void);
int ranging_more(void);
int average(int *distance);       //去最大与最小，8个中位数取平均值

#endif /* APPLICATIONS_ACTUATE_ULTRASONIC_ULTRASONIC_H_ */

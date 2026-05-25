/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-01-16     lp666       the first version
 */
#ifndef APPLICATIONS_ACTUATE_TASK_TASK_1_H_
#define APPLICATIONS_ACTUATE_TASK_TASK_1_H_
void task_1(void);
void send_to_nextion_n_n(rt_device_t lcd_dev,const char* prefix,const char* suffix, const char *format, ...);
void pause_task(void);
#endif /* APPLICATIONS_ACTUATE_TASK_TASK_1_H_ */

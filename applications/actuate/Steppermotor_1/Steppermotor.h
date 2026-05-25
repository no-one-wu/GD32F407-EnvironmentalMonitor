/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-07-14     Administrator       the first version
 */
#ifndef APPLICATIONS_ACTUATE_STEPPER_MOTOR_1_STEPPER_MOTOR_H_
#define APPLICATIONS_ACTUATE_STEPPER_MOTOR_1_STEPPER_MOTOR_H_

void set_frequency(float freq);//设置PWM频率
void Stepper_motor_Init(float hz, int angle);
void Stepper_motor_open(void);
void Stepper_motor_reset(void);

#endif /* APPLICATIONS_ACTUATE_STEPPER_MOTOR_1_STEPPER_MOTOR_H_ */

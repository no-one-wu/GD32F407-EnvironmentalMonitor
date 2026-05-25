/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-21     lzh28       the first version
 */
#ifndef APPLICATIONS_ACTUATE_STEPPERMOTOR_STEPPERMOTOR_H_
#define APPLICATIONS_ACTUATE_STEPPERMOTOR_STEPPERMOTOR_H_
void StepperMotor_init(void);
void step_28byj48_control(unsigned char  step,unsigned char  dir);
void step_28byj48_angles(unsigned int  angles,unsigned char  dir);
#define SM_IN1(IN) rt_pin_write(IN1_pin, IN)
#define SM_IN2(IN) rt_pin_write(IN2_pin, IN)
#define SM_IN3(IN) rt_pin_write(IN3_pin, IN)
#define SM_IN4(IN) rt_pin_write(IN4_pin, IN)
//void step_28byj48_run(unsigned int speed, unsigned char dir);
void step_28byj48_stop();
void step_28byj48_angles_speed(unsigned int angles, unsigned char dir, unsigned int step_delay);


/* 全局标志位，volatile修饰确保多线程环境下变量正确更新 */
extern volatile int new_command_flag;
#endif /* APPLICATIONS_ACTUATE_STEPPERMOTOR_STEPPERMOTOR_H_ */

/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-23     lzh28       the first version
 */
#ifndef APPLICATIONS_ACTUATE_LED_LED_H_
#define APPLICATIONS_ACTUATE_LED_LED_H_
extern rt_base_t LED1_pin, LED2_pin, LED3_pin, LED4_pin,LED_R_pin, LED_G_pin, LED_B_pin,usrt_LED_pin;
#define LED1_set  rt_pin_write(LED1_pin, PIN_HIGH);
#define LED2_set  rt_pin_write(LED2_pin, PIN_HIGH);
#define LED3_set  rt_pin_write(LED3_pin, PIN_HIGH);
#define LED4_set  rt_pin_write(LED4_pin, PIN_HIGH);
#define LEDB_set  rt_pin_write(LED_B_pin, PIN_HIGH);
#define LEDG_set  rt_pin_write(LED_G_pin, PIN_HIGH);
#define LEDR_set  rt_pin_write(LED_R_pin, PIN_HIGH);
#define user_LED_set  rt_pin_write(usrt_LED_pin, PIN_LOW);
#define LED1_reset  rt_pin_write(LED1_pin, PIN_LOW);
#define LED2_reset  rt_pin_write(LED2_pin, PIN_LOW);
#define LED3_reset  rt_pin_write(LED3_pin, PIN_LOW);
#define LED4_reset  rt_pin_write(LED4_pin, PIN_LOW);
#define LEDB_reset  rt_pin_write(LED_B_pin, PIN_LOW);
#define LEDG_reset  rt_pin_write(LED_G_pin, PIN_LOW);
#define LEDR_reset  rt_pin_write(LED_R_pin, PIN_LOW);
#define user_LED_reset  rt_pin_write(usrt_LED_pin, PIN_HIGH);

#define OFF 0

#define redRGB_ON 0x11   // 红色
#define redRGB_OFF 0X12



#define blueRGB_ON 0x21  // 蓝色
#define blueRGB_OFF 0X22

#define greenRGB_ON 0x31 // 绿色
#define greenRGB_OFF 0x32 // 绿色

#define yellowRGB 0x41    // 黄色

#define whiteRGB 0x42    // 白色

#define purpleRGB 0x43   // 紫色

#define cyanRGB 0X44     //青色

#define PWM_DEVICE_NAME        "pwm3"  //红灯
#define PWM_CHANNEL     2

#define PWM_DEVICE_NAME2        "pwm3"  //绿灯
#define PWM_CHANNEL2     1

#define PWM_DEVICE_NAME3        "pwm2"  //蓝灯
#define PWM_CHANNEL3     2




void pwm_white_reset(void);
void breath_white_led(rt_uint32_t period,rt_uint32_t max_pulse);
void pwm_white_init(uint8_t brightness4);
void LED_RGB_init(void);
void LEDx_Set_Status(u8 status);
void pwm_red_init(uint8_t brightness);
void pwm_green_init(uint8_t brightness2);
void pwm_blue_init(uint8_t brightness3);
void pwm_bei_init(uint8_t brightness2);
void breath_led(const char *pwm_name, rt_uint32_t channel, rt_uint32_t period, rt_uint32_t max_pulse, rt_uint32_t delay_ms);
#endif /* APPLICATIONS_ACTUATE_LED_LED_H_ */

/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-19     lzh28       the first version
 */
#ifndef APPLICATIONS_ACTUATE_DHT11_DHT11_H_
#define APPLICATIONS_ACTUATE_DHT11_DHT11_H_

#define DH11_GPIO_Init_OUT() rt_pin_mode(DHT11_OUT_pin, PIN_MODE_OUTPUT)
#define DH11_GPIO_Init_IN()  rt_pin_mode(DHT11_OUT_pin, PIN_MODE_INPUT)
#define Read_Data rt_pin_read(DHT11_OUT_pin)
#define dht11_high rt_pin_write(DHT11_OUT_pin,PIN_HIGH)
#define dht11_low  rt_pin_write(DHT11_OUT_pin,PIN_LOW)
char DHT11_REC_Data(unsigned char *temp, unsigned char *humi);
extern unsigned int rec_data[4];

#endif /* APPLICATIONS_ACTUATE_DHT11_DHT11_H_ */

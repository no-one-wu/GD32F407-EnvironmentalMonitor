/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-19     lzh28       the first version
 */
#include "DHT11.h"
#include "rtthread.h"
#include "board.h"

static rt_base_t DHT11_OUT_pin = RT_NULL;
//数据
unsigned int rec_data[4];
//主机发送开始信号
void DHT11_Start(void)
{
    DH11_GPIO_Init_OUT(); //输出模式

    dht11_low; //拉低电平至少18ms
    rt_thread_mdelay(20);

    dht11_high; //拉高电平20~40us
    rt_hw_us_delay(30);

}

//获取一个字节
char DHT11_Rec_Byte(void)
{
    unsigned char i = 0;
    unsigned char data;
    for (i = 0; i < 8; i++) //1个数据就是1个字节byte，1个字节byte有8位bit
    {
        int j = 0;
        while (Read_Data == 1)
        {
            j++;
            rt_hw_us_delay(1);
            if (j >= 100)
                return 0;
        }
        j = 0;
        while (Read_Data == 0)
        {
            j++;
            rt_hw_us_delay(1);
            if (j >= 100)
                return 0;
        } //高电平变低电平，等待高电平结束

        rt_hw_us_delay(40); //延迟30us是为了区别数据0和数据1，0只有26~28us

        data <<= 1; //左移
        if (Read_Data == 1) //如果过了40us还是高电平的话就是数据1
        {
            data |= 1; //数据+1
        }
        j = 0;
    }

    return data;
}

//获取数据

char DHT11_REC_Data(uint8_t *temp, uint8_t *humi)
{
    if (DHT11_OUT_pin == RT_NULL)
    {
        DHT11_OUT_pin = rt_pin_get("PC.7");
        if (DHT11_OUT_pin == RT_NULL)
            return 0 ;
    }
    unsigned int R_H, R_L, T_H, T_L;
    unsigned char RH, TH, CHECK;
    int i = 0;
    DHT11_Start(); //主机发送复位
    DH11_GPIO_Init_IN(); //输入模式

    while (Read_Data == 1)//高电平变低电平，等待高电平结束
    {
        i++;
        rt_hw_us_delay(1);
        if (i >= 100)
            return 0;
    }
    i = 0;
    while (Read_Data == 0)//低电平变高电平，等待低电平结束
    {
        i++;
        rt_hw_us_delay(1);
        if (i >= 100)
            return 0;
    }


    R_H = DHT11_Rec_Byte();
    R_L = DHT11_Rec_Byte();
    T_H = DHT11_Rec_Byte();
    T_L = DHT11_Rec_Byte();
    CHECK = DHT11_Rec_Byte(); //接收5个数据
    if (R_H + R_L + T_H + T_L == CHECK) //和检验位对比，判断校验接收到的数据是否正确
    {
        RH = R_H;
        TH = T_H;
        *temp  = TH;
        *humi  = RH;
        return 1;
    }
    return 0;
}

//static void dht11_reset(rt_base_t pin)
//{
//    rt_pin_mode(pin, PIN_MODE_OUTPUT);
//
//    rt_pin_write(pin, PIN_LOW);
//    rt_thread_mdelay(20); /* 20ms */
//
//    rt_pin_write(pin, PIN_HIGH);
//    rt_hw_us_delay(30); /* 30us*/
//}
//
//static uint8_t dht11_check(rt_base_t pin)
//{
//    uint8_t retry = 0;
//    rt_pin_mode(pin, PIN_MODE_INPUT);
//
//    while (rt_pin_read(pin) && retry < 100)
//    {
//        retry++;
//        rt_hw_us_delay(1);
//    }
//
//    if (retry >= 100)
//    {
//        return CONNECT_FAILED;
//    }
//
//    retry = 0;
//    while (!rt_pin_read(pin) && retry < 100)
//    {
//        retry++;
//        rt_hw_us_delay(1);
//    };
//
//    if (retry >= 100)
//    {
//        return CONNECT_FAILED;
//    }
//
//    return CONNECT_SUCCESS;
//}
//
//static uint8_t dht11_read_bit(rt_base_t pin)
//{
//    uint8_t retry = 0;
//    while (rt_pin_read(pin) && retry < 100)
//    {
//        retry++;
//        rt_hw_us_delay(1);
//    }
//    retry = 0;
//
//    while (!rt_pin_read(pin) && retry < 100)
//    {
//        retry++;
//        rt_hw_us_delay(1);
//    }
//
//    rt_hw_us_delay(40);
//    if (rt_pin_read(pin))
//        return 1;
//    return 0;
//}
//
//static uint8_t dht11_read_byte(rt_base_t pin)
//{
//    uint8_t i, dat = 0;
//
//    for (i = 1; i <= 8; i++)
//    {
//        dat <<= 1;
//        dat |= dht11_read_bit(pin);
//    }
//
//    return dat;
//}
//
//static uint8_t dht11_read_Data(rt_base_t pin, uint8_t *temp, uint8_t *humi)
//{
//    uint8_t i, buf[5];
//    dht11_reset(pin);
//
//    if (dht11_check(pin) == 0)
//    {
//        for (i = 0; i < 5; i++) /* read 40 bits */
//        {
//            buf[i] = dht11_read_byte(pin);
//        }
//
//        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
//        {
//            *humi = buf[0];
//            *temp = buf[2];
//        }
//    }
//    else
//    {
//        return 1;
//    }
//
//    return 0;
//}

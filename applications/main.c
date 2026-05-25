/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-18     RT-Thread    first version
  * 硬件接线：光照度传 感器需连接“开发底板”的P19端口----------------BH1750(该引脚换了oled屏的IIC可自换)
 * 硬件接线：称重传感器需连接“开发底板”的P17端口------------HX711
 * 硬件接线：13.56M RFID传感   器需连接“开发底板”的P20端口---------rc522
 * 硬件接线：温湿度传感器需连接“开发底板”的P10端口------DHT11
 * 硬件接线：姿态传感器需连接“开发底板”的P19端口 ----有时接触不良需重新插拔----------MPU6050
 *? 硬件接线：烟雾传感器需连接“开发底板”的P5(或P6)端口-------MQ2
 * 硬件接线：超声波传感器需连接“开发底板”的P16端口-------Ultrasonic
 *硬件接线：舵机控制模块需连接“开发底板”的P6端口-----SG90
 * 硬件接线：矩阵键盘需连接“开发底板”的P19端口-----------------CH455
 * 硬件接线：步进电机模块需连接“开发底板”的P24端口-------------------StepperMotor
 * 硬件接线：RGB LED灯模块需连接“开发底板”的P25端口-------pwm用P24端口----LED
 * 硬件接线：红外热释电传感器需连接“开发底板”的P9端口-------------------IR
 * ？硬件接线：酒精传感器需连接“开发底板”的P5端口------Alcohol
 * ？硬件接线：火焰传感器需连接“开发底板”的P9端口------------------flame
 * 硬件接线：智能门锁控制需连接“开发底板”的P7端口-----lock
 * 硬件接线：电动风扇控制需连接“开发底板”的P6端口--------------fan
 * 硬件接线：数码管显示模块需连接“开发底板”的P25，P21端口--------smg
 *  硬件接线：智能语音模块需连接“开发底板”的P18端口----------------------------bkrc_voice
 * 硬件接线：DS1302模块(年月日)需连接“开发底板”的P25端口的5v 01 02 03 GND--------------DS1302
 * 硬件接线：指纹模块需连接“开发底板”的P18，P13端口-------识别不到擦拭手指和模块-----------------------------as608
 * ？ 硬件接线：手势模块需连接“开发底板”的P22端口------------------------------paj760u2
 *  硬件接线：电子罗盘模块需连接“开发底板”的P24端口------------hmc5883
*  硬件接线：点阵模块需连接“开发底板”的P25端口--------matrix
*  硬件连接：触摸传感器连接“开发底板”的P6端口--------------------touch
*  硬件接线：串口液晶显示屏LCD需连接“开发底板”的P14端口
 */
#include <rtdevice.h>
#include <rtthread.h>
#include "SG90.h"
#include "CH455.h"
#include "DHT11.h"
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include "Ultrasonic.h"
#include "ATK_MV1268D.h"
#include "rc522.h"
#include "board.h"
#include "BH1750.h"
#include "bkrc_voice.h"
#include "MQ2.h"
#include "MPU6050.h"
#include "inv_mpu.h"
#include <stdio.h>
#include "StepperMotor.h"
#include "Steppermotor.h"
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
#include "all_task.h"
#include "string.h"
#include "stdio.h"
#include "actuate/ds1302/ds1302.h"
#include "actuate/as608/as608.h"
#include "actuate/paj7620u2/paj7620u2.h"
#include "actuate/HMC5883L/hmc5883l.h"
#include <math.h> // 引入数学库
#include "actuate/task/task_1.h"
#include "actuate/task/task_2.h"
#include "actuate/task/task_3.h"
#include "actuate/task/task_4.h"
#include "actuate/task/task_5.h"
#include "actuate/task/task_6.h"
#include "actuate/task/task_7.h"
#include "actuate/esp-12f/wifi.h"
//#include "N20.h"
// 全局变量，保存退出标志
unsigned char Taskshutdown = 0;
uint8_t task_buff[12];
uint8_t task_6buff[12];
uint8_t task_1buff[12];
static rt_thread_t  task1_thread = NULL,thread1_ptr = RT_NULL,thread1_wifi = RT_NULL; // 线程控制句柄 //, task2_thread = NULL

rt_mailbox_t task1_thread_mailbox;
//static rt_sem_t rx_data_sem; // 声明信号量
rt_device_t lcd_dev = RT_NULL;
uint8_t BEEP_ONY = 0;
int ligh = 0;
uint8_t lora = 0;
short gx = 0,gy = 0,gz = 0;
int turn = 0;
char drivers_id = 2;
extern void Stepper_motor_Init(float hz, int angle);
extern void Stepper_motor_open(void);
extern void Stepper_motor_reset(void);

void send_to_nextion_all(rt_device_t lcd_dev, const char* text,const char* prefix)
{


//= "page0.t2.txt=\"";
    const char* suffix = "\"";


    size_t prefix_len = strlen(prefix);
    size_t text_len   = strlen(text);
    size_t suffix_len = strlen(suffix);


    size_t total_len = prefix_len + text_len + suffix_len + 3 /* 0xFF */ + 1 /* '\0' */;


    rt_uint8_t* buffer = (rt_uint8_t*)rt_malloc(total_len);
    if (!buffer)
    {
        rt_kprintf("malloc failed!\n");
        return;
    }
    memset(buffer, 0, total_len);  // 清零


    memcpy(buffer, prefix, prefix_len);

    memcpy(buffer + prefix_len, text, text_len);

    memcpy(buffer + prefix_len + text_len, suffix, suffix_len);


    buffer[prefix_len + text_len + suffix_len + 0] = 0xFF;
    buffer[prefix_len + text_len + suffix_len + 1] = 0xFF;
    buffer[prefix_len + text_len + suffix_len + 2] = 0xFF;


    buffer[prefix_len + text_len + suffix_len + 3] = '\0';


    size_t write_len = prefix_len + text_len + suffix_len + 3;  // 不带末尾 '\0'
    rt_device_write(lcd_dev, 0, buffer, write_len);


    rt_kprintf("Send to Nextion(含0xFFx3): %s\n", buffer);

    rt_free(buffer);
}
void send_to_nextion_0(rt_device_t lcd_dev,const char* prefix)
{
    size_t prefix_len = strlen(prefix);

    size_t total_len = prefix_len + 3 /* 0xFF */ + 1 /* '\0' */;


    rt_uint8_t* buffer = (rt_uint8_t*)rt_malloc(total_len);
    if (!buffer)
    {
        rt_kprintf("malloc failed!\n");
        return;
    }
    memset(buffer, 0, total_len);  // 清零


    memcpy(buffer, prefix, prefix_len);

    buffer[prefix_len + 0] = 0xFF;
    buffer[prefix_len + 1] = 0xFF;
    buffer[prefix_len + 2] = 0xFF;


    buffer[prefix_len + 3] = '\0';


    size_t write_len = prefix_len + 3;  // 不带末尾 '\0'
    rt_device_write(lcd_dev, 0, buffer, write_len);


    rt_kprintf("Send to Nextion(含0xFFx3): %s\n", buffer);

    rt_free(buffer);
}

void handle_task_switch(unsigned char task_id)
{
    switch (task_id)
    {
        case 1:

            task_1();
            break;
        case 2:

            task_2();
            break;
        case 3:

            task_3();
            break;
        case 4:

            task_4();
            break;
        case 5:

            task_5();
            break;
        case 6:

            task_6();
            break;
        case 7:

            task_7();
            break;
        default:
            Taskshutdown = 0;
            break;
    }
}

void error_work(void)
{
    send_to_nextion_0(lcd_dev,"page 0");
    return ;
}


rt_err_t lcd_rx_handler(rt_device_t dev, rt_size_t size)
{
    static uint8_t buffer[32];
    static int16_t buf_index = 0;
    rt_device_read(dev, 0, &buffer[buf_index], 1);

    if (buffer[buf_index] == 0xBB)
    {
        rt_kprintf("%#x,%#x,%#x,%#x,%#x,%#x,%#x,%#x\n", buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);
        buffer[buf_index] = '\0';
//        rt_kprintf("串口数据:%s\n", buffer);
        buf_index = 0;

        if (buffer[0] == 0x31 && buffer[2] == 0x6e)
        {
            Stepper_motor_open();
//            handle_task_switch(1);
        }
        else if (buffer[0] == 0x31 && buffer[2] == 0x66)
        {
            Stepper_motor_reset();
//            Taskshutdown = 0;
//            pause_task();
        }
        else if (buffer[0] == 0x32 && buffer[2] == 0x6e)
        {
            rt_kprintf("buffer[0] = %#x\n",buffer[0]);
            rt_kprintf("buffer[2] = %#x\n",buffer[2]);
            handle_task_switch(2);
        }
        else if (buffer[0] == 0x32 && buffer[2] == 0x66)
        {
            Taskshutdown = 0;
        }
        else if (buffer[0] == 0x33 && buffer[2] == 0x6e)
        {
            handle_task_switch(3);
        }
        else if (buffer[0] == 0x33 && buffer[2] == 0x66)
        {
            Taskshutdown = 0;
        }
        else if(buffer[0] == 0x34 && buffer[2] == 0x6e)
        {
            handle_task_switch(4);
        }
        else if(buffer[0] == 0x34 && buffer[2] == 0x66)
        {
            Taskshutdown = 0;
        }

        else if(buffer[0] == 0x35 && buffer[2] == 0x6e)
        {
            handle_task_switch(5);
        }
        else if(buffer[0] == 0x35 && buffer[2] == 0x66)
        {
            Taskshutdown = 0;
        }
        else if(buffer[0] == 0x36 && buffer[2] == 0x6e)
        {
            handle_task_switch(6);
        }
        else if(buffer[0] == 0x36 && buffer[2] == 0x66)
        {
            Taskshutdown = 0;
        }
//        else if(buffer[0] == 0x36 && buffer[1] <= 101)
//        {
//            task_6buff[0] = buffer[1];
//            rt_kprintf("task_6buff[0] = %#x\n",task_6buff[0]);
//            task_6buff[1] = '\0';
//        }
        else if(buffer[0] == 0x37 && buffer[2] == 0x6e)
        {
            handle_task_switch(7);
        }

//        else if(buffer[0] == 0x37 && buffer[1] <= 0x39)
//        {
//            task_buff[0] = buffer[1];
//            rt_kprintf("task_buff[0] = %#x\n",task_buff[0]);
//            task_buff[1] = '\0';
//
//        }
        // Add similar cases for other tasks...
        else
        {
            rt_kprintf("串口数据错误!\n");
            Taskshutdown = 0;
            rt_mb_send(task1_thread_mailbox,(rt_ubase_t)error_work);
        }
    }
    else
    {
        buf_index++;
    }
    return RT_EOK;
}

void usart_2init(void)
{
    lcd_dev = rt_device_find("uart2");
    if (lcd_dev == RT_NULL)
    {
        rt_kprintf("未找到设备\r\n");
        return;
    }
    /* step2：修改串口配置参数 */
    static struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; /* 初始化配置参数 */
    /* step2：修改串口配置参数 */
    config.baud_rate = BAUD_RATE_115200;        //修改波特率为115200
    config.data_bits = DATA_BITS_8;             //数据位 8
    config.stop_bits = STOP_BITS_1;             //停止位 1
    config.bufsz = 128;                           //修改缓冲区 buff size 为 128
    config.parity = PARITY_NONE;                //无奇偶校验位

    /* step3：控制串口设备。通过控制接口传入命令控制字，与控制参数 */
    if (rt_device_control(lcd_dev, RT_DEVICE_CTRL_CONFIG, &config) != RT_EOK)
    {
        lcd_dev = RT_NULL;
        rt_kprintf("串口2配置失败");
        return;
    }
    /* step4：打开串口设备。以中断接收及轮询发送模式打开串口设备 */
    if (rt_device_open(lcd_dev, RT_DEVICE_OFLAG_RDWR |RT_DEVICE_FLAG_INT_TX|RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        lcd_dev = RT_NULL;
        rt_kprintf("串口2打开失败");
        return;
    }


}




static void task1_thread_entry(void *arg)
{
    task1_thread_mailbox = rt_mb_create("task1_thread_mb", 1, RT_IPC_FLAG_FIFO);
    void (*p)(void);
    for (;;)
    {
        if (rt_mb_recv(task1_thread_mailbox, &p, RT_WAITING_FOREVER) == RT_EOK)
        {
            p();
        }

    }
}


//static void task2_thread_entry(void *arg)
//{
//    task2_thread_mailbox = rt_mb_create("task1_thread_mb", 1, RT_IPC_FLAG_FIFO);
//    void (*p)(void);
//    for (;;)
//    {
//        if (rt_mb_recv(task2_thread_mailbox, &p, RT_WAITING_FOREVER) == RT_EOK)
//        {
//            p();
//        }
//    }
//}





uint8_t mm[] = {0x00,0x00,0x00,0x00,0x10,0x20,0x10,0x20,0xF8,0x3F,0x00,0x20,0x00,0x20,0x00,0x00};
uint8_t fexn = 0;


int main(void)
{

    // usart_2init();


    // thread1_ptr = rt_thread_create("MV1268D", uart_thread_entry, RT_NULL, 1024 * 5, 15, 1);


    // if(thread1_ptr == RT_NULL)
    // {
    //     rt_kprintf("thread1_ptr启动失败");
    // }
    // else {
    //     rt_thread_startup(thread1_ptr);
    // }

     thread1_wifi = rt_thread_create("wifi_thread", esp12f_thread_entry, RT_NULL, 1024 * 5, 15, 1);


    if(thread1_wifi == RT_NULL)
    {
        rt_kprintf("thread1_wifi启动失败");
    }
    else {
        rt_thread_startup(thread1_wifi);
    }

//    task1_thread = rt_thread_create("task1_thread",
//                                    task1_thread_entry,
//                                    RT_NULL,      // arg
//                                    1024 * 5, // statck size
//                                    20,        // priority
//                                    1);        // slice ticks
//    task2_thread = rt_thread_create("task2_thread",
//                                    task2_thread_entry,
//                                    NULL,      // arg
//                                    1024 * 10, // statck size
//                                    15,        // priority
//                                    1);        // slice ticks



//    if(task1_thread == RT_NULL)
//    {
//        rt_kprintf("task1_thread启动失败");
//    }
//    else {
//        rt_thread_startup(task1_thread);
//    }


//    rt_device_set_rx_indicate(lcd_dev, lcd_rx_handler);



    return RT_EOK;
}





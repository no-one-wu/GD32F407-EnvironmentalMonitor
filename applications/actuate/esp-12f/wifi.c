/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-03-29     wcj    the first version for ESP-12F UART transmission
 */
#include <rtdevice.h>
#include <rtthread.h>
#include <rtdbg.h>
#include "string.h"
#include <stdio.h>
#include <ctype.h>
#include "BH1750.h"
#include "DHT11.h"
#include "flame.h"
#include "MQ2.h"
#include "beep.h"

rt_mq_t esp12f_mqt = RT_NULL;
static rt_sem_t esp12f_sem = RT_NULL;
void esp12f_thread_entry(void *parameter);
static rt_err_t esp12f_rx_ind(rt_device_t dev, rt_size_t size);
static char esp12f_tx_data[13] = { 0xAA, 0x55, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // 帧头(2) + 长度(1) + payload(9) + 校验(1)
rt_device_t esp12f_dev;
rt_bool_t esp_flame = RT_FALSE;
uint8_t esp_temp = 0, esp_humi = 0;
int esp_light = 0;
int esp_gas_value = 0;

void esp12f_init(void)
{
    /* step1：查找 UART 设备 (假设 ESP-12F 连接到 uart1) */
    esp12f_dev = rt_device_find("uart2");
    if (esp12f_dev == RT_NULL)
    {
        rt_kprintf("没有找到 UART2 设备\n");
        return;
    }

    /* step2：修改 UART 配置参数 */
    static struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate = BAUD_RATE_115200;        // 波特率 115200
    config.data_bits = DATA_BITS_8;             // 数据位 8
    config.stop_bits = STOP_BITS_1;             // 停止位 1
    config.bufsz = 256;                         // 缓冲区大小 256
    config.parity = PARITY_NONE;                // 无奇偶校验

    /* step3：控制 UART 设备 */
    if (rt_device_control(esp12f_dev, RT_DEVICE_CTRL_CONFIG, &config) != RT_EOK)
    {
        esp12f_dev = RT_NULL;
        rt_kprintf("UART2 配置失败\n");
        return;
    }

    /* step4：打开 UART 设备。以中断接收及轮询发送模式打开 */
    if (rt_device_open(esp12f_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        esp12f_dev = RT_NULL;
        rt_kprintf("UART2 打开失败\n");
        return;
    }

    /* step5：设置接收回调函数 */
    if (rt_device_set_rx_indicate(esp12f_dev, esp12f_rx_ind) != RT_EOK)
    {
        esp12f_dev = RT_NULL;
        rt_kprintf("UART2 设置接收回调函数失败\n");
        return;
    }
}

static rt_err_t esp12f_rx_ind(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(esp12f_sem);
    return RT_EOK;
}

void esp12f_thread_entry(void *parameter)                         // RT-Thread 线程入口函数：用于周期采集传感器并通过 UART 向 ESP-12F 发送数据帧
{
    char buff[256];                                               // UART 接收缓存
    unsigned int start_time = rt_tick_get();                      // 记录当前系统 tick，用于周期判断
    esp12f_sem = rt_sem_create("esp12f_sem", 0, RT_IPC_FLAG_PRIO); // 创建信号量
    if (esp12f_sem == RT_NULL)
    {
        rt_kprintf("ESP-12F 信号量初始化失败\n");
    }
    esp12f_mqt = rt_mq_create("esp12f", 1, 200, RT_IPC_FLAG_FIFO); // 创建消息队列
    rt_kprintf("ESP-12F 初始化完成\n");
    esp12f_init();                                                // 初始化 UART

    while (1)
    {
        DHT11_REC_Data(&esp_temp, &esp_humi);                             // 读取温度湿度
        esp_light = BH1750_Test();                                    // 读取光照
        esp_gas_value = (int)get_gas_real_value();                   // 读取气体
        esp_flame = get_flame_open_tim();                             // 读取火焰
        rt_thread_mdelay(100);                                    // 延时 100ms

        // 接收处理（当前注释，可根据需要启用）
        rt_err_t result = rt_sem_take(esp12f_sem, 1000);
        if (result == RT_EOK)
        {
            int i, d = rt_device_read(esp12f_dev, 0, buff, 256);
            for (i = 0; i < d; i++)
            {
                // 处理接收数据
//                rt_kprintf("接收到 %d 字节数据，已清空缓冲区\n", d);
            }
        }

        if (rt_tick_get() - start_time > 1000)                    // 每 1500 tick (1.5秒) 发送一次
        {
            // 打印原始传感器数据
            // rt_kprintf("原始传感器数据: temp=%d, humi=%d, light=%d, gas_value=%d, flame=%d\n",
            //            esp_temp, esp_humi, esp_light, esp_gas_value, esp_flame);

            int16_t te = (int16_t)(esp_temp * 100);                  // DHT11 返回整数，单位 0.01°C
            int16_t hu = (int16_t)(esp_humi * 100);                  // 0.01%RH
            uint16_t li = (uint16_t)esp_light;
            uint16_t sm = (uint16_t)esp_gas_value;
            uint8_t fl = (esp_flame == RT_TRUE) ? 1 : 0;

            // 打印转换后的数据
            // rt_kprintf("转换后数据: te=%d (0x%04X), hu=%d (0x%04X), li=%d (0x%04X), sm=%d (0x%04X), fl=%d\n",
            //            te, te, hu, hu, li, li, sm, sm, fl);

            esp12f_tx_data[4] = (uint8_t)(te & 0xFF);
            esp12f_tx_data[3] = (uint8_t)((te >> 8) & 0xFF);
            esp12f_tx_data[6] = (uint8_t)(hu & 0xFF);
            esp12f_tx_data[5] = (uint8_t)((hu >> 8) & 0xFF);
            esp12f_tx_data[8] = (uint8_t)(li & 0xFF);
            esp12f_tx_data[7] = (uint8_t)((li >> 8) & 0xFF);
            esp12f_tx_data[9] = fl;
            esp12f_tx_data[11] = (uint8_t)(sm & 0xFF);
            esp12f_tx_data[10] = (uint8_t)((sm >> 8) & 0xFF);

            uint8_t checksum = 0;
            for (int i = 3; i <= 11; i++)
            {
                checksum += (uint8_t)esp12f_tx_data[i];
            }
            esp12f_tx_data[12] = checksum;

            // 打印打包后的数据帧
            // rt_kprintf("打包数据帧: ");
            // for (int i = 0; i < 13; i++)
            // {
            //     rt_kprintf("%02X ", (uint8_t)esp12f_tx_data[i]);
            // }
            // rt_kprintf("\n校验: 0x%02X\n", checksum);

            // 注释掉实际发送，改为调试输出
            rt_device_write(esp12f_dev, 0, (void *)esp12f_tx_data, sizeof(esp12f_tx_data));
            start_time = rt_tick_get();
        }
    }
}


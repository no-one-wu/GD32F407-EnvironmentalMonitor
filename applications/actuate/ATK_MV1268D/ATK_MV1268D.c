/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-15     WJHcomputer       the first version
 */
#include <rtdevice.h>
#include <rtthread.h>
#include <rtdbg.h>
#include "string.h"
#include <stdio.h>
#include <ctype.h>
#include "ATK_MV1268D.h"
#include "Steppermotor.h"
#include "BH1750.h"
#include "DHT11.h"
#include "flame.h"
#include "MQ2.h"
#include "beep.h"


rt_mq_t ATK_MV1268D_mqt = RT_NULL;
static rt_sem_t uaart_sem = RT_NULL;
void uart4_interrupt(int vector, void *param);
void uart_thread_entry(void *parameter);
//static void ATK_MV1268D_Rx_Check(unsigned char data);
static rt_err_t uart6_rx_ind(rt_device_t dev, rt_size_t size);
static char ATK_MV1268D_tx_data[11] = { 0X00, 0X00, 0X06, 0x55, 0xBC, 0x00, 0x01, 0x00, 0x00, 0x00, 0xbb };
rt_device_t u6_dev;
struct mv1268 mv1268Data;
extern char drivers_id;
extern char Stepper_mode;
extern void voice_broadcast(unsigned char instruct);
extern void Stepper_motor_open(void);
extern void Stepper_motor_reset(void);
rt_bool_t flame = RT_FALSE;
char flame_TRUEstate[] = "ture";
char flame_FALSEstate[] = "false";
uint8_t temp = 0, humi = 0;
int light = 0;
int gas_value = 0;

void ATK_MV1268D_init(void)
{

    /* step1：查找串口设备 */
    u6_dev = rt_device_find("uart6");
    if (u6_dev == RT_NULL)
    {
        rt_kprintf("没有找到串口6");
        return;
    }

    /* step3：修改串口配置参数 */
    static struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; /* 初始化配置参数 */
    /* step2：修改串口配置参数 */
    config.baud_rate = BAUD_RATE_115200;        //修改波特率为115200
    config.data_bits = DATA_BITS_8;             //数据位 8
    config.stop_bits = STOP_BITS_1;             //停止位 1
    config.bufsz = 256;                           //修改缓冲区 buff size 为 128
    config.parity = PARITY_NONE;                //无奇偶校验位

    /* step4：控制串口设备。通过控制接口传入命令控制字，与控制参数 */
    if (rt_device_control(u6_dev, RT_DEVICE_CTRL_CONFIG, &config) != RT_EOK)
    {
        u6_dev = RT_NULL;
        rt_kprintf("串口6配置失败");
        return;
    }

    /* step2：打开串口设备。以中断接收及轮询发送模式打开串口设备 */
    if (rt_device_open(u6_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        u6_dev = RT_NULL;
        rt_kprintf("串口6打开失败");
        return;
    }
    /* step4：设置回调函数*/
    if (rt_device_set_rx_indicate(u6_dev, uart6_rx_ind) != RT_EOK)
    {
        u6_dev = RT_NULL;
        rt_kprintf("串口6设置中断回调函数失败");
        return;
    }
}

static rt_err_t uart6_rx_ind(rt_device_t dev, rt_size_t size)
{
//    rt_kprintf("rx is ok\n");
    rt_sem_release(uaart_sem);
    return RT_EOK;
}

void uart_thread_entry(void *parameter)                         // RT-Thread 线程入口函数：用于周期采集传感器并通过串口向 ATK-MV1268D 上报
{                                                               // 线程入口开始：被 rt_thread_create/rt_thread_startup 调度运行
    char buff[256];                                             // 串口接收缓存：配合 rt_device_read 从 uart6 读取数据（当前接收解析代码被注释）
    unsigned int start_time = rt_tick_get();                    // 记录当前系统 tick（节拍计数），用于“软件定时”判断是否到达上报周期
    // 创建一个信号量，初始值为 0，类型为二值信号量                         // IPC 同步说明：rx 回调释放信号量，线程可 take 等待“有数据可读”
    uaart_sem = rt_sem_create("my_sem", 0, RT_IPC_FLAG_PRIO);   // 动态创建信号量（来自堆）：初值 0；PRIO 表示按线程优先级唤醒等待者
    if (uaart_sem == RT_NULL)                                   // 判断信号量是否创建成功（失败通常是堆不足或对象创建异常）
    {                                                           // 失败分支开始
        rt_kprintf("信号量初始化失败\n");                       // 输出错误信息（rt_kprintf 为内核打印，频繁打印会影响实时性）
    }                                                           // 失败分支结束（当前实现未中止线程，后续回调释放 RT_NULL 可能导致异常）
    ATK_MV1268D_mqt = rt_mq_create("ATK_MV1268D", 1, 200, RT_IPC_FLAG_FIFO); // 创建消息队列：消息大小 1 字节，深度 200；FIFO 表示按等待顺序唤醒
    rt_kprintf("ATK_MV1268D初始化完成\n");                      // 提示 IPC 资源准备完成（并不代表串口/模块已完全可用）
    ATK_MV1268D_init();                                         // 初始化串口6：rt_device_find/control/open + rt_device_set_rx_indicate 安装接收回调
                                                                // 空行：分隔初始化与主循环逻辑
    while (1)                                                   // 线程主循环：RT-Thread 下线程通常通过 while(1) 常驻运行
    {                                                           // 循环体开始
        DHT11_REC_Data(&temp, &humi);                            // 读取 DHT11：通过驱动/时序采样更新温度 temp 与湿度 humi（输出参数）
        light = BH1750_Test();                                   // 读取 BH1750：返回光照值（单位与缩放由驱动定义）
        gas_value = (int)get_gas_real_value();                   // 读取 MQ2：获取气体浓度/传感值并转为 int（便于协议上报）
        flame = get_flame_open_tim();                            // 读取火焰传感器：返回 rt_bool_t（RT_TRUE/RT_FALSE）
        rt_thread_mdelay(100);                                   // 线程延时 100ms：让出 CPU，进入延时阻塞态，避免忙等占用处理器
//        rt_err_t result = rt_sem_take(uaart_sem, 1000);          // 线程等待串口接收事件：最多等待 1000 tick（tick 与 RT_TICK_PER_SECOND 相关）
//        if (result == RT_EOK)                                    // 若成功拿到信号量，说明 rx 回调已释放，串口驱动 ringbuffer 中可能有新数据
//        {                                                        // 接收处理开始
//            int i, d = rt_device_read(u6_dev, 0, buff, 256);     // 从 uart6 读取数据：返回实际读取字节数 d（非阻塞/阻塞由驱动与打开方式决定）
//            for (i = 0; i < d; i++)                              // 逐字节送入协议解析状态机
//            {                                                    // 逐字节解析循环开始
//                ATK_MV1268D_Rx_Check(buff[i]);                   // 协议解析：通常会入队、拼帧、校验并触发对应动作（当前函数在本文件中被注释）
//                //rt_kprintf("%x", buff[i]);                    // 调试打印接收字节（开启会显著影响实时性与吞吐）
//            }                                                    // 逐字节解析循环结束
////            rt_kprintf("%d\n",d);                               // 调试：打印本次读取长度
//        }                                                        // 接收处理结束
        if (rt_tick_get() - start_time > 500)                    // 周期判断：tick 差值超过 500 则触发一次“上报序列”
        {                                                         // 周期上报开始
            ATK_MV1268D_TxT_Check(5, temp, 0);                    // 上报温度：地址(通道)=5；按协议拆分十位/个位（见 ATK_MV1268D_TxT_Check）
            rt_thread_mdelay(10);                                 // 发送间隔：给模块处理留时间，降低串口连发导致的拥塞风险
            ATK_MV1268D_TxT_Check(6, humi, 0);                    // 上报湿度：地址(通道)=6
            rt_thread_mdelay(10);                                 // 发送间隔
            ATK_MV1268D_TxT_Check(7, light / 100, light % 100);   // 上报光照：将 light 拆为两段参数（协议层约定：整数/小数或高/低位）
            rt_thread_mdelay(10);                                 // 发送间隔
            ATK_MV1268D_TxT_Check(8, gas_value, 0);               // 上报气体值：地址(通道)=8
            rt_thread_mdelay(10);                                 // 发送间隔
            if (flame == RT_TRUE)                                 // 条件判断：火焰传感器触发则上报告警
            {                                                     // 触发分支开始
                                                                // 空行：保持原有代码风格（不影响逻辑）
                ATK_MV1268D_TxT_Check(9, 1, 1);                   // 上报火焰告警：地址(通道)=9；参数含义由协议定义（此处置 1）
                rt_thread_mdelay(10);                              // 发送间隔
                                                                // 空行：保持原有代码风格（不影响逻辑）
            }                                                     // 触发分支结束
            else                                                  // 未触发火焰：上报正常状态
            {                                                     // 未触发分支开始
                ATK_MV1268D_TxT_Check(9, 0, 0);                   // 上报火焰正常：地址(通道)=9；参数清零
                rt_thread_mdelay(10);                              // 发送间隔
            }                                                     // 未触发分支结束
            start_time = rt_tick_get();                            // 重置周期起点：下一次周期从当前 tick 开始累计
//            for(uint8_t i = 0;i < 11;i++)                         // 调试：遍历发送帧数组（11 字节）观察协议内容
//            {                                                     // 调试块开始
//                rt_kprintf("%#x",ATK_MV1268D_tx_data[i]);        // 打印发送帧每个字节（用于校验帧格式/校验值）
//            }                                                     // 调试块结束
        }                                                         // 周期上报结束
    }                                                             // 循环体结束（线程继续下一轮）
}                                                               // 线程入口结束（理论上不会到达）

//static void ATK_MV1268D_Rx_Check(unsigned char data)
//{
//    static unsigned char zigbeeData[8] = {0};
//    int i = 0;
//    if (rt_mq_send(ATK_MV1268D_mqt, &data, 1) != RT_EOK)
//        rt_kprintf("WIFI数据写入FIFO失败");
//    while (zigbeeData[0] != 0x55)
//    {
//        if (rt_mq_recv(ATK_MV1268D_mqt, &zigbeeData[0], 1, RT_WAITING_NO) != RT_EOK)
//            return;
//    }
//
//    if (ATK_MV1268D_mqt->entry < 9)
//        return;
//
//    for (i = 1; i < 8; i++)
//    {
//
//        if (rt_mq_recv(ATK_MV1268D_mqt, &zigbeeData[i], 1, RT_WAITING_NO) != RT_EOK)
//            return;
//            rt_kprintf("%x",zigbeeData[i]);
//    }
//    rt_kprintf("\n");
//    zigbeeData[0] = 0x00;
//    if (zigbeeData[7] != 0xBB) // 验证数据格式是否合法
//        return;
//
//    if (((zigbeeData[2] + zigbeeData[3] + zigbeeData[4] + zigbeeData[5]) % 256) != zigbeeData[6]) // 校验
//        return;
//
//    if (zigbeeData[1] == 0xBC) // 语音播报返回
//    {
//        if (zigbeeData[2] == drivers_id)
//        {
//            if (zigbeeData[4] == 1&&Stepper_mode==0)
//            {
//                rt_kprintf("执行除尘\n");
//                voice_broadcast(3);
//                Stepper_motor_open();
//
//            }
//            else if (zigbeeData[5] == 1&&Stepper_mode==0)
//            {
//                rt_kprintf("执行复位\n");
//                Stepper_motor_reset();
//
//            }
//        }
//    }
//}

//void ATK_MV1268D_Tx_Check(unsigned char ATK_MV1268D_address, unsigned charFeature)
//{
//    ATK_MV1268D_tx_data[5] = ATK_MV1268D_address;
//    ATK_MV1268D_tx_data[7] = charFeature == 1 ? 1 : 0;
//    ATK_MV1268D_tx_data[8] = charFeature == 2 ? 1 : 0;
//    ATK_MV1268D_tx_data[9] = (ATK_MV1268D_tx_data[5] + ATK_MV1268D_tx_data[6] + ATK_MV1268D_tx_data[7]
//            + ATK_MV1268D_tx_data[8]) % 256;
//    rt_device_write(u6_dev, 0, (void *) ATK_MV1268D_tx_data, sizeof(ATK_MV1268D_tx_data));
//}

//void set_Photovoltaic_panel_mode(unsigned char ATK_MV1268D_address, unsigned charFeature){
//    if(ATK_MV1268D_address<1||ATK_MV1268D_address>4)
//        return;
//    if(charFeature<1||charFeature>2)
//        return;
//    mv1268Data.setID[ATK_MV1268D_address-1] = charFeature;
//    ATK_MV1268D_Tx_Check(ATK_MV1268D_address,charFeature);
//}

void ATK_MV1268D_TxT_Check(unsigned char ATK_MV1268D_address, unsigned int charFeature,unsigned int charFeature2)
{
    ATK_MV1268D_tx_data[5] = ATK_MV1268D_address;
    switch(ATK_MV1268D_address)
    {
        case 5:{
            ATK_MV1268D_tx_data[7] = charFeature / 10;
            ATK_MV1268D_tx_data[8] = charFeature % 10;
            break;
        }
        case 6:{
            ATK_MV1268D_tx_data[7] = charFeature / 10;
            ATK_MV1268D_tx_data[8] = charFeature % 10;
            break;
        }
        case 7:{
            ATK_MV1268D_tx_data[6] = charFeature / 10;
            ATK_MV1268D_tx_data[7] = charFeature % 10;
            ATK_MV1268D_tx_data[8] = charFeature2 / 10;
            ATK_MV1268D_tx_data[9] = charFeature2 % 10;
            break;
        }
        case 8:{
            ATK_MV1268D_tx_data[7] = charFeature / 10;
            ATK_MV1268D_tx_data[8] = charFeature % 10;
            break;
        }
        case 9:{
            ATK_MV1268D_tx_data[7] = charFeature;
            ATK_MV1268D_tx_data[8] = charFeature2;
            break;
        }
    }

    ATK_MV1268D_tx_data[9] = (ATK_MV1268D_tx_data[5] + ATK_MV1268D_tx_data[6] + ATK_MV1268D_tx_data[7]
            + ATK_MV1268D_tx_data[8]) % 256;
    rt_device_write(u6_dev, 0, (void *) ATK_MV1268D_tx_data, sizeof(ATK_MV1268D_tx_data));
}


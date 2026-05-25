/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-01-16     lp666       the first version
 */
#include "all_task.h"

#include "led.h"
#include "beep.h"

rt_uint8_t buffer_5[64];
static rt_tick_t start_time = 0; // 全局变量声明

void send_to_nextion_7(rt_device_t lcd_dev, const char *format, ...)
{
    // 定义一个足够大的缓冲区存放格式化后的字符串
    char text_buffer[256];
    va_list args;
    va_start(args, format);
    int n = vsnprintf(text_buffer, sizeof(text_buffer), format, args);
    va_end(args);

    if (n < 0) {
        rt_kprintf("格式化错误！\n");
        return;
    }

    // 构造发送给 Nextion 的完整命令：前缀 + 格式化后的文本 + 后缀 + 3个0xFF结尾
    const char* prefix = "page7.t0.txt=\"";
    const char* suffix = "\"";

    size_t prefix_len = strlen(prefix);
    size_t text_len   = strlen(text_buffer);
    size_t suffix_len = strlen(suffix);

    // 总长度：前缀 + 文本 + 后缀 + 3个0xFF + 1个'\0'
    size_t total_len = prefix_len + text_len + suffix_len + 3 + 1;

    rt_uint8_t* buffer = (rt_uint8_t*)rt_malloc(total_len);
    if (!buffer)
    {
        rt_kprintf("内存分配失败!\n");
        return;
    }
    memset(buffer, 0, total_len);

    // 拼接命令：依次复制前缀、格式化后的文本、后缀
    memcpy(buffer, prefix, prefix_len);
    memcpy(buffer + prefix_len, text_buffer, text_len);
    memcpy(buffer + prefix_len + text_len, suffix, suffix_len);

    // 添加 3 个0xFF 作为 Nextion命令结束标志
    buffer[prefix_len + text_len + suffix_len + 0] = 0xFF;
    buffer[prefix_len + text_len + suffix_len + 1] = 0xFF;
    buffer[prefix_len + text_len + suffix_len + 2] = 0xFF;

    // 最后一个字节置为'\0'
    buffer[prefix_len + text_len + suffix_len + 3] = '\0';

    // 发送数据时不包括最后的'\0'
    size_t write_len = prefix_len + text_len + suffix_len + 3;
    rt_device_write(lcd_dev, 0, buffer, write_len);

    rt_kprintf("Send to Nextion (含0xFFx3): %s\n", buffer);
    rt_free(buffer);
}
void RW7_thread1(void)
{
//    OLED_Init();
//    OLED_ShowString(1,1,"start01",16, 1);
//    OLED_Refresh();
//    beep_init();
//
//    beep_music(1,500);
//    beep_music(2,500);
//    beep_music(3,500);
//    beep_music(4,500);
//    beep_music(5,500);
//    beep_music(6,500);
//    beep_music(7,500);
//    beep_music(0,0);

    while(1)
    {

        if (Taskshutdown != 7) {


            BEEP_OFF;
            return;  // 退出任务
        }


    }
}

void task_7(void)
{




    Taskshutdown = 7;
    rt_mb_send(task1_thread_mailbox,(rt_ubase_t)RW7_thread1);

}

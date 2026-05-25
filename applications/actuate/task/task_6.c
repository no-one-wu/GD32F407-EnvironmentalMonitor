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

extern int turn;

void send_to_nextion_6_6(rt_device_t lcd_dev,const char* prefix,const char* suffix, const char *format, ...)
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
//    const char* prefix = "page0.t1.txt=\"";
//    const char* suffix = "\"";

    size_t prefix_len = strlen(prefix);
    size_t text_len   = strlen(text_buffer);
    size_t suffix_len = strlen(suffix);
    rt_kprintf("prefix_len = %d,text_len = %d,suffix_len = %d\n",prefix_len,text_len,suffix_len);

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
void send_to_nextion_6x_6(rt_device_t lcd_dev, const char *format, ...)
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
    const char* prefix = "page6.t2.txt=\"";
    const char* suffix = "\"";

    size_t prefix_len = strlen(prefix);
    size_t text_len   = strlen(text_buffer);
    size_t suffix_len = strlen(suffix);
    rt_kprintf("prefix_len = %d,text_len = %d,suffix_len = %d\n",prefix_len,text_len,suffix_len);

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

void RW6_thread1(void)
{
//    OLED_Init();
//    C1016_Init();
//    const char *kuang = "page6.b1.txt=\"";
//    const char* pie = "\"";
//    uint8_t num0 = 0,num1 = 0,num2 = 0,num3 = 0,num4 = 0,num5 = 0,num6 = 0,num7 = 0,num8 = 0,num9 = 0;
//    OLED_ShowString(1,1,"one",16, 1);
//    OLED_Refresh();
//    rt_thread_mdelay(3000);
//    C1016_Enroll(1);
//
//    OLED_ShowString(1,1,"two",16, 1);
//    OLED_Refresh();
//    rt_thread_mdelay(3000);
//    C1016_Enroll(2);
//
//    OLED_ShowString(1,1,"three",16, 1);
//    OLED_Refresh();
//    rt_thread_mdelay(3000);
//    C1016_Enroll(3);
//
//    BEEP_Control(RT_TRUE);
//    rt_thread_mdelay(1000);
//    BEEP_Control(RT_FALSE);
//
//    int n = 0;

    while(1)
    {
//        n = C1016_Identify();
//        if(n == 1 && num0 == 0)
//        {
//            send_to_nextion_6_6(lcd_dev,kuang,pie,"1");
//            num0 = 1;
//            num1 = 0;
//            num2 = 0;
//        }
//        else if(n == 2 && num1 == 0)
//        {
//            send_to_nextion_6_6(lcd_dev,kuang,pie,"2");
//            num0 = 0;
//            num1 = 1;
//            num2 = 0;
//        }
//        else if(n == 3 && num2 == 0)
//        {
//            send_to_nextion_6_6(lcd_dev,kuang,pie,"3");
//            num0 = 0;
//            num1 = 0;
//            num2 = 1;
//        }


        if (Taskshutdown != 6) {


            return;  // 退出任务
        }

        rt_thread_mdelay(50);
    }
}


void task_6(void)
{

    Taskshutdown = 6;
    rt_mb_send(task1_thread_mailbox,(rt_ubase_t)RW6_thread1);

}

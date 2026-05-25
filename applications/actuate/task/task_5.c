/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-19     lp666       the first version
 */

#include "all_task.h"
#include "smg.h"

void send_to_nextion_5_5(rt_device_t lcd_dev,const char* prefix,const char* suffix, const char *format, ...)
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
void RW5_thread1(void)
{
//    uint8_t kuang[20] = "page5.t0.txt=\"";
//    uint8_t pie[5] = "\"";
//    uint8_t temp = 0, humi = 0;
//    int num1 = 0,num2 = 0;
//    IR_Init();
//    LED_RGB_init();
//    rt_bool_t fall = 0;

    while(1)
    {
//        fall = get_ir_open_tim();
//        if(fall == RT_TRUE && num1 == 0)
//        {
//            LEDx_Set_Status(OFF);
//            LEDx_Set_Status(redRGB_ON);
//            num1++;
//            send_to_nextion_5_5(lcd_dev,kuang,pie,"当前被魂挡，请挪开!");
//            num2 = 0;
//        }
//        else if(fall == RT_FALSE && num2 == 0)
//        {
//            LEDx_Set_Status(OFF);
//            LEDx_Set_Status(greenRGB_ON);
//            num2++;
//            send_to_nextion_5_5(lcd_dev,kuang,pie,"实时监测中，请您检验!");
//            num1 = 0;
//        }


        if (Taskshutdown != 5) {


            return;  // 退出任务
        }
        rt_thread_mdelay(100);
    }
}


void task_5(void)
{

    Taskshutdown = 5;
    Smg_init();
    rt_mb_send(task1_thread_mailbox,(rt_ubase_t)RW5_thread1);

}

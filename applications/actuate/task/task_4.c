/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-11     lp666       the first version
 */
#include "all_task.h"

#include "led.h"
#include "beep.h"
#include "bkrc_voice.h"
#include "actuate/paj7620u2/paj7620u2_iic.h"

#include "Ultrasonic.h"

extern int miao;
extern int fen;

void send_to_nextion_4_4(rt_device_t lcd_dev,const char* prefix,const char* suffix, const char *format, ...)
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

void  RW4_thread1(void)
{
//    LED_RGB_init();
//    Voice_Config_Init();
//    voice_rouse();
//    voice_broadcast(8);
//    const char* prefix = "page4.t0.txt=\"";
//    const char* pie = "\"";
//    uint8_t num1 = 0,num2 = 0,num3 = 0,num4 = 0;
//    unsigned short ligh = 0;
//    uint8_t n = 0,n1 = 0,n3 = 0;

    while(1)
    {
//        ligh = BH1750_Test();
//        num1++;
//        if(num1>=5)
//        {
//            send_to_nextion_4_4(lcd_dev,prefix,pie,"%d",ligh);
//            num1 = 0;
//        }
//        n = Voice_Drive();
//        if(n != 255)
//        {
//            n1 = n;
//            num1 = 0;
//            num2 = num1;
//            num3 = num1;
//            num4 = num1;
//        }
//
//        if(n1 == 1 && num2 == 0)
//        {
//            BEEP_Control(RT_TRUE);
//            rt_thread_mdelay(1000);
//            BEEP_Control(RT_FALSE);
//            num2 = 1;
//        }
//        else if(n1 == 2 || n3 == 1)
//        {
//            if(ligh > 100 && n3 == 0)
//            {
//                voice_broadcast(9);
//                n3 = 1;
//            }
//            if(n1 == 6)
//            {
//                LEDx_Set_Status(redRGB_ON);
//                n3 = 0;
//            }
//            if(n1 == 7)
//            {
//                n3 = 0;
//            }
//        }
//        else if(n1 == 3)
//        {
//            LEDx_Set_Status(OFF);
//        }
//        else if(n1 == 4 && num3 == 0)
//        {
//            step_28byj48_angles(90,1);
//            num3 = 1;
//        }
//        else if(n1 == 5 && num4 == 0)
//        {
//            step_28byj48_angles(90,0);
//            num4 = 1;
//        }


        if (Taskshutdown == 0)
        {
            LEDx_Set_Status(OFF);
            BEEP_Control(RT_FALSE);
            return;  // 退出任务
        }

        rt_thread_mdelay(100);
    }

}


void task_4(void)
{

    Taskshutdown = 4;
    rt_mb_send(task1_thread_mailbox,(rt_ubase_t)RW4_thread1);

}

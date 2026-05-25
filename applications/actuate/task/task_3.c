#include "all_task.h"
#include "DHT11.h"

#include "actuate/ds1302/ds1302.h"
#include "Ultrasonic.h"

#include "led.h"

#include "beep.h"

extern int ligh;

void send_to_nextion_3_n(rt_device_t lcd_dev,const char* prefix,const char* suffix, const char *format, ...)
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

void RW3_thread1(void)
{
//    uint8_t kuang1[20] = "page3.g0.txt=\"";
//    uint8_t pie[5] = "\"";
//    flame_Init();
//    Fan_init(0);

    while(1)
    {
//        if(get_flame_open_tim() == RT_TRUE)
//        {
//            BEEP_Control(RT_TRUE);
//            seg_Fan_pwd(100);
//        }
//        else {
//            BEEP_Control(RT_FALSE);
//            seg_Fan_pwd(0);
//        }

        if (Taskshutdown != 3) {

            BEEP_Control(RT_FALSE);
            Fan_init(0);

            return;  // 退出任务
        }

        rt_thread_mdelay(100);
    }
}


void task_3(void)
{
    Taskshutdown = 3;
    rt_mb_send(task1_thread_mailbox,(rt_ubase_t)RW3_thread1);

}

/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-26     lzh28       the first version
 */
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-25     lzh28       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "matrix.h"
#include "board.h"
#include "all_task.h"
//static rt_thread_t matrixThread;
static rt_base_t matrix_RCK_pin = RT_NULL, matrix_SCK_pin = RT_NULL, matrix_SOE_pin = RT_NULL, matrix_DATA_pin = RT_NULL;


//行扫描数据数组
unsigned short Data_H[16] = {  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,}; //第一个行管HC595




/* 线程 1 入口 */
void matrixThread_entry(void* parameter);
void matrix_init(void)
{
//    /* 创建线程 2
//          *   线程的入口是 thread2_entry, 参数是 RT_NULL
//          *   栈空间是 512，优先级是 250，时间片是 25 个 OS Tick
//     */
//    matrixThread = rt_thread_create("matrixThread", matrixThread_entry, RT_NULL, 512, 20, 1);
//    /* 启动线程 */
//    if (matrixThread != RT_NULL)
//        rt_thread_startup(matrixThread);
    matrix_RCK_pin = rt_pin_get("PC.2");  //输出
    matrix_SCK_pin = rt_pin_get("PC.1");  //输出
    matrix_SOE_pin = rt_pin_get("PC.0");  //输出
    matrix_DATA_pin = rt_pin_get("PC.3");  //输出


    rt_pin_mode(matrix_RCK_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(matrix_SCK_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(matrix_SOE_pin, PIN_MODE_OUTPUT);
    rt_pin_mode(matrix_DATA_pin, PIN_MODE_OUTPUT);
    matrix_SOE_RESET  //使能引脚
}


void HC595_Send_16Bit(uint16_t data)
{
    uint16_t i = 0;
    for(i=0;i<16;i++)
    {
        if((data & 0x8000) == 0X8000)
        {
            matrix_DATA_SET;
        }
        else
        {
            matrix_DATA_RESET;
        }
        matrix_SCK_RESET;
        matrix_SCK_SET;                     //移位输入时钟，上升沿输入
        data <<=1;
    }
    matrix_RCK_RESET;
    matrix_RCK_SET;
}



void HC595_Send_Data(uint8_t dat)
{
    unsigned char i;
    for(i = 0; i < 8; i++)
    {
        if( ( dat << i ) & 0x80)
        {
            matrix_DATA_SET;
        }
        else
        {
            matrix_DATA_RESET;
        }
        matrix_SCK_RESET;
        matrix_SCK_SET;                     //移位输入时钟，上升沿输入
    }

    matrix_RCK_RESET;
    matrix_RCK_SET;
}



void Dis_Char_1(unsigned char *data)
{
   unsigned char i,j=0;
   for(i = 0; i < 8; i++)//扫描16行
   {

       HC595_Send_Data(data[j]);


       HC595_Send_Data(Data_H[i]);
       j++;
   }
   j = 0;
}




uint16_t HC595_Dat_Handle(uint8_t dat,uint8_t cnt)
{
    uint16_t  Rt = 0;
    Rt = cnt;
    Rt <<= 8;
    Rt |= (uint8_t)(~dat);
    return Rt;
}

void matrix_OFF(void)
{
    /**
     * @brief 关闭点整
     * @param 1 void
     * @return void
     */
    matrix_SOE_SET
}
void matrix_ON(void)
{
    /**
     * @brief 开启点整
     * @param 1 void
     * @return void
     */
    matrix_SOE_RESET
}

/*************************************************************************************
**函数功能：静态显示 //处理成从右至左扫描，数据高位在上
**说明：可显示一个汉字和字符，图片等
**参数：@unsigned char *data  -- 数据
**************************************************************************************/
void Dis_Char(unsigned char *data)
{
    uint16_t dat = 0;
    for(int i=0;i<8;i++)
    {
        dat = HC595_Dat_Handle(*(data+i),1<<i);
        HC595_Send_16Bit(dat);
        rt_thread_mdelay(3);
    }

}



/****  跳出任务是写死在任务5   if (Taskshutdown != 5) 如果需要在其他任务记得修改  **********/
void Dis_Dynamic(unsigned char *data, unsigned int len)  //从下到上  第一个参数是数组,第二个是二维数组的第一个*第二个的总长度
{
    uint16_t dat = 0;
    unsigned char cnt = 0;
    unsigned char k;

    while (1) // **持续滚动**
    {
        if (Taskshutdown != 5)
        {
            matrix_OFF();  // 关闭点阵显示
            return;
        }
        for (k = 0; k < 200; k++)  // **增加显示稳定性**
        {
            for (int i = 0; i < 8; i++)
            {
                // **确保 cnt 影响列数据**
                dat = HC595_Dat_Handle(*(data + cnt + i), 1 << i);
                HC595_Send_16Bit(dat); // **修正这里的错误**
                rt_thread_mdelay(3);
            }
        }

        cnt += 1; // **正确增加滚动偏移**
        if (cnt > len - 8)
            cnt = 0;
    }
}





/****  跳出任务是写死在任务5   if (Taskshutdown != 5) 如果需要在其他任务记得修改  **********/
/**  参数第一个是数组，第二个是二维数组的第一个不是第二个  **/
void Dis_Dynamic_Horiz_Extend(unsigned char (*pattern)[8], unsigned int num_chars)  //从右向左
{
    uint16_t dat = 0;
    unsigned char offset = 0;
    unsigned char k;
    unsigned int total_width = num_chars * 8;

    // 用于存储每一行拼接后的数据，采用 64 位整数（适用于字符数较少的情况）
    unsigned long long full[8] = {0};


    for (int i = 0; i < 8; i++)
    {
        full[i] = 0;
        for (int j = 0; j < num_chars; j++)
        {
            full[i] = (full[i] << 8) | pattern[j][i];
        }
    }

    while(1)  // 持续滚动显示
    {
        if (Taskshutdown != 5)
        {
            matrix_OFF();  // 关闭点阵显示
            return;
        }
        // 重复刷新当前显示帧以保证稳定性
        for (k = 0; k < 200; k++)
        {
            // 行扫描：逐行显示
            for (int i = 0; i < 8; i++)
            {
                // 从拼接后的整行数据中提取 8 位作为当前显示窗口
                unsigned char display_byte = (full[i] >> offset) & 0xFF;
                // 将显示数据和行选择位组合后发送到 74HC595
                dat = HC595_Dat_Handle(display_byte, 1 << i);
                HC595_Send_16Bit(dat);
                rt_thread_mdelay(3);
            }
        }
        // 更新偏移量，实现水平滚动（窗口向左移动）
        offset++;
        if (offset > (total_width - 8))
            offset = 0;
    }
}




/****  跳出任务是写死在任务5!   if (Taskshutdown != 5) 如果需要在其他任务记得修改  **********/
/**  参数第一个是数组，第二个是二维数组的第一个不是第二个  **/
void Dis_Dynamic_Horiz_L2R(unsigned char (*pattern)[8], unsigned int num_chars) //从左到右
{
    uint16_t dat = 0;
    unsigned char offset = 0;
    unsigned int total_width = num_chars * 8;  // 总列数

    // 拼接每一行的数据到一个 64 位整数中
    unsigned long long full[8] = {0};
    for (int i = 0; i < 8; i++)
    {
        full[i] = 0;
        for (int j = 0; j < num_chars; j++)
        {
            full[i] = (full[i] << 8) | pattern[j][i];
        }
    }

    // 进入函数后，先刷新一次，确保马上能看到数据
    for (int i = 0; i < 8; i++)
    {
        unsigned char display_byte = (full[i] >> (total_width - 8 - offset)) & 0xFF;
        dat = HC595_Dat_Handle(display_byte, 1 << i);
        HC595_Send_16Bit(dat);
        rt_thread_mdelay(3);
    }

    // 进入滚动显示主循环
    while (1)
    {

        for(int i = 0; i<20;i++)
        {
            // 每次刷新前检查退出条件
            if (Taskshutdown != 5)
            {
                matrix_OFF();  // 关闭点阵显示
                return;
            }

            // 刷新一帧：逐行输出当前窗口数据
            for (int i = 0; i < 8; i++)
            {
                unsigned char display_byte = (full[i] >> (total_width - 8 - offset)) & 0xFF;
                dat = HC595_Dat_Handle(display_byte, 1 << i);
                HC595_Send_16Bit(dat);
                rt_thread_mdelay(3);
            }
            // 更新偏移量，实现窗口向右移动（从左到右滚动）

        }
        offset++;
        if (offset > (total_width - 8))
        {
            offset = 0;
        }
    }
    // 注意：退出时不对 g_scroll_enable 进行置零操作
}



//从上往下但是有点问题显示完第一个直接就显示最后一个再显示第二个
void Dis_Dynamic_Vertical_TopDown(unsigned char (*pattern)[8], unsigned int num_chars)
{
    uint16_t dat = 0;
    unsigned int total_rows = num_chars * 8;   // 拼接后的总行数
    int offset = 0;                            // 当前窗口偏移（有效范围：0 ~ total_rows-1）

    // 使用一个局部数组存储拼接后的竖直数据（按字符顺序堆叠，每个字符8行）
    unsigned char vertical_full[total_rows];
    for (int i = 0; i < num_chars; i++)
    {
        for (int row = 0; row < 8; row++)
        {
            vertical_full[i * 8 + row] = pattern[i][row];
        }
    }

    // 进入函数后，先刷新一次，确保立即显示初始窗口内容
    for (int i = 0; i < 8; i++)
    {
        // 采用 (i - offset + total_rows) % total_rows 保证索引在合法范围内
        unsigned char display_byte = vertical_full[(i - offset + total_rows) % total_rows];
        dat = HC595_Dat_Handle(display_byte, 1 << i);
        HC595_Send_16Bit(dat);
        rt_thread_mdelay(3);
    }

    // 进入滚动显示主循环
    while (1)
    {
        // 刷新当前帧若干次，增强显示稳定性
        for (int frame = 0; frame < 25; frame++)
        {
            if (Taskshutdown != 5)
            {
                rt_kprintf("Exiting vertical top-down scroll, Taskshutdown = %d\n", Taskshutdown);
                matrix_OFF();  // 关闭点阵显示
                return;
            }
            for (int i = 0; i < 8; i++)
            {
                unsigned char display_byte = vertical_full[(i - offset + total_rows) % total_rows];
                dat = HC595_Dat_Handle(display_byte, 1 << i);
                HC595_Send_16Bit(dat);
                rt_thread_mdelay(3);
            }
        }
        // 更新偏移量——向下滚动：offset 正向增加
        offset++;
        if (offset >= total_rows)
            offset = 0;
    }

}

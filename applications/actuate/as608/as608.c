/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-13     lp666       the first version
 */

#include "rtthread.h"
#include "as608.h"
#include <rtdevice.h>
#include <rtdbg.h>
#include <string.h>

rt_device_t c1016_dev;
uint8_t c1016_buff[128];


volatile uint8_t c1016_rx_flag = 0; //接收标志
uint8_t c1016_weakup_flag = 0;

uint16_t rx_size = 26;




rt_err_t c1016_rx_func_cb(rt_device_t dev, rt_size_t size)
{   //串口回调函数
    static unsigned char ch = 0;
    static unsigned int index = 0;

    if (dev == c1016_dev && rt_device_read(c1016_dev, 0, &ch, 1))
    {
        /* receive data */
        c1016_buff[index++] = ch;
        if(index >= rx_size)
        {
            c1016_rx_flag=1;
            index = 0;

        }
    }
    return 0;
}

void c1016_iqr_func_cb(void *args)
{ //指纹模块引脚检测中断
    c1016_weakup_flag = 1;
}

void C1016_OUT_Init(void)
{  //指纹输入检测
   rt_base_t pin = rt_pin_get(C1016_OUT);

   rt_pin_mode(pin, PIN_MODE_INPUT);

   rt_pin_attach_irq(pin, PIN_IRQ_MODE_FALLING, c1016_iqr_func_cb, RT_NULL);

   rt_pin_irq_enable(pin, PIN_IRQ_ENABLE);
}


//指纹模块初始化
void C1016_Init(void)
{
        /* step1：查找串口设备 */
        c1016_dev = rt_device_find(C1016_NAME);
        if (c1016_dev == RT_NULL)
        {
            rt_kprintf("no find %s\n",C1016_NAME);
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
        if (rt_device_control(c1016_dev, RT_DEVICE_CTRL_CONFIG, &config) != RT_EOK)
        {
            c1016_dev = RT_NULL;
            rt_kprintf("config failed %s\n",C1016_NAME);
            return;
        }
        /* step4：打开串口设备。以中断接收及轮询发送模式打开串口设备 */
        if (rt_device_open(c1016_dev, RT_DEVICE_OFLAG_RDWR |RT_DEVICE_FLAG_INT_TX|RT_DEVICE_FLAG_INT_RX) != RT_EOK)
        {
            c1016_dev = RT_NULL;
            rt_kprintf("open failed %s\n",C1016_NAME);
            return;
        }
        //设置接收回调函数
        rt_device_set_rx_indicate(c1016_dev,c1016_rx_func_cb);

        C1016_OUT_Init();
}





void C1016_Data_Verification(uint8_t *da,uint8_t len)
{
    unsigned int lenl, lenh, lensum;
    if(len >= 26)
    {
        lenl = da[0] + da[1] + da[2] + da[3] +da[4] +da[5] +da[6] +da[7] + da[8] + da[9] + da[10];
        lenh = da[11] + da[12] + da[13] + da[14] +da[15] +da[16] +da[17] +da[18] + da[19] + da[20] + da[21] + da[22] + da[23];
        lensum = (lenl + lenh) % 65536;
        da[24] = lensum % 256;
        da[25] = lensum / 256;
    }
}

uint16_t C1016_Get_Verification(uint8_t *da,uint8_t len)
{
    unsigned int lenl, lenh, lensum = 0;
    if(len >= 26)
    {
        lenl = da[0] + da[1] + da[2] + da[3] +da[4] +da[5] +da[6] +da[7] + da[8] + da[9] + da[10];
        lenh = da[11] + da[12] + da[13] + da[14] +da[15] +da[16] +da[17] +da[18] + da[19] + da[20] + da[21] + da[22] + da[23];
        lensum = (lenl + lenh) % 65536;
    }
    return lensum;
}

uint8_t pData[] = {0x55,0xaa,0x00,0x00,0x24,0x00,0x02,0x00,0x01,0x00,0x00,0x00,0x00,
                   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x01
                  };

/**
 * 描述：检查指定的编号是否已被注册
 * 示例：0001
 *      CMD_GET_STATU[8] = 0x01
 *      CMD_GET_STATU[9] = 0x00
 * **/
uint8_t CMD_GET_STATU[] = {0x55,0xAA,0x00,0x00,0x46,0x00,0x02,0x00,0x01,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x01
                          };

/**
 *采集指纹图像
 *命令 0x0020
 * **/
uint8_t CMD_GET_IMAGE[] = {0x55,0xAA,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x01
                          };

/**
 * 从 ImageBuffer 中的指纹图像产生指纹模板 Template 并保存于指定 Ram Buffer 中
 * CMD_GENERATE
 * 命令 0x0060
 *示例 保存到 Ram Buffer1
 *CMD_GENERATE[8] = 01
 *CMD_GENERATE[9] = 00
 *
 * **/
uint8_t CMD_GENERATE[] = {0x55,0xAA,0x00,0x00,0x60,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x61,0x01
                         };


/**
* 删除指定编号范围内指纹
* CMD_GENERATE
* 命令 0x0044
*
*
* **/
uint8_t CMD_DEL_CHAR[] = {0x55,0xAA,0x00,0x00,0x44,0x00,0x04,0x00,0x01,0x00,0x32,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x02
                         };
/**
 * 将暂存在 Ram Buffer 中的模板合并生成模板数据并后保存于指定的 Ram Buffer 中。
 * CMD_MERGE 0x0061
 *
 *示例 合成个数 3
 *CMD_MERGE[10] = 03
 *
 *示例 保存于指定的 Ram Buffer0
 *CMD_MERGE[8] = 00
 *CMD_MERGE[9] = 00
 * **/

uint8_t CMD_MERGE[] = {0x55,0xAA,0x00,0x00,0x61,0x00,0x03,0x00,0x00,0x00,0x03,0x00,0x00,
                                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x01
                      };

/**
 * 将暂存于指定 Ram Buffer 中的指纹模板保存于指定编号的模块指纹库中。
 * CMD_STORE_CHAR 0x0040
 *
 *示例 保存 RamBuffer0 中的模板数据到指定编号为 1 的模块数据库中：
 *CMD_STORE_CHAR[8] = 01
 *CMD_STORE_CHAR[9] = 00
 *
 *CMD_STORE_CHAR[10] = 00
 *CMD_STORE_CHAR[11] = 00
 *
 * **/
uint8_t CMD_STORE_CHAR[] = {0x55,0xAA,0x00,0x00,0x40,0x00,0x04,0x00,0x01,0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x01
                           };


/**
 * 指定 Ram Buffer 中的模板与指定搜索范围(起始 Template 编号 ~ 结束 Template 编号)内的
 * 所有已注册指纹 Template 之间进行 1:N 比对并返回其结果。
 * CMD_SEARCH 0x0063
 *实例 4.23- 暂存在 RamBuffer0 中的模板在 1-200 编号范围内的指纹比对，比对结果 ID=8
 *
 *CMD_SEARCH[8] = 00
 *CMD_SEARCH[9] = 00
 *CMD_SEARCH[9] = 01
 *CMD_SEARCH[9] = 00
 *CMD_SEARCH[9] = C8
 *CMD_SEARCH[9] = 00
 *
 * **/
uint8_t CMD_SEARCH[] = {0x55,0xAA,0x00,0x00,0x63,0x00,0x06,0x00,0x00,0x00,0x01,0x00,0x50,
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x31,0x02
                       };


/**********************************************************
功  能：指令发送
参  数: *da  指令   len: 指令长度
返回值: 无
***********************************************************/
void C1016_Send_Data(uint8_t *da, uint8_t len)
{
    rt_device_write(c1016_dev, 0, (void *) da, len);
}

/**********************************************************
功  能：清空指纹  范围：1~200
参  数: 无
返回值: 无
***********************************************************/
void C1016_cmd_Delete(void)
{
    C1016_Data_Verification(CMD_DEL_CHAR,sizeof(CMD_DEL_CHAR));
    C1016_Send_Data(CMD_DEL_CHAR,sizeof(CMD_DEL_CHAR));
}

/**********************************************************
功  能：将暂存在 ImageBuffer 中的指纹图像生成模板数据，
         并保存于指定编号的 Ram Buffer 中
参  数: 无
返回值: 无
***********************************************************/
void C1016_cmd_Generate(uint16_t RamBufferx)
{
    CMD_GENERATE[8] = RamBufferx & 0xFF;
    CMD_GENERATE[9] = (RamBufferx >> 8) & 0xFF;
    C1016_Data_Verification(CMD_GENERATE,sizeof(CMD_GENERATE));
    C1016_Send_Data(CMD_GENERATE,sizeof(CMD_GENERATE));
}




void C1016_SetColor(C1016_COLOR_Struct *p)
{
    uint8_t pData1[] = {0x55,0xaa,0x00,0x00,0x24,0x00,0x04,0x00,0x03,0x02,0x00,0x00,0x00,
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2C,0x01
                       };
    pData1[8] = p->color_mode;
    pData1[9] = p->color_start_value;
    pData1[10] = p->color_end_value;
    pData1[11] = p->color_loop;
    C1016_Data_Verification(pData1,26);     //校验
    C1016_Send_Data(pData1, 26);
}



/**
 * 检测ID是否被注册
 *参数：*rad 被判断数据指针
 *参数：len 数据长度
 *
 *返回：0 ID已经注册
 *返回：1 ID可以注册
 * **/
uint16_t C1016_Check_id_Registration(uint8_t *rad, int len)
{
    uint16_t SRC_CMD;
    uint16_t Verif;
    uint16_t refl;

    refl = 0;
    if(c1016_rx_flag == 1)
    {
        if(len >= 26)                                               //判断数据长度
        {
            SRC_CMD = (rad[5] << 8) + rad[4];                       //得到指令
            Verif = (rad[25] << 8) + rad[24];                       //得到校验

            if((rad[0] == 0xAA) && (rad[1] == 0x55))                //判断帧头
            {
                if(C1016_Get_Verification(rad,len) == Verif)        //判断校验
                {
                    if(SRC_CMD == 0x0046)                           //判断指令
                    {
                        if(rad[10] == 0x01)                         //ID已经注册
                        {
                            refl = 0;
                        } else {                                    //ID可以注册
                            refl = 1;
                        }
                    }
                    else if(SRC_CMD == 0x0020)
                    {
                        refl = 1;
                    }
                    else if(SRC_CMD == 0x0060)
                    {
                        refl = 1;
                    }
                    else if(SRC_CMD == 0x0040)
                    {
                        refl = 1;
                    }
                    else if(SRC_CMD == 0x0061)
                    {
                        refl = 1;
                    }
                    else if(SRC_CMD == 0x0063)
                    {
                        refl = ((rad[11] << 8) + rad[10]);
                    }
                    else
                    {
                        refl = 0;
                    }
                }
                else
                {
                    refl = 0;
                }
            }
            else
            {
                refl = 0;
            }
        }
        c1016_rx_flag = 0;
//              memset(c1016_buff,0,sizeof(c1016_buff));
    }
    return refl;
}




unsigned int gt_get_sub(unsigned int c)
{
    if(c > rt_tick_get())
        c -= rt_tick_get();
    else
        c = 0;
    return c;
}

/**
 * C1016_Enroll 注册
 * 注册流程
 *  1、判断注册ID是否存在（CMD_GET_STATU）
 *  2、采集指纹图像（CMD_GET_IMAGE）
 *  3、转化成特征模板（CMD_GENERATE）
 *  4、融合指纹模板（CMD_MERGE）
 *  5、保存指纹模板（CMD_STORE_CHAR）
 *
 *
 * **/
uint16_t setidnnumber;
void C1016_Enroll(uint16_t id_number)
{
    unsigned int c1016_enroll_time = 0;
    unsigned int c1016_enroll_time_out = 0;
    c1016_enroll_time_out =  rt_tick_get() + 2000;
    while(1)
    {
        rt_thread_mdelay(1);
        if(c1016_weakup_flag == 1)
        {
            CMD_GET_STATU[8] = id_number & 0xFF;                                //判断id_number 是否已经注册
            CMD_GET_STATU[9] = (id_number >> 8) & 0xFF;
            C1016_Data_Verification(CMD_GET_STATU,sizeof(CMD_GET_STATU));
            C1016_Send_Data(CMD_GET_STATU,26);
            c1016_enroll_time =  rt_tick_get() + 1000;
            while(!C1016_Check_id_Registration(c1016_buff,sizeof(c1016_buff)))//& (Sub_time(c1016_enroll_time))
            {
                if(gt_get_sub(c1016_enroll_time)==0)
                {
                    break;
                }
                rt_thread_mdelay(1);
            }


            C1016_Data_Verification(CMD_GET_IMAGE,sizeof(CMD_GET_IMAGE));       //采集指纹图像
            C1016_Send_Data(CMD_GET_IMAGE,sizeof(CMD_GET_IMAGE));
            c1016_enroll_time =  rt_tick_get() + 1000;
            while(!C1016_Check_id_Registration(c1016_buff,sizeof(c1016_buff)) )//& (Sub_time(c1016_enroll_time))
            {

                if(gt_get_sub(c1016_enroll_time)==0)
                {
                    break;
                }
                rt_thread_mdelay(1);
            }

            C1016_cmd_Generate(0);                                              //保存到缓存区0
            c1016_enroll_time =  rt_tick_get() + 1000;
            while(!C1016_Check_id_Registration(c1016_buff,sizeof(c1016_buff)))//& (Sub_time(c1016_enroll_time))
            {

                if(gt_get_sub(c1016_enroll_time)==0)
                {
                    break;
                }
                rt_thread_mdelay(1);
            }


            C1016_cmd_Generate(1);                                              //保存到缓存区1
            c1016_enroll_time =  rt_tick_get() + 1000;
            while(!C1016_Check_id_Registration(c1016_buff,sizeof(c1016_buff)))//& (Sub_time(c1016_enroll_time))
            {
                if(gt_get_sub(c1016_enroll_time)==0)
                {
                    break;
                }
                rt_thread_mdelay(1);
            }


            C1016_cmd_Generate(2);                                              //保存到缓存区2
            c1016_enroll_time =  rt_tick_get() + 1000;
            while(!C1016_Check_id_Registration(c1016_buff,sizeof(c1016_buff)))// & (Sub_time(c1016_enroll_time))
            {
                if(gt_get_sub(c1016_enroll_time)==0)
                {
                    break;
                }
                rt_thread_mdelay(1);
            }

            C1016_Data_Verification(CMD_MERGE,sizeof(CMD_MERGE));               //将暂存在 Ram Buffer 中的模板合并生成模板数据并后保存于指定的 Ram Buffer 中
            C1016_Send_Data(CMD_MERGE,sizeof(CMD_MERGE));
            c1016_enroll_time =  rt_tick_get() + 1000;
            while(!C1016_Check_id_Registration(c1016_buff,sizeof(c1016_buff)) )// & (Sub_time(c1016_enroll_time))
            {
                if(gt_get_sub(c1016_enroll_time)==0)
                {
                    break;
                }
                rt_thread_mdelay(1);
            }

            CMD_STORE_CHAR[8] = id_number & 0xFF;
            CMD_STORE_CHAR[9] = (id_number >> 8) & 0xFF;
            C1016_Data_Verification(CMD_STORE_CHAR,sizeof(CMD_STORE_CHAR));     //将暂存于指定 Ram Buffer 中的指纹模板保存于指定编号的模块指纹库中。
            C1016_Send_Data(CMD_STORE_CHAR,sizeof(CMD_STORE_CHAR));             //判断
            c1016_enroll_time =  rt_tick_get() + 1000;
            while(!C1016_Check_id_Registration(c1016_buff,sizeof(c1016_buff)))//& (Sub_time(c1016_enroll_time))
            {

                if(gt_get_sub(c1016_enroll_time)==0)
                {
                    break;
                }

                rt_thread_mdelay(1);
            }
            rt_kprintf("input is succend ID:%d\r\n",id_number);

            setidnnumber++;
            c1016_weakup_flag = 0;
            rt_thread_mdelay(500);
            break;
        }


        if(gt_get_sub(c1016_enroll_time_out)==0)
        {
            rt_kprintf("input time out\r\n");
            rt_thread_mdelay(500);
            rt_thread_mdelay(500);

            c1016_weakup_flag = 0;
            break;
        }
    }
}

//识别指纹
//uint16_t C1016_Identify(void)
//{
//    uint16_t Identify_ID = 0;
////    uint8_t ids[] = "000000";
//    unsigned int c1016_identify_time = 0;
//
//    if(c1016_weakup_flag == 1)
//    {
//
//        C1016_Data_Verification(CMD_GET_IMAGE,sizeof(CMD_GET_IMAGE));
//        C1016_Send_Data(CMD_GET_IMAGE,sizeof(CMD_GET_IMAGE));   //判断
//        while(!C1016_Check_id_Registration(c1016_buff,100))
//        {
//            rt_thread_mdelay(1);
//        }
//        C1016_cmd_Generate(0);
//        while(!C1016_Check_id_Registration(c1016_buff,100))
//        {
//            rt_thread_mdelay(1);
//        }
//
//
//        C1016_Data_Verification(CMD_SEARCH,sizeof(CMD_SEARCH));
//        C1016_Send_Data(CMD_SEARCH,sizeof(CMD_SEARCH)); //判断
//        c1016_identify_time =  rt_tick_get() + 2000;
//        memset(c1016_buff,0,sizeof(c1016_buff));
//        while(c1016_weakup_flag)
//        {
//
//            Identify_ID = C1016_Check_id_Registration(c1016_buff,100);
//            if(Identify_ID != 0)
//            {
//                c1016_weakup_flag = 0;
//                rt_kprintf("identfiy ID:%d\r\n",Identify_ID);
//                return Identify_ID;
//            } else
//            {
//
//            }
//
//            if(gt_get_sub(c1016_identify_time)==0)
//            {
//                c1016_weakup_flag = 0;
//                rt_kprintf("identfiy failed\r\n");
//                return -1;
//            }
//            rt_thread_mdelay(1);
//        }
//    }
//}



uint16_t C1016_Identify(void)
{
    uint16_t Identify_ID = 0;
    unsigned int c1016_identify_time = 0;

    if (c1016_weakup_flag == 1)
    {


        C1016_Data_Verification(CMD_GET_IMAGE, sizeof(CMD_GET_IMAGE));
        C1016_Send_Data(CMD_GET_IMAGE, sizeof(CMD_GET_IMAGE));

        while (!C1016_Check_id_Registration(c1016_buff, 100))
        {
            rt_thread_mdelay(1);
        }

        C1016_cmd_Generate(0);

        while (!C1016_Check_id_Registration(c1016_buff, 100))
        {
            rt_thread_mdelay(1);
        }

        C1016_Data_Verification(CMD_SEARCH, sizeof(CMD_SEARCH));
        C1016_Send_Data(CMD_SEARCH, sizeof(CMD_SEARCH));

        c1016_identify_time = rt_tick_get() + 5000; // 延长超时时间
        memset(c1016_buff, 0, sizeof(c1016_buff));

        while (c1016_weakup_flag)
        {
            Identify_ID = C1016_Check_id_Registration(c1016_buff, 100);
            if (Identify_ID != 0)
            {
                c1016_weakup_flag = 0;
                rt_kprintf("Identify Success! ID: %d\n", Identify_ID);
                return Identify_ID;
            }

            // 调试打印指纹数据
//            rt_kprintf("C1016 Buffer Data: ");
//            for (int i = 0; i < 10; i++)
//            {
//                rt_kprintf("%02X ", c1016_buff[i]);
//            }
//            rt_kprintf("\n");

            // 检查是否超时
            if (gt_get_sub(c1016_identify_time) == 0)
            {
                c1016_weakup_flag = 0;
                rt_kprintf("Identify Failed (Timeout)\n");
                return -1;
            }

            rt_thread_mdelay(1);
        }
    }


    return 0;
}




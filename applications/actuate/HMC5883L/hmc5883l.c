#include <rtdevice.h>
#include "rtthread.h"
#include "hmc5883l.h"
#include <math.h> // 引入数学库



void IIC_Init_hmc(void)
{

    rt_pin_mode(rt_pin_get("PB.2"), PIN_MODE_OUTPUT);  //44 "PB2"
    rt_pin_mode(rt_pin_get("PB.3"), PIN_MODE_OUTPUT);  //40 "PB3"       rt_pin_mode(rt_pin_get("PC.0"), PIN_MODE_OUTPUT);

    rt_pin_write(rt_pin_get("PB.2"),1);
    rt_pin_write(rt_pin_get("PB.3"),1);

}


void SDA_OUT_hmc(void)
{


    rt_pin_mode(rt_pin_get("PB.3"), PIN_MODE_OUTPUT);  //"PB3"
}


void SDA_IN_hmc(void)
{
    rt_pin_mode(rt_pin_get("PB.3"), PIN_MODE_INPUT);  //"PB3"
}


void IIC_Start_hmc(void)
{

    SDA_OUT_hmc();

    rt_pin_write(rt_pin_get("PB.3"),1);
    rt_pin_write(rt_pin_get("PB.2"),1);
    rt_hw_us_delay(2);
    rt_pin_write(rt_pin_get("PB.3"),0);
    rt_hw_us_delay(2);
    rt_pin_write(rt_pin_get("PB.2"),0);


}


void IIC_Stop_hmc(void)
{
    SDA_OUT_hmc();
    rt_pin_write(rt_pin_get("PB.2"),0);
    rt_pin_write(rt_pin_get("PB.3"),0);
    rt_hw_us_delay(2);
    rt_pin_write(rt_pin_get("PB.2"),1);
    rt_pin_write(rt_pin_get("PB.3"),1);
    rt_hw_us_delay(2);

}



//等待应答信号
unsigned char IIC_Wait_Ack_hmc(void)
{
    unsigned char ucErrTime = 0;
    SDA_IN_hmc();
    rt_pin_write(rt_pin_get("PB.3"),1);
    rt_hw_us_delay(5);
    rt_pin_write(rt_pin_get("PB.2"),1);
    rt_hw_us_delay(5);
    while (rt_pin_read(rt_pin_get("PB.3")))
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            IIC_Stop_hmc();

            return 1;
        }
    }
    rt_pin_write(rt_pin_get("PB.2"),0);

    return 0;
}

// 发送ACK信号
void IIC_Ack_hmc(void)
{
    rt_pin_write(rt_pin_get("PB.2"),0);
    SDA_OUT_hmc();
    rt_pin_write(rt_pin_get("PB.3"),0);
    rt_hw_us_delay(2);
    rt_pin_write(rt_pin_get("PB.2"),1);
    rt_hw_us_delay(2);
    rt_pin_write(rt_pin_get("PB.2"),0);
}

// 发送NACK信号
void IIC_NAck_hmc(void)
{
    rt_pin_write(rt_pin_get("PB.2"),0);
    SDA_OUT_hmc();
    rt_pin_write(rt_pin_get("PB.3"),1);
    rt_hw_us_delay(2);
    rt_pin_write(rt_pin_get("PB.2"),1);
    rt_hw_us_delay(2);
    rt_pin_write(rt_pin_get("PB.2"),0);
}

// 发送一个字节
void IIC_Send_Byte_hmc(unsigned char txd)
{
    unsigned char t;
    SDA_OUT_hmc();
    rt_pin_write(rt_pin_get("PB.2"),0);
    for (t = 0; t < 8; t++)
    {
        rt_pin_write(rt_pin_get("PB.3"),(txd & 0x80) >> 7);
        txd <<= 1;
        rt_hw_us_delay(2);
        rt_pin_write(rt_pin_get("PB.2"),1);
        rt_hw_us_delay(2);
        rt_pin_write(rt_pin_get("PB.2"),0);
        rt_hw_us_delay(2);
    }
}

// 读取一个字节
unsigned char IIC_Read_Byte_hmc(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN_hmc();
    for (i = 0; i < 8; i++)
    {
        rt_pin_write(rt_pin_get("PB.2"),0);
        rt_hw_us_delay(1);
        rt_pin_write(rt_pin_get("PB.2"),1);
        receive <<= 1;
        if (rt_pin_read(rt_pin_get("PB.3")))
            receive++;
        rt_hw_us_delay(1);
    }
    if (!ack)
        IIC_NAck_hmc();
    else
        IIC_Ack_hmc();
    return receive;
}



void hmc5883l_init(void)
{
    IIC_Init_hmc();


    // 配置寄存器 A: 8 次平均采样，15 Hz 输出速率，正常测量模式
    IIC_Start_hmc();
    IIC_Send_Byte_hmc(0x1E << 1);  // 发送写地址
    IIC_Wait_Ack_hmc();
    IIC_Send_Byte_hmc(0x00);       // 选择寄存器 A
    IIC_Wait_Ack_hmc();
    IIC_Send_Byte_hmc(0x70);       // 配置为 8 次平均采样, 15Hz, 正常测量
    IIC_Wait_Ack_hmc();
    IIC_Stop_hmc();

    // 配置寄存器 B: 设置增益
    IIC_Start_hmc();
    IIC_Send_Byte_hmc(0x1E << 1);  // 发送写地址
    IIC_Wait_Ack_hmc();
    IIC_Send_Byte_hmc(0x01);       // 选择寄存器 B
    IIC_Wait_Ack_hmc();
    IIC_Send_Byte_hmc(0xA0);       // 设置增益
    IIC_Wait_Ack_hmc();
    IIC_Stop_hmc();

    // 配置模式寄存器: 设置为连续测量模式
    IIC_Start_hmc();
    IIC_Send_Byte_hmc(0x1E << 1);  // 发送写地址
    IIC_Wait_Ack_hmc();
    IIC_Send_Byte_hmc(0x02);       // 选择模式寄存器
    IIC_Wait_Ack_hmc();
    IIC_Send_Byte_hmc(0x00);       // 设置为连续测量模式
    IIC_Wait_Ack_hmc();
    IIC_Stop_hmc();
}




void hmc5883l_read_xy(short *x, short *y)
{
    unsigned char data[6];

    IIC_Start_hmc();
    IIC_Send_Byte_hmc(0x1E << 1); // 发送写地址
    IIC_Wait_Ack_hmc();
    IIC_Send_Byte_hmc(0x03); // 数据寄存器起始地址（X轴高字节）
    IIC_Wait_Ack_hmc();
    IIC_Start_hmc();
    IIC_Send_Byte_hmc(0x1E << 1 | 1); // 发送读地址
    IIC_Wait_Ack_hmc();


    int i;
    for (i = 0; i < 6; i++) {
        data[i] = IIC_Read_Byte_hmc(i < 5); // 最后一个字节发送NACK
    }

    IIC_Stop_hmc();

     //将读取的数据组合成 16 位整数
    *x = (data[0] << 8) | data[1];
    *y = (data[4] << 8) | data[5];
    short z = (data[2] << 8) | data[3];  // Z 轴数据（调试用）

}


void I2C_Scan(void)
{
    unsigned char address;
    rt_kprintf("Starting I2C address scan...\n");

     //扫描可能的 I2C 地址范围(I2C 地址范围一般是0x03到0x77)
    for (address = 0x03; address < 0x78; address++) //
    {
        IIC_Start_hmc();
        IIC_Send_Byte_hmc(address << 1);    // 发送写地址
        if (IIC_Wait_Ack_hmc() == 0)
        {
            rt_kprintf("Device found at address 0x%02X.\n", address);
        }
        IIC_Stop_hmc();
    }

    rt_kprintf("I2C address scan finished.\n");
}


// 方位角计算函数
float calculate_heading(void)  //使用如果要串口打印记得传换成int类型打印！！！！
{
    short x = 0, y = 0;

    hmc5883l_read_xy(&x,&y);
    rt_kprintf("x = %d  y = %d\n",x,y);

    float heading = atan2(y, x) * 180 / M_PI; // 将弧度转为角度


    if (heading < 0)
    {
        heading += 360;//保证是正数
    }


    return heading;
}


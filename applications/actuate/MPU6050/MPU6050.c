/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-20     lzh28       the first version
 */


#include "MPU6050.h"
#include "inv_mpu.h"
#include <rtthread.h>
#include <rtdevice.h>

static struct rt_i2c_bus_device * MPU6050_IIC ;

//初始化IIC
void IIC_Init(void)
{
    // * 查找I2C总线设备，获取I2C总线设备句柄 */
    MPU6050_IIC = (struct rt_i2c_bus_device *) rt_device_find("i2c2");
    if(MPU6050_IIC == RT_NULL)
        rt_kprintf("MPU6050_IIC初始化失败");

}

//IIC连续写
//addr:器件地址
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
u8 mpu6050_write(rt_uint8_t addr, rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *databuf)
{
    rt_int8_t res = 0;
    struct rt_i2c_msg msgs;
    rt_uint8_t buf[50] = { 0 };
    buf[0] = reg;

    for (int i = 0; i < len; i++)
    {
        buf[i + 1] = databuf[i];
    }

    msgs.addr = addr; /* slave address */
    msgs.flags = RT_I2C_WR; /* write flag */
    msgs.buf = buf; /* Send data pointer */
    msgs.len = len + 1;

    if (rt_i2c_transfer(MPU6050_IIC, &msgs, 1) == 1)
    {
        res = RT_EOK;
    }
    else
    {
        res = -RT_ERROR;
    }
    return res;
}

//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 mpu6050_read(rt_uint8_t addr, rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *buf)
{
    rt_int8_t res = 0;
    struct rt_i2c_msg msgs[2];
    msgs[0].addr = addr; /* Slave address */
    msgs[0].flags = RT_I2C_WR; /* Write flag */
    msgs[0].buf = &reg; /* Slave register address */
    msgs[0].len = 1; /* Number of bytes sent */

    msgs[1].addr = addr; /* Slave address */
    msgs[1].flags = RT_I2C_RD; /* Read flag */
    msgs[1].buf = buf; /* Read data pointer */
    msgs[1].len = len; /* Number of bytes read */

    if (rt_i2c_transfer(MPU6050_IIC, msgs, 2) == 2)
    {
        res = RT_EOK;
    }
    else
    {
        res = -RT_ERROR;
    }
    return res;
}

void mpu6050_write_reg(u8 reg, u8 dat)
{
   mpu6050_write(MPU_ADDR,reg,1,&dat);
}

u8   mpu6050_read_reg (u8 reg)
{
     u8 dat;
   mpu6050_read(MPU_ADDR,reg,1,&dat);
     return dat;
}

//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
    mpu6050_write_reg(GYRO_CONFIG,fsr<<3);//设置陀螺仪满量程范围
    return 0;
}
//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
    mpu6050_write_reg(ACCEL_CONFIG,fsr<<3);//设置加速度传感器满量程范围
    return 0;
}
//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败
u8 MPU_Set_LPF(u16 lpf)
{
    u8 data=0;
    if(lpf>=188)data=1;
    else if(lpf>=98)data=2;
    else if(lpf>=42)data=3;
    else if(lpf>=20)data=4;
    else if(lpf>=10)data=5;
    else data=6;
    mpu6050_write_reg(MPU_CFG_REG,data);//设置数字低通滤波器
    return 0;
}
//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败
u8 MPU_Set_Rate(u16 rate)
{
    u8 data;
    if(rate>1000)rate=1000;
    if(rate<4)rate=4;
    data=1000/rate-1;
    mpu6050_write_reg(MPU_SAMPLE_RATE_REG,data);    //设置数字低通滤波器
    return MPU_Set_LPF(rate/2); //自动设置LPF为采样率的一半
}

void MPU6050_Init(void)
{
    u8 res;
    IIC_Init(); //初始化IIC总线
    mpu6050_write_reg(PWR_MGMT_1,0X80); //复位MPU6050
    rt_thread_mdelay(200);
    mpu6050_write_reg(PWR_MGMT_1,0X00); //唤醒MPU6050
    rt_thread_mdelay(100);
    MPU_Set_Gyro_Fsr(3);                    //陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                   //加速度传感器,±2g
    MPU_Set_Rate(200);                      //设置采样率50Hz
    mpu6050_write_reg(MPU_INT_EN_REG,0X00); //关闭所有中断
    mpu6050_write_reg(MPU_USER_CTRL_REG,0X00);  //I2C主模式关闭
    mpu6050_write_reg(MPU_FIFO_EN_REG,0X00);    //关闭FIFO
    mpu6050_write_reg(MPU_INTBP_CFG_REG,0X80);  //INT引脚低电平有效
    res=mpu6050_read_reg(MPU_DEVICE_ID_REG);
    if(res==MPU_ADDR)//器件ID正确
    {
        mpu6050_write_reg(PWR_MGMT_1,0X01); //设置CLKSEL,PLL X轴为参考
        mpu6050_write_reg(PWR_MGMT_2,0X00); //加速度与陀螺仪都工作
        MPU_Set_Rate(100);                      //设置采样率为50Hz
    }

}

//得到温度值
//返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(void)
{
    u8 buf[2];
    short raw;
    float temp;
    mpu6050_read(MPU_ADDR,TEMP_OUT_H,2,buf);
    raw=((u16)buf[0]<<8)|buf[1];
    temp=36.53+((double)raw)/340;
    return temp*100;
}
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;
    res=mpu6050_read(MPU_ADDR,GYRO_XOUT_H,6,buf);
    if(res==0)
    {
        *gx=((u16)buf[0]<<8)|buf[1];
        *gy=((u16)buf[2]<<8)|buf[3];
        *gz=((u16)buf[4]<<8)|buf[5];
    }
    return res;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;
    res=mpu6050_read(MPU_ADDR,ACCEL_XOUT_H,6,buf);
    if(res==0)
    {
        *ax=((u16)buf[0]<<8)|buf[1];
        *ay=((u16)buf[2]<<8)|buf[3];
        *az=((u16)buf[4]<<8)|buf[5];
    }
    return res;
}
void get_ms(unsigned long *time){

}


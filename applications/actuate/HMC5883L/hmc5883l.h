/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-20     lp666       the first version
 */
#ifndef APPLICATIONS_ACTUATE_HMC5883L_HMC5883L_H_
#define APPLICATIONS_ACTUATE_HMC5883L_HMC5883L_H_


#include <stdint.h>

// HMC5883L I2C 地址
#define HMC5883L_ADDRESS 0x1E
#define WRITE_ADDRESS (HMC5883L_ADDRESS << 1)
#define READ_ADDRESS ((HMC5883L_ADDRESS << 1) | 1)


#define M_PI 3.14159265358979323846  // 定义圆周率



// HMC5883L 操作函数声明

/**
 * @brief 初始化 HMC5883L 传感器
 */
void hmc5883l_init(void);

/**
 * @brief 读取 HMC5883L 的原始数据
 *
 * @param GaX 指向存储 X 轴高斯值的指针
 * @param GaY 指向存储 Y 轴高斯值的指针
 */
void hmc5883l_read_xy(short *x, short *y);

void I2C_Scan(void);
float calculate_heading(void);

#endif /* APPLICATIONS_ACTUATE_HMC5883L_HMC5883L_H_ */

/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-24     lzh28       the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "flame.h"
static rt_base_t flame_pin = RT_NULL;
static rt_tick_t TICK_TIM = 0;
static void GPIO_IRQHandler(void *args)
{
    if(rt_pin_read(flame_pin)==PIN_LOW){
        TICK_TIM = rt_tick_get();
    }
}
void flame_Init(void)
{
    flame_pin = rt_pin_get("PC.8");  //输出
    rt_pin_mode(flame_pin, PIN_MODE_INPUT);
    /* 下降沿触发 */
    rt_pin_attach_irq(flame_pin, PIN_IRQ_MODE_FALLING, GPIO_IRQHandler, RT_NULL);
    rt_pin_irq_enable(flame_pin, PIN_IRQ_ENABLE);
}
//返回RT_TRUE有火，返回RT_FALSE没火
rt_bool_t get_flame_open_tim(){
    if(rt_pin_read(flame_pin)==PIN_LOW){
        TICK_TIM = rt_tick_get();
    }
    if(TICK_TIM!=0&&TICK_TIM+2000>rt_tick_get()){
        return RT_TRUE;
    }
    return RT_FALSE;
}

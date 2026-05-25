#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "beep.h"
#define BEEP_PIN GET_PIN(C,12)//"PC.12"

static rt_base_t beep_pin = RT_NULL;
static void Smg_init(void){
    beep_pin = rt_pin_get("PC.12");  //输出
    rt_pin_mode(beep_pin, PIN_MODE_OUTPUT);
}


void BEEP_Control(rt_bool_t state)
{
    if(beep_pin == RT_NULL){
        Smg_init();
        if(beep_pin == RT_NULL)
            return ;
    }

    if(state == RT_TRUE){
        rt_pin_write(beep_pin, PIN_HIGH);
    }else{
        rt_pin_write(beep_pin, PIN_LOW);
    }
}

static rt_device_t beep_dev = RT_NULL;
static rt_hwtimerval_t timeout_s;

unsigned short music[] = {955, 851, 758, 715, 637, 568, 506,247,220,196,175,165,147,131,1911};
//{
//    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
//    523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
//   1046, 1109, 1175, 1245, 1318, 1397, 1480, 1568, 1661, 1760, 1865, 1976
//};




rt_err_t beep_timer_callback(rt_device_t dev, rt_size_t size)
{
    static uint8_t beep_state = 0;
    rt_pin_write(BEEP_PIN, beep_state);
    beep_state = !beep_state;


    return 0;
}

// 初始化蜂鸣器
void beep_init(void)
{
    rt_err_t ret = RT_EOK;
    rt_hwtimer_mode_t mode;
//    rt_uint32_t freq = 84000000;  // 定时器计数频率

    // 初始化蜂鸣器引脚为输出
    rt_pin_mode(BEEP_PIN, PIN_MODE_OUTPUT);  // 设置为输出模式

    // 查找定时器设备
    beep_dev = rt_device_find("timer11");
    if (beep_dev == RT_NULL)
    {
        rt_kprintf("没有找到 timer11 设备!\n");
        return;
    }

    // 打开定时器设备
    ret = rt_device_open(beep_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("打开定时器设备失败: %d\n", ret);
        return;
    }

    // 设置定时器模式为周期性模式
    mode = HWTIMER_MODE_PERIOD;
    ret = rt_device_control(beep_dev, HWTIMER_CTRL_MODE_SET, &mode);
    if (ret != RT_EOK)
    {
        rt_kprintf("设置定时器模式失败: %d\n", ret);
        return;
    }

//    // 设置计数频率
//    ret = rt_device_control(beep_dev, HWTIMER_CTRL_FREQ_SET, &freq);
//    if (ret != RT_EOK)
//    {
//        rt_kprintf("设置计数频率失败: %d\n", ret);
//        return;
//    }



    // 设置定时器超时回调
    rt_device_set_rx_indicate(beep_dev, beep_timer_callback);

    rt_kprintf("蜂鸣器初始化完成！\n");

    // 设置默认定时器周期
//    timeout_s.sec = 0;
//    timeout_s.usec = M1/2;
//    rt_device_write(beep_dev, 0, &timeout_s, sizeof(timeout_s));
//    rt_thread_mdelay(2000);
//    timeout_s.usec = M2/2;
//    rt_device_write(beep_dev, 0, &timeout_s, sizeof(timeout_s));
//    rt_thread_mdelay(2000);
//    timeout_s.usec = M3/2;
//    rt_device_write(beep_dev, 0, &timeout_s, sizeof(timeout_s));
//    rt_thread_mdelay(2000);
//    timeout_s.usec = M4/2;
//    rt_device_write(beep_dev, 0, &timeout_s, sizeof(timeout_s));
//    rt_thread_mdelay(2000);
//    timeout_s.usec = M5/2;
//    rt_device_write(beep_dev, 0, &timeout_s, sizeof(timeout_s));

}

void beep_music(int num,long ms)
{

    if(num==0 && ms==0)
    {

        timeout_s.sec = 0;
        timeout_s.usec = 0;
        rt_device_write(beep_dev,0,&timeout_s,sizeof(timeout_s));
        rt_pin_write(rt_pin_get("PC.12"), PIN_LOW);
//        rt_thread_mdelay(500);
    }
    else {
        timeout_s.sec = 0;
        timeout_s.usec = music[num-1];
        rt_device_write(beep_dev,0,&timeout_s,sizeof(timeout_s));
        rt_thread_mdelay(ms);
    }

}





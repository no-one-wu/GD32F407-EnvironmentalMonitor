#include "rtthread.h"
#include "paj7620u2.h"
#include "rtdevice.h"
#include "paj7620u2_cfg.h"

static void paj7620u2_selectBank(bank_e bank);//选择PAJ7620U2 BANK区域
static unsigned char  paj7620u2_wakeup(void);//PAJ7620U2唤醒


//PAJ7620U2初始化
//返回值：0:失败 1:成功
unsigned char  paj7620u2_init()
{

    unsigned int i;

    unsigned char status;
    GS_i2c_init();//传感器I2C初始化
    status = paj7620u2_wakeup();        //唤醒PAJ7620U2
    if(!status){

        return 0;
    }
    paj7620u2_selectBank(BANK0);    //进入BANK0寄存器区域
    for(i=0;i<INIT_SIZE;i++)                        //初始化模块
    {
        GS_Write_Byte(init_Array[i][0], init_Array[i][1]);//初始化PAJ7620U2
    }
    paj7620u2_selectBank(BANK0);//切换回BANK0寄存器区域
    return 1;
}


void Gesture_Init(void)
{
    unsigned char  i;
    paj7620u2_selectBank(BANK0);//进入BANK0寄存器区域
    for(i=0;i<GESTURE_SIZE;i++)
    {
        GS_Write_Byte(gesture_arry[i][0],gesture_arry[i][1]);//手势识别模式初始化
    }
    paj7620u2_selectBank(BANK0);//切换回BANK0寄存器区域

    //gesture = (GestureData *)malloc(sizeof(GestureData));
    //if(NULL == gesture){

        //rt_kprintf("Error: struct \"GESTURE_DATA\" malloc failed\r\n");
    //}
    //memset(gesture, 0, sizeof(GestureData));

}

//选择PAJ7620U2 BANK区域
void paj7620u2_selectBank(bank_e bank)
{
    switch(bank)
    {
        case BANK0: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK0);break;//BANK0寄存器区域
        case BANK1: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK1);break;//BANK1寄存器区域
    }
}

//PAJ7620U2唤醒
unsigned char  paj7620u2_wakeup()
{
    unsigned char  data=0x0a;
    GS_WakeUp();//唤醒PAJ7620U2
    rt_thread_mdelay(5);//唤醒时间>400us
    GS_WakeUp();//唤醒PAJ7620U2
    rt_thread_mdelay(5);//唤醒时间>400us
    paj7620u2_selectBank(BANK0);//进入BANK0寄存器区域

    data = GS_Read_Byte(0x00);//读取状态
    if(data!=0x20)
    {
        return 0; //唤醒失败
    }
    return 1;
}

void  PAJ7620_Init(void)
{
    while(!paj7620u2_init())//PAJ7620U2传感器初始化
    {
        rt_kprintf("PAJ7620U2_B Error!!!\r\n");
        rt_thread_mdelay(1000);
    }
    rt_thread_mdelay(1000);
    Gesture_Init();
    rt_kprintf("PAJ7620U2 OK\r\n");
}


void PAJ7620_Use(void)
{
    unsigned char gesture_result = GS_Read_Byte(PAJ_GET_INT_FLAG1);

    if (gesture_result & GES_UP) {
        rt_kprintf("指令: 向上\n");
    } else if (gesture_result & GES_DOWN) {
        rt_kprintf("指令: 向下\n");
    } else if (gesture_result & GES_LEFT) {
        rt_kprintf("指令: 向左\n");
    } else if (gesture_result & GES_RIGHT) {
        rt_kprintf("指令: 向右\n");
    } else if (gesture_result & GES_FORWARD) {
        rt_kprintf("指令: 向前\n");
    } else if (gesture_result & GES_BACKWARD) {
        rt_kprintf("指令: 向后\n");
    } else if (gesture_result & GES_CLOCKWISE) {
        rt_kprintf("指令: 顺时针\n");
    } else if (gesture_result & GES_ANTI_CLOCKWISE) {
        rt_kprintf("指令: 逆时针\n");
    } else if (gesture_result & GES_WAVE) {
        rt_kprintf("指令: 挥动\n");
    }
}

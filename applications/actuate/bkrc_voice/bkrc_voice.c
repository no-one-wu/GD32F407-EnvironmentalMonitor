
#include "rtthread.h"
#include "bkrc_voice.h"
#include <rtdevice.h>
#include <rtdbg.h>

unsigned char voice_buff[100];
//0-9,万，千，百，十
static short  stringdata[]={24,25,26,27,28,29,30,31,32,33,34,35,36,37};
static char rouse[] = {0xF4, 0x06, 0x00, 0xFB};
uint8_t k[4] = {0xF4,0xF5,0,0xFB};
volatile rt_uint8_t bkrc_voice_data[4] = {0xF4, 0x00, 0x00, 0xFB};

//播报命令词
        //@唤醒词=小创小创
        //@命令词=美好生活
        //@命令词=秀丽山河
        //@命令词=追逐梦想
        //@命令词=扬帆启航
        //@命令词=齐头并进
        //@命令词=富强路站
        //@命令词=民主路站
        //@命令词=文明路站
        //@命令词=和谐路站
        //@命令词=爱国路站
        //@命令词=敬业路站
        //@命令词=友善路站
        //@命令词=技能成才
        //@命令词=匠心筑梦
        //@命令词=逐梦扬威
        //@命令词=技行天下
        //@命令词=展行业百技
        //@命令词=树人才新观
        //@命令词=打开红灯
        //@命令词=打开绿灯
        //@命令词=打开蓝灯
        //@命令词=关闭灯光



rt_device_t voice_dev;

volatile uint8_t voice_rx_flag = 0; //接收标志

rt_err_t voice_rx_func_cb(rt_device_t dev, rt_size_t size)
{   //串口回调函数
    static uint8_t ch = 0;
    static uint8_t index = 0;

    if (dev == voice_dev && rt_device_read(voice_dev, 0, &ch, 1))
    {
        voice_buff[index] = ch;
        if (voice_buff[index] == 0xFB)
        {   //当检测到0xFB帧尾
            voice_rx_flag = 1;
            index = 0;
        }
        else
        {
            index++;
            index %= 100;
        }
    }
    return 0;
}


void Voice_Config_Init(void)
{
        /* step1：查找串口设备 */
        voice_dev = rt_device_find(VOICE_NAME);
        if (voice_dev == RT_NULL)
        {
            rt_kprintf("no find %s\n",VOICE_NAME);
            return;
        }

        /* step2：修改串口配置参数 */
        static struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; /* 初始化配置参数 */
        /* step2：修改串口配置参数 */
        config.baud_rate = BAUD_RATE_9600;        //修改波特率为115200
        config.data_bits = DATA_BITS_8;             //数据位 8
        config.stop_bits = STOP_BITS_1;             //停止位 1
        config.bufsz = 100;                           //修改缓冲区 buff size 为 128
        config.parity = PARITY_NONE;                //无奇偶校验位

        /* step3：控制串口设备。通过控制接口传入命令控制字，与控制参数 */
        if (rt_device_control(voice_dev, RT_DEVICE_CTRL_CONFIG, &config) != RT_EOK)
        {
            voice_dev = RT_NULL;
            rt_kprintf("config failed %s\n",VOICE_NAME);
            return;
        }
        /* step4：打开串口设备。以中断接收及轮询发送模式打开串口设备 */
        if (rt_device_open(voice_dev, RT_DEVICE_OFLAG_RDWR |RT_DEVICE_FLAG_INT_TX|RT_DEVICE_FLAG_INT_RX) != RT_EOK)
        {
            voice_dev = RT_NULL;
            rt_kprintf("open failed %s\n",VOICE_NAME);
            return;
        }
        //设置接收回调函数
        rt_device_set_rx_indicate(voice_dev,voice_rx_func_cb);
}


void voice_rouse(void){    //唤醒语音模块
     rt_device_write(voice_dev, 0, (void *) rouse, sizeof(rouse));
}
//播报词列表 instruct:
        //@0=我在
        //@1=识别成功，美好生活
        //@2=识别成功，秀丽山河
        //@3=识别成功，追逐梦想
        //@4=识别成功，扬帆启航
        //@5=识别成功，齐头并进
        //@6=识别成功，富强路站
        //@7=识别成功，民主路站
        //@8=识别成功，文明路站
        //@9=识别成功，和谐路站
        //@10=识别成功，爱国路站
        //@11=识别成功，敬业路站
        //@12=识别成功，友善路站
        //@13=识别成功，技能成才
        //@14=识别成功，匠心筑梦
        //@15=识别成功，逐梦扬威
        //@16=识别成功，技行天下
        //@17=识别成功，展行业百技
        //@18=识别成功，树人才新观
        //@19=好的，已打开红灯
        //@20=好的，已打开绿灯
        //@21=好的，已打开蓝灯
        //@22=好的，已关闭所有灯光
        //@98=欢迎使用百科荣创AI语音识别系统
        //@99=小创竭诚为您服务
void voice_broadcast(unsigned char instruct)
{ //播报内容
   uint8_t cmd[4] = {0xF4,0xF5,instruct,0xFB};
   rt_device_write(voice_dev, 0, cmd, 4);
}

void voice_figure( int instruct){
    int data[5] = {0},i=0,j = 0;
    while (instruct!=0)
    {
        data[i]=instruct%10;
        instruct/=10;
        i++;
    }
    for(j = i ;j>0;j--){
        if (data[j-1] == 0 && j == 1) break;
        k[2] = stringdata[data[j-1]];
        rt_device_write(voice_dev, 0, k, 4);
        rt_thread_mdelay(700);

        if(data[j-1] == 0) continue;
        switch(j){
        case 5:
            k[2] = stringdata[10];
            rt_device_write(voice_dev, 0, k, 4);
            break;  //万
        case 4:
            k[2] = stringdata[11];
            rt_device_write(voice_dev, 0, k, 4);
            break;  //千
        case 3:
            k[2] = stringdata[12];
            rt_device_write(voice_dev, 0, k, 4);
            break;  //百
        case 2:
            k[2] = stringdata[13];
            rt_device_write(voice_dev, 0, k, 4);
            break;  //十
        }
        rt_thread_mdelay(700);
    }
}



/**************************************************
Voice_Drive
函数说明：执行对应词条的任务（语音识别）
输入参数：   无
返 回 值：  语音词条ID    词条内容
***************************************************/

char Voice_Drive(void)
{
    char ID_flag = -1;

    if (voice_rx_flag && voice_buff[0] == 0xF4 && voice_buff[3] == 0xFB)          // 自定义数据帧接收完毕
    {
        rt_kprintf("voice id:%#X\n",voice_buff[2]);
        rt_kprintf("voice cmd:%#X %#X %#X %#X %#X %#X %#X\n",voice_buff[0],voice_buff[1],voice_buff[2],voice_buff[3],voice_buff[4],voice_buff[5],voice_buff[6]);

        if (voice_buff[1] == 0x06)
        {  //播报
            switch (voice_buff[2])
            {
            case 0x01:

                ID_flag = 1;
                break;

            case 0x02:
                ID_flag = 2;
                break;

            case 0x03:
                ID_flag = 3 ;
                break;

            case 0x04:
                ID_flag = 4;
                break;

            case 0x05:
                ID_flag = 5;
                break;

            case 0x06:
                ID_flag = 6;
                break;
            case 0x07:
                ID_flag = 7;
                break;

            default  :
                ID_flag = 0;
                break;
            }
        }
        voice_buff[0] = 0;
        voice_buff[1] = 0;
        voice_buff[2] = 0;
        voice_buff[3] = 0;
        //清除标志
        voice_rx_flag = 0;
    }
    return ID_flag;
}




#ifndef __OLED_H_
#define __OLED_H_

#include <rtthread.h>
#include <board.h>
#include <drv_common.h>
#define I2C_OLED        0x78
#define IIC_OLED_CMD    0x00    //写命令
#define IIC_OLED_DATA   0x40    //写数据


void OLED_Init(void);
void OLED_Display_Off(void);
void OLED_Display_On(void);
void OLED_Clear(void);
void OLED_Refresh(void);
void OLED_DrawPoint(u8 x, u8 y, u8 t);
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode);
void OLED_DrawCircle(u8 x, u8 y, u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
void OLED_ShowNum(u8 x,u8 y,uint32_t num,u8 len,u8 size1,u8 mode);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_Printf(u8 x,u8 y,u8 size1,u8 mode,const char *fmt,...);

#define       LCD_LINEDIR_HORIZONTAL  0
#define       LCD_LINEDIR_VERTICAL    1
void lcd_line_draw(uint16_t xpos, uint16_t ypos, uint16_t length, uint8_t line_direction);
void lcd_rectangle_draw(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height);
void lcd_circle_draw(uint16_t xpos, uint16_t ypos, uint16_t radius);
void lcd_ellipse_draw(uint16_t xpos, uint16_t ypos, uint16_t axis1, uint16_t axis2);
void lcd_rectangle_fill(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height);
#endif


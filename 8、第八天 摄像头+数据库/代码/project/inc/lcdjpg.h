#ifndef __LCDJPG_H__
#define __LCDJPG_H__

#define LCD_WIDTH  			800
#define LCD_HEIGHT 			480
#define FB_SIZE				(LCD_WIDTH * LCD_HEIGHT * 4)


/* video_chat.c 画中画显示的坐标 */
extern volatile int g_jpg_in_jpg_x;
extern volatile int g_jpg_in_jpg_y;

void lcd_draw_point(unsigned int x,unsigned int y, unsigned int color);
int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size,unsigned int jpg_half);
#endif
#ifndef __SHOWBMP_H__
#define __SHOWBMP_H__

#include <stdio.h>
#include <sys/types.h>//open
#include <sys/stat.h>//open
#include <fcntl.h>//open
#include <unistd.h>//read   write   close
#include <string.h>//strlen   memset
#include <sys/mman.h>//mmap

#define WIGHT 800
#define HIGHT 480
#define LCD_MAP_SIZE WIGHT*HIGHT*4

void lcd_draw_point_bmp(int x,int y,int color);
/***************************
 * 参数：xx        --->显示的起点横坐标
 *       yy        --->显示的起点位置纵坐标
 *       path      --->要显示的bmp图片路径
 * ***********************/
int show_bmp(int xx,int yy,char *path);
#endif
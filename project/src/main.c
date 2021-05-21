#include <stdio.h>
#include "rfid.h"
#include "get_xy.h"
#include "lcdjpg.h"
#include <pthread.h>
#include "font.h"
#include <time.h>

int x,y=0;
int flag = 0;
time_t tim;
struct tm *timeinfo;
void *erzi(void *arg)
{
	while(1)
	{
		get_x_y(&x,&y);
		x=0;
		y=0;
	}
}

void *erzi1(void *arg)
{
	while(1)
	{
		char time1[50] = {0};
		time(&tim);
		timeinfo = localtime(&tim);//转化为当前系统时间
		Clean_Area(0,430,525,50,0x00ffffff);
		sprintf(time1,"%d/%d/%d/%d  %02d:%02d:%02d",
			timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_wday,
			timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		Display_characterX(0,430,time1,0x00ff0000,3);
		sleep(1);
	}
}

int main(int argc, char const *argv[])
{
	//初始化
	Init_Font();
	//显示主界面
	loop2:lcd_draw_jpg(0,0,"../pictures/background1.jpg",NULL,0,0);
	lcd_draw_jpg(0,0,"../pictures/stop.jpg",NULL,0,0);
	lcd_draw_jpg(125,200,"../pictures/charge.jpg",NULL,0,0);
	lcd_draw_jpg(325,200,"../pictures/consume.jpg",NULL,0,0);
	lcd_draw_jpg(525,200,"../pictures/find.jpg",NULL,0,0);
	
	pthread_t pid;
	pthread_create(&pid,NULL,erzi,NULL);
	pthread_create(&pid,NULL,erzi1,NULL);
	while(1)
	{
		if((x>125&&x<275)&&(y>200&&y<350))//充值的按钮
		{
			Rfid_Recharge();	
		}
		if((x>325&&x<475)&&(y>200&&y<350))//消费的按钮
		{
			Rfid_Consumption();
		}
		if((x>525&&x<675)&&(y>200&&y<350))
		{
			Rfid_find();
			goto loop2;
		}
		
	}
	//解除
	UnInit_Font();
	return 0;
}
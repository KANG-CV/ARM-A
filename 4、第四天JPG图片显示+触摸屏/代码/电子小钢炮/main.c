#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lcdjpg.h"
#include "get_xy.h"

int x,y;
void *erzi(void *arg)
{
	//���߳�
	while(1)
	{
		get_x_y(&x,&y);
		x=0;
		y=0;
	}
}
int main(void)
{
	int i=0;
	lcd_draw_jpg(0,0,"./background.jpg",NULL,0,0);
	lcd_draw_jpg(0,0,"./bar.jpg",NULL,0,0);
	for(i=60;i<=672;i+=68)  
	{
		lcd_draw_jpg(i,90,"./key_off.jpg",NULL,0,0);	
	}
	lcd_draw_jpg(456,412,"./logo.jpg",NULL,0,0);
	pthread_t pid;
	pthread_create(&pid, NULL, erzi, NULL);
	//���߳�
	while(1)
	{
		if((x>60&&x<128)&&(y>90&&y<367))
		{
			system("killall -9 madplay");
			//��ʾ��ɫ��ť
			lcd_draw_jpg(60,90,"./key_on.jpg",NULL,0,0);
			//���Ŷ�Ӧ����Ƶ
			system("madplay d1.mp3 &");//&��̨����
		}
		if((x>128&&x<196)&&(y>90&&y<367))
		{
			system("killall -9 madplay");
			//��ʾ��ɫ��ť
			lcd_draw_jpg(128,90,"./key_on.jpg",NULL,0,0);
			//���Ŷ�Ӧ����Ƶ
			system("madplay d2.mp3 &");//&��̨����
		}
	}
	return 0;
}
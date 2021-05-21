#include <stdio.h>
#include "rfid.h"
#include "get_xy.h"
#include "lcdjpg.h"
#include <pthread.h>

int x,y=0;
void *erzi(void *arg)
{
	while(1)
	{
		get_x_y(&x,&y);
		x=0;
		y=0;
	}
}

int main(int argc, char const *argv[])
{
	//显示主界面
	lcd_draw_jpg(0,0,"./zjm.jpg",NULL,0,0);
	pthread_t pid;
	pthread_create(&pid,NULL,erzi,NULL);
	while(1)
	{
		if((x>45&&x<330)&&(y>224&&y<368))//充值的按钮
		{
			Rfid_Recharge();	
		}
		/*if()//消费的按钮
		{
			Rfid_Consumption();
		}*/
		if((x>700&&x<800)&&(y>400&&y<480))//摄像头的按钮
		{
			do_camera();	
		}
		if((x>0&&x<80)&&(y>450&&y<480))//查看抓拍的图片
		{
			lcd_draw_jpg(0,0,"./0.jpg",NULL,0,0);	
		}
	}
	return 0;
}
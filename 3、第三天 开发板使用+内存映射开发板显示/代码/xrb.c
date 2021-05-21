#include <stdio.h>
#include <sys/types.h>//open
#include <sys/stat.h>//open
#include <fcntl.h>//open
#include <unistd.h>//read  write  close
#include <sys/mman.h>//mmap

int main(void)
{
	int x,y=0;
	//1.打开屏幕文件
	int lcd_fd = open("/dev/fb0",O_RDWR);
	if(lcd_fd < 0)
	{
		printf("open lcd failed!\n");
		return -1;
	}
	//2.用 mmap 将颜色写到屏幕
	int *mmap_addr = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd_fd,0);
	int red = 0x00FF0000;//A R G B
	int white = 0x00FFFFFF;//A R G B
	int black = 0x00000000;//A R G B
	int xa=400,ya=240,xb=2,yb=2;
	int xa1=240,ya1=60,xb1=3,yb1=3;
	int flag;
	while(1)
	{
		for(y=0;y<480;y++)
		{
			for(x=0;x<800;x++)
			{
				*(mmap_addr+y*800+x) = white;
				if((x-xa)*(x-xa)+(y-ya)*(y-ya)<=30*30)//圆心(400,240)  半径 100
				{
					*(mmap_addr+y*800+x) = red;
				}	
				if((x-xa1)*(x-xa1)+(y-ya1)*(y-ya1)<=50*50)//圆心(400,240)  半径 100
				{
					*(mmap_addr+y*800+x) = black;
				}	
			}
		}
		xa=xa+xb;
		ya=ya+yb;
		xa1=xa1+xb1;
		ya1=ya1+yb1;
		if(xa==30 || xa==770)
		{
			xb=0-xb;
		}
		if(ya==30 || ya==450)
		{
			yb=0-yb;
		}
		if(xa1<=50 || xa1>=750)
		{
			xb1=0-xb1;
		}
		if(ya1<=50 || ya1>=430)
		{
			yb1=0-yb1;
		}
		if((xa-xa1)*(xa-xa1)+(ya-ya1)*(ya-ya1)<=80*80)//两个圆撞
		{
			xb=0-xb;
			yb=0-yb;
			xb1=0-xb1;
			yb1=0-yb1;
			flag = red;
			red = black;
			black = flag;
		}
	}
	
	//3.关闭屏幕
	close(lcd_fd);
	return 0;
}

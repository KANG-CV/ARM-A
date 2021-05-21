#include "showbmp.h"

int *mmap_addr =NULL;

void lcd_draw_point_bmp(int x,int y,int color)
{
	*(mmap_addr+y*WIGHT+x)=color;
}

int show_bmp(int xx,int yy,char *path)
{
	int lcd_fd = open("/dev/fb0",O_RDWR);
	if(lcd_fd < 0)
	{
		printf("open lcd failed!\n");
		return -1;
	}
	mmap_addr= mmap(NULL,//写NULL，由内核帮我们选择地址
	                      LCD_MAP_SIZE,//映射大小
						  PROT_READ | PROT_WRITE,//可读可写
						  MAP_SHARED,//分享
						  lcd_fd,//lcd文件描述符
						  0//偏移量，一般给
						);
	//1.目标文件
	unsigned int bmp_fd = open(path,O_RDONLY);
	if(bmp_fd < 0)
	{
		printf("open bmp failed!\n");
		return -1;
	}
	unsigned int l,w;
	lseek(bmp_fd,18,SEEK_SET);
	read(bmp_fd,&l,4);
	read(bmp_fd,&w,4);
	//printf("bmp_l=%d  bmp_w=%d\n",l,w);
	//2.读取图片数据
	lseek(bmp_fd,54,SEEK_SET);//偏移54个字节
	unsigned char bmp_buf[l*w*3];
	read(bmp_fd,bmp_buf,l*w*3);
	close(bmp_fd);
	//3.写入屏幕
	unsigned int x,y,i=0;
	unsigned char r,g,b;
	unsigned int color;
	for(y=0;y<w;y++)
	{
		for(x=0;x<l;x++)
		{
			b=bmp_buf[i++];
			g=bmp_buf[i++];
			r=bmp_buf[i++];
			color = b | g << 8 | r << 16;
			//lcd_draw_point_bmp(x+xx,y+yy,color);//倒着显示
			lcd_draw_point_bmp(x+xx,w-1-y+yy,color);//正常显示
		}	
	}
	//4.解除内存映射并且关闭文件
	close(lcd_fd);
	munmap(mmap_addr,LCD_MAP_SIZE);
	return 0;
}

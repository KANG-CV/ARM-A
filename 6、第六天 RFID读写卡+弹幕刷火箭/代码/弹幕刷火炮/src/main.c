#include <stdio.h>
#include "font.h"
#include "showbmp.h"

int main(int argc, char *argv[])
{
	int i=0;
	//初始化
	Init_Font();
	while(1)
	{
		for(i=10;i>=0;i--)
		{
			show_bmp(0,0,"./dm.bmp");
			Display_characterX(i*8,10,"行车不规范，亲人两行泪",0x00FF0000,3);
			sleep(0.5);
		}	
	}
	//解除
	UnInit_Font();
	return 0;
}
#include <stdio.h>
#include "font.h"
#include "showbmp.h"

int main(int argc, char *argv[])
{
	int i=0;
	//��ʼ��
	Init_Font();
	while(1)
	{
		for(i=10;i>=0;i--)
		{
			show_bmp(0,0,"./dm.bmp");
			Display_characterX(i*8,10,"�г����淶������������",0x00FF0000,3);
			sleep(0.5);
		}	
	}
	//���
	UnInit_Font();
	return 0;
}
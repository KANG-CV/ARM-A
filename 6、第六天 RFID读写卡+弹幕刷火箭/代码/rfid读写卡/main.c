#include <stdio.h>
#include "rfid.h"

int main(int argc, char const *argv[])
{
	//1.打开串口1 COM2
	int fd = open(DEV_PATH,O_RDWR | O_NOCTTY);
	init_tty(fd);
	printf("初始化 COM2 成功\n");
	int ret;
	int money=0;
	while(1)
	{
		//1.寻卡
		if(PiccRequest(fd))
		{
			printf("寻卡失败\n");
			sleep(1);
			continue;
		}
		printf("寻卡成功\n");
		//2.防碰撞
		if(PiccAnticoll(fd))
		{
			printf("防碰撞失败\n");
			continue;
		}
		printf("防碰撞成功\n");
		//3.选择
		if(PiccSelect(fd))
		{
			printf("选择卡失败\n");
			continue;
		}
		printf("选择卡成功\n");
		//4.秘钥验证
		if(PiccAuthKey(fd,SECTOR,KEYA))
		{
			printf("PiccAuthKey KEYA failed!,try KEYB now\n");
			//1.寻卡
			if(PiccRequest(fd))
			{
				printf("寻卡失败\n");
				sleep(1);
				continue;
			}
			printf("寻卡成功\n");
			//2.防碰撞
			if(PiccAnticoll(fd))
			{
				printf("防碰撞失败\n");
				continue;
			}
			printf("防碰撞成功\n");
			//3.选择
			if(PiccSelect(fd))
			{
				printf("选择卡失败\n");
				continue;
			}
			printf("选择卡成功\n");
			if(PiccAuthKey(fd,SECTOR,KEYB))
			{
				printf("请注意，别尝试了破解了，我已经报警了\n");
				continue;
			}
			printf("秘钥验证KEYB成功\n");
		}
		printf("秘钥验证KEYA成功\n");
		//选择界面
		//输入 1-读卡  输入 2-写卡    输入 其他-无效操作
		printf("请输入您的选择：\n\n");
		printf("1:写卡 .....2:读卡 .....3:退出 .....\n\n");
		scanf("%d",&ret); 
		{
			switch(ret)
			{
				case 1:
						
						break;
				case 2:
						if(PiccRead(fd,SECTOR))
						{
							printf("读卡失败\n");
							break;
						}
						money = DataReadBuf[0]*100+DataReadBuf[1];
						printf("当前余额: %d 元\n",money);
						break;
				case 3:
						break;
				default:
						printf("兄die,看不清楚只能输1,2,3？\n\n");
						break;
			}
		}
	}
	close(fd);
	return 0;
}
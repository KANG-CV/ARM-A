#include "rfid.h"
#include "lcdjpg.h"
#include "get_xy.h"

unsigned char KEYA_BUF[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char KEYB_BUF[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

extern int x;
extern int y;
/*串口(RFID模块)初始化 */
void init_tty(int fd)
{    
	//声明设置串口的结构体
	struct termios termios_new;
	//先清空该结构体
	bzero( &termios_new, sizeof(termios_new));
	//	cfmakeraw()设置终端属性，就是设置termios结构中的各个参数。
	cfmakeraw(&termios_new);
	//设置波特率
	//termios_new.c_cflag=(B9600);
	cfsetispeed(&termios_new, B9600);
	cfsetospeed(&termios_new, B9600);
	//CLOCAL和CREAD分别用于本地连接和接受使能，因此，首先要通过位掩码的方式激活这两个选项。    
	termios_new.c_cflag |= CLOCAL | CREAD;
	//通过掩码设置数据位为8位
	termios_new.c_cflag &= ~CSIZE;
	termios_new.c_cflag |= CS8; 
	//设置无奇偶校验
	termios_new.c_cflag &= ~PARENB;
	//一位停止位
	termios_new.c_cflag &= ~CSTOPB;
	tcflush(fd,TCIFLUSH);
	// 可设置接收字符和等待时间，无特殊要求可以将其设置为0
	termios_new.c_cc[VTIME] = 0;
	termios_new.c_cc[VMIN] = 1;
	// 用于清空输入/输出缓冲区
	tcflush (fd, TCIFLUSH);
	//完成配置后，可以使用以下函数激活串口设置
	if(tcsetattr(fd,TCSANOW,&termios_new) )
		printf("Setting the serial1 failed!\n");

}

/*计算校验位*/
unsigned char CalBCC(unsigned char *buf, int n)
{
	int i;
	unsigned char bcc=0;
	for(i = 0; i < n; i++)
	{
		bcc ^= *(buf+i);
	}
	return (~bcc);
}
/*寻卡*/
int PiccRequest(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x07;	//帧长 7 Byte
	WBuf[1] = 0x02;	//命令类型 ISO14443A   0x02
	WBuf[2] = 'A';	//命令= 'A'   寻卡(请求)
	WBuf[3] = 0x01;	//数据长度 = 1
	WBuf[4] = 0x52;	//数据  ALL=0x52
	WBuf[5] = CalBCC(WBuf, WBuf[0]-2);		//检验位
	WBuf[6] = 0x03; //帧结束符

    timeout.tv_sec = 300;
    timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);

	write(fd, WBuf, 7);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
 	//printf("3 PiccRequest\n");
	switch(ret)
	{
		case -1:
			perror("PiccRequest select error\n");
			break;
		case  0:
			printf("PiccRequest timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 8);
			if (ret < 0)
			{
				printf("PiccRequest ret = %d, %m\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//buf[2] 状态位  该位为0，则成功响应，你就可以去获取你想要的数据
			{
			    DataBfr[0] = RBuf[4];
			    DataBfr[1] = RBuf[5];
			    FD_CLR(fd,&rdfd);
			    //printf("PiccRequest success\n");
				return 0;
			}
			break;
	}
	FD_CLR(fd,&rdfd);
	return 1;
}

/*防碰撞*/
int PiccAnticoll(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int ret, i;
	fd_set rdfd;
	struct timeval timeout;
	unsigned int ID;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x08;	//帧长= 8 Byte
	WBuf[1] = 0x02;	//命令类型 ISO14443A   0x02
	WBuf[2] = 'B';	//命令= 'B'
	WBuf[3] = 0x02;	//数据长度= 2
	WBuf[4] = 0x93;	//93表示1级防碰撞   95 二级    97 三级防碰撞
	WBuf[5] = 0x00;	//
	WBuf[6] = CalBCC(WBuf, WBuf[0]-2);		//校验位
	WBuf[7] = 0x03; //帧结束符 

	timeout.tv_sec = 300;
	timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);
	write(fd, WBuf, 8);//将这8个字节给通过串口发过去

	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccAnticoll select error\n");
			break;
		case  0:
			perror("PiccAnticoll Timeout:");
			break;
		default:
			ret = read(fd, RBuf, 10);
			if (ret < 0)
			{
				printf("PiccAnticoll ret = %d, %m\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00) //buf[2] 状态位  该位为0，则成功响应，你就可以去获取你想要的数据
			{
				ID = (RBuf[7]<<24) | (RBuf[6]<<16) | (RBuf[5]<<8) | RBuf[4];
				if(0x99FF88FF == ID)
				{
					flag1++;
				}
				memcpy(&cardid,&RBuf[4],4);//字符串的复制   将卡号(低字节在前)复制出来
				//system("madplay skcg.mp3 &");
				printf("防碰撞后 The card ID is %x\n",ID);
				return 0;
			}
	}
	return -1;
}
/*选择*/
int PiccSelect(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x0B;	//帧长= 7 Byte
	WBuf[1] = 0x02;	//命令类型 ISO14443A   0x02
	WBuf[2] = 'C';	//命令= 'C'
	WBuf[3] = 0x05;	//信息长度 = 5
	WBuf[4] = 0x93;	//防碰撞等级   ALL=0x52
        memcpy(&WBuf[5],&cardid,4);//从第5个到第9就是你要选择哪张卡的卡ID
	WBuf[9] = CalBCC(WBuf, WBuf[0]-2);		//校验位
	WBuf[10] = 0x03;//帧结束符

    timeout.tv_sec = 300;
    timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);
	
	write(fd, WBuf, WBuf[0]);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccSelect error\n");
			break;
		case  0:
			printf("PiccSelect timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 7);
			if (ret < 0)
			{
				printf("PiccSelect  ret = %d, %m\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 //buf[2] 状态位  该位为0，则成功响应，你就可以去获取你想要的数据
			{
			    printf("PiccSelect success\n");
				return 0;
			}
			break;
	}
	return -1;
}
/*暂停*/
int PiccHalt(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x06;	//帧长= 6 Byte
	WBuf[1] = 0x02;	//命令类型 ISO14443A   0x02
	WBuf[2] = 'D';	//命令= 'D
	WBuf[3] = 0x00;	//信息长度= 0
	WBuf[4] = CalBCC(WBuf, WBuf[0]-2);		//校验位
	WBuf[5] = 0x03; //帧结束符

    timeout.tv_sec = 300;
    timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);
	
	write(fd, WBuf, WBuf[0]);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccHalt error\n");
			break;
		case  0:
			printf("PiccHalt timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 7);
			if (ret < 0)
			{
				printf("PiccHalt  ret = %d, %m\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//buf[2] 状态位  该位为0，则成功响应，你就可以去获取你想要的数据
			{
			    //printf("PiccHalt success\n");
				return 0;
			}
			break;
	}
	return -1;
}
/*验证秘钥*/
int PiccAuthKey(int fd,int sector,unsigned char KeyAB)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x12;	//帧长= 17 Byte
	WBuf[1] = 0x02;	//命令类型 ISO14443A   0x02
	WBuf[2] = 'F';	//命令= 'F'
	WBuf[3] = 12;	//信息长度= 0x0c
	WBuf[4] = KeyAB;//秘钥占1位
        memcpy(&WBuf[5],&cardid,4);//卡ID占4位
        memcpy(&WBuf[9],KEYA_BUF,6);//中间秘钥 6字节
        WBuf[15]=sector;   //你要验证卡的第几块
	WBuf[16] = CalBCC(WBuf, WBuf[0]-2);		//校验位
	WBuf[17] = 0x03; 	//帧结束符

    timeout.tv_sec = 300;
    timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);

	write(fd, WBuf, WBuf[0]);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccAuthKey error\n");
			break;
		case  0:
			printf("PiccAuthKey timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 6);
			if (ret < 0)
			{
				printf("PiccAuthKey  ret = %d, %m\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//buf[2] 状态位  该位为0，则成功响应，你就可以去获取你想要的数据
			{
			    //printf("PiccAuthKey success\n");
				return 0;
			}
			break;
	}
	return -1;
}
/*读卡*/
int PiccRead(int fd,unsigned char sector)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x07;	//帧长= 7 Byte
	WBuf[1] = 0x02;	//命令类型 ISO14443A   0x02
	WBuf[2] = 'G';	//命令= 'g
	WBuf[3] = 0x01;	//信息长度= 1
	WBuf[4] = sector;	//卡的第几块
	WBuf[5] = CalBCC(WBuf, WBuf[0]-2);		//检验位
	WBuf[6] = 0x03; 	//帧结束符

    timeout.tv_sec = 300;
    timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);

	write(fd, WBuf, WBuf[0]);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccRead error\n");
			break;
		case  0:
			printf("PiccRead timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 23);
			if (ret < 0)
			{
				printf("PiccRead  ret = %d, %m\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//buf[2] 状态位  该位为0，则成功响应，你就可以去获取你想要的数据
			{
			    usleep(10*1000);
			    read(fd,&RBuf[ret],6);
			    memcpy(DataReadBuf,&RBuf[4],16); //将从buf[4]开始读取16个数据到 DataReadBuf
			    /* for(i=0;i<23;i++)
			        printf("PiccRead RBuf[%d]=%x\n",i,RBuf[i]);
				 */
				return 0;
			}
			break;
	}
	return -1;
}
/*写卡*/
int PiccWrite(int fd,unsigned char sector)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 23;	//帧长= 23 Byte
	WBuf[1] = 0x02;	//命令类型 ISO14443A   0x02
	WBuf[2] = 'H';	//命令= 'H
	WBuf[3] = 0X11;	//信息长度= 17
	WBuf[4] = sector;   //第几块
        memcpy(&WBuf[5],DataWriteBuf,16);//往第几块写入的数据  16个字节
	WBuf[21] = CalBCC(WBuf, WBuf[0]-2);		//校验位
	WBuf[22] = 0x03; 	//帧结束符

    timeout.tv_sec = 300;
    timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);
        
	write(fd, WBuf, 23);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccWrite error\n");
			break;
		case  0:
			printf("PiccWrite timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 7);
			if (ret < 0)
			{
				printf("PiccWrite  ret = %d, %m\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//buf[2] 状态位  该位为0，则成功响应，你就可以去获取你想要的数据
			{
			    //printf("PiccWrite card success\n");
				return 0;
			}
			break;
	}
	return -1;
}

//卡的充值
int Rfid_Recharge(void)
{
	int Recharge_amount = 0;//充值的金额
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
		usleep(30*1000);
		printf("寻卡成功\n");
		//2.防碰撞
		if(PiccAnticoll(fd))
		{
			printf("防碰撞失败\n");
			continue;
		}
		usleep(30*1000);
		printf("防碰撞成功\n");
		//3.选择
		if(PiccSelect(fd))
		{
			printf("选择卡失败\n");
			continue;
		}
		usleep(30*1000);
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
			usleep(30*1000);
			printf("寻卡成功\n");
			//2.防碰撞
			if(PiccAnticoll(fd))
			{
				printf("防碰撞失败\n");
				continue;
			}
			usleep(30*1000);
			printf("防碰撞成功\n");
			//3.选择
			if(PiccSelect(fd))
			{
				printf("选择卡失败\n");
				continue;
			}
			usleep(30*1000);
			printf("选择卡成功\n");
			if(PiccAuthKey(fd,SECTOR,KEYB))
			{
				printf("请注意，别尝试了破解了，我已经报警了\n");
				continue;
			}
			usleep(30*1000);
			printf("秘钥验证KEYB成功\n");
		}
		usleep(30*1000);
		printf("秘钥验证KEYA成功\n");
		if(PiccRead(fd,SECTOR))
		{
			printf("读卡失败\n");
			break;
		}
		usleep(30*1000);
		money = DataReadBuf[0]*100+DataReadBuf[1];
		printf("当前卡内余额: %d 元\n",money);
		
		//确认充值和充值界面
		lcd_draw_jpg(0,0,"./cz_jm.jpg",NULL,0,0);
		
		while(1)
		{
			if((x>112&&x<338)&&(y>72&&y<203))//4个金额充值的按钮  50  的坐标
			{

				Recharge_amount = 50;
				lcd_draw_jpg(350,205,"./queren.jpg",NULL,0,0);//显示的是 是否确认充值   小图标
				while(1)
				{
					if((x>354&&x<410)&&(y>265&&y<298))//点击确认充值
					{
						break;
					}
					if((x>455&&x<508)&&(y>265&&y<298))//除了确认键，其他任意位置不充值返回
					{
						Recharge_amount = 0;
						goto loop1;
					}
				}
				x=0;
				y=0;
				break;
			}
			/*if()//4个金额充值的按钮  100  的坐标
			{
				Recharge_amount = 100;
				
			}
			if()//4个金额充值的按钮  200  的坐标
			{
				Recharge_amount = 200;
				
			}
			if()//4个金额充值的按钮  500  的坐标
			{
				Recharge_amount = 500;
				
			}*/	
		}
		printf("数据准备就绪,开始充值!!!!\n");
		money += Recharge_amount;
		DataWriteBuf[0] = money/100;
		DataWriteBuf[1] = money%100;
		usleep(3*1000);
		if(PiccWrite(fd,SECTOR))
		{
			printf("充值失败\n");
			break;
		}
		printf("充值后的余额: %d 元\n",money);
		break;
	}	
	
loop1:
	close(fd);
	//显示主界面
	lcd_draw_jpg(0,0,"./zjm.jpg",NULL,0,0);
}
//卡的消费
int Rfid_Consumption(void)
{
	while(1)//1.寻卡
	{
		if(PiccRequest(fd))
		{
			printf("寻卡失败\n");
			sleep(1);
			continue;
		}
		usleep(10*1000);
		printf("寻卡成功\n");
		//2.防碰撞
		if(PiccAnticoll(fd))
		{
			printf("防碰撞失败\n");
			continue;
		}
		usleep(10*1000);
		printf("防碰撞成功\n");
		if(flag1/2 == 1)
		{
			//记录分1  和  秒1
		}
		if(flag1/2 == 0)
		{
			//记录分2  和  秒2
			//计算入库时间   (分2-分1)*60+秒2-秒1  == 以秒算钱
			//money  = 停车时长(秒) * 10(一秒钟1快钱)
			//①先读出卡余额  做个对比，如果卡内余额不够就图示余额不足，请先充值,然后再缴费
			//必须先充值  write卡
			//②余额充足,直接扣款，write卡
			//最后：屏幕上打出消费几元，然后余额还剩多少，3秒钟过后，返回主界面
		}	
		if(触摸屏点击哪个位置，退出消费，返回主界面)
	}
}
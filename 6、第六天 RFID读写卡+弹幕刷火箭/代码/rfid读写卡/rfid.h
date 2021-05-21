#ifndef __RFID_H__
#define __RFID_H__

#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <termios.h> 
#include <sys/select.h>
#include <pthread.h>

//连接RFID的串口
#define DEV_PATH   "/dev/ttySAC1"   //6818上面的COM2
//宏定义
#define KEYA		0x60
#define KEYB		0x61
#define SECTOR	    2

int ser_fd;
int swing_flag;//操作的标志：0-不操作 1-刷卡  2-充值
unsigned char DataReadBuf[16];
unsigned char DataWriteBuf[16];
unsigned char Enter_check[16];
unsigned int cardid;
unsigned int ID;
unsigned char sector;
unsigned char Room_ID[6];
int count;
unsigned char DataBfr[16];

/*********RFID相关函数***********/
void init_tty(int fd);/* 设置串口参数:9600速率 */
/*计算校验和*/
unsigned char CalBCC(unsigned char *buf, int n);
/*请求*/
int PiccRequest(int fd);
/*防碰撞，获取范围内最大ID*/
int PiccAnticoll(int fd);
//选择
int PiccSelect(int fd);
//验证密钥
int PiccAuthKey(int fd,int sector,unsigned char KeyAB);
//读取信息
int PiccRead(int fd,unsigned char sector);
//写入数据
int PiccWrite(int fd,unsigned char sector);
/*********************************/



#endif

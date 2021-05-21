#include <stdio.h>
#include <sys/types.h>//open
#include <sys/stat.h>//open
#include <fcntl.h>//open
#include <unistd.h>//close
#include <string.h>
#include <termios.h>
#include <errno.h>//perror
#include <strings.h>//bzero
#include <pthread.h>

#define UART2_COM "/dev/ttySAC1"
int fd=-1;
void uart_init(int fd)
{
	//①保存原先串口配置     
	struct termios old_cfg,new_cfg;
    bzero(&old_cfg,sizeof(struct termios));//memset
	bzero(&new_cfg,sizeof(struct termios));//memset
	if(tcgetattr(fd, &old_cfg) != 0)     
	{
         perror("tcgetattr"); 
         return ;
    }
	//②激活选项
    new_cfg.c_cflag  |=  CLOCAL | CREAD; 
	cfmakeraw(&new_cfg); 
	//③设置波特率  波特率：115200 ：每一秒传输115200 bit（位）数据      115200/8Byte
	cfsetispeed(&new_cfg, B115200); 
	cfsetospeed(&new_cfg, B115200);
	//④设置字符大小   8位
    new_cfg.c_cflag &= ~CSIZE; /* 用数据位掩码清空数据位设置 */
	new_cfg.c_cflag |= CS8;
	//⑤设置奇偶校验位  无校验
    new_cfg.c_cflag &= ~PARENB;
	//⑥设置停止位
    new_cfg.c_cflag &= ~CSTOPB; /* 将停止位设置为一个比特 */
	//⑦设置最少字符和等待时间
    //MIN > 0和TIME = 0：read()函数会被阻塞直到MIN个字节数据可被读取。
    new_cfg.c_cc[VTIME]= 0;
    new_cfg.c_cc[VMIN] = 1;
	//⑧清除串口缓冲
    tcflush(fd, TCIFLUSH);
	//⑨立马激活配置
	tcsetattr(fd,TCSANOW,&new_cfg);
}

void *send_data(void *arg)
{
	char buf[20]={0};
	//子线程
	while(1)
	{
		//接收数据
		bzero(buf,20);
		read(fd,buf,20);
		printf("\n\n接收的数据:%s\n",buf);
		//⑧清除串口缓冲
		tcflush(fd, TCIFLUSH);	
	}
}
int main(void)
{
	//打开串口
	fd = open(UART2_COM,O_RDWR | O_NOCTTY);
	if(fd < 0)
	{
		printf("open COM2 failed!");
		return -1;
	}
	printf("打开 COM2 成功\n");
	uart_init(fd);
	printf("初始化 COM2 成功\n");
	pthread_t pid;
	pthread_create(&pid, NULL, send_data, NULL);
	char buf[20]={0};
	sleep(0.01);
	while(1)
	{
		//发送数据
		bzero(buf,20);
		printf("请输入发送的数据:\n");
		fgets(buf,20,stdin);//stdin指键盘
		write(fd,buf,strlen(buf));//字符串字节长度
		//⑧清除串口缓冲
		tcflush(fd, TCIFLUSH);
		sleep(0.02);
	}
	close(fd);
	return 0;
}
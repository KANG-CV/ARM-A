#include <stdio.h>
#include <sys/types.h>//open
#include <sys/stat.h>//open
#include <fcntl.h>//open
#include <unistd.h>//close
#include <string.h>
#define BUF_SIZE 1024



int main(void)
{
	//1.打开源文件和目标文件
	int fd1 = open("../1.avi",O_RDWR);
	if(fd1 < 0)
	{
		printf("open fd1 failed!\n");
		return -1;
	}
	printf("fd1=%d\n",fd1);
	int fd2 = open("./2.avi",O_RDWR|O_CREAT,0644);
	if(fd2 < 0)
	{
		printf("open fd2 failed!\n");
		return -1;
	}
	printf("fd2=%d\n",fd2);
	//2.循环的从源文件读取数据并且写入目标文件
	char buf[BUF_SIZE]={0};
	int ret_r,ret_w = 0;
	while(1)
	{
		memset(buf,0,BUF_SIZE);//bzero(buf,200);
		//读取字节到缓冲区
		ret_r = read(fd1,buf,BUF_SIZE);
		printf("成功读取到[%d]个字节\n",ret_r);
		//读取几个字节就写入新文件几个字节
		ret_w = write(fd2,buf,ret_r);
		printf("成功写入[%d]个字节\n",ret_w);
		//写入完毕，break
		if(ret_w < 1024)
		{
			printf("copy success\n");
			break;
		}
	}
	//3.关闭文件
	close(fd1);
	close(fd2);
	return 0;
}
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	Init_Font();
	time_t tim;
	struct tm *timeinfo;
	char buf[30]={0};
	while(1)
	{
		time(&tim);
		timeinfo = localtime(&tim);//转化为当前系统时间
		Clean_Area(0,0,200,76,0x00FFFFFF);
		sprintf(buf,"\n%d/%d/%d/%d",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_wday);
		Display_characterX(0,10,buf,0x00FF0000,2);
		memset(buf,0,30);
		sprintf(buf,"%02d:%02d:%02d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
		Display_characterX(0,100,buf,0x00FF0000,2);
		memset(buf,0,30);
		sleep(1);
	}
	UnInit_Font();
	return 0;
}
#include "get_xy.h"
/***********************
struct input_event {
	__u16 type;//事件的类型
	__u16 code;//事件的编码，对事件进一步描述
	__s32 value;//事件的值
};
****************************/
int get_x_y(int *x,int *y)
{
	//1.打开触摸屏
	int ret=0;
	int touch_fd = open("/dev/input/event0",O_RDWR);
	if(touch_fd < 0)
	{
		printf("open touch failed!\n");
		return -1;
	}
	struct input_event A;
	memset(&A,0,sizeof(struct input_event));
	while(1)
	{
		read(touch_fd,&A,sizeof(struct input_event));//x 1024  y  600
		if(EV_ABS == A.type && ABS_X == A.code)
		{
			*x = A.value*0.78;//1024
		}
		if(EV_ABS == A.type && ABS_Y == A.code)
		{
			*y = A.value*0.8;//600
		}
		//松开返回
		if(EV_KEY == A.type && BTN_TOUCH == A.code && 0 == A.code)//按下是1   松开是0
		{
			printf("x=%d   y=%d\n",*x,*y);
			break;
		}
	}
	//关闭触摸屏
	close(touch_fd);
	return 0;
}
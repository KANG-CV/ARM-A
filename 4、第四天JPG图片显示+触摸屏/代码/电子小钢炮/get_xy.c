#include "get_xy.h"
/***********************
struct input_event {
	__u16 type;//�¼�������
	__u16 code;//�¼��ı��룬���¼���һ������
	__s32 value;//�¼���ֵ
};
****************************/
int get_x_y(int *x,int *y)
{
	//1.�򿪴�����
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
		//�ɿ�����
		if(EV_KEY == A.type && BTN_TOUCH == A.code && 0 == A.code)//������1   �ɿ���0
		{
			printf("x=%d   y=%d\n",*x,*y);
			break;
		}
	}
	//�رմ�����
	close(touch_fd);
	return 0;
}
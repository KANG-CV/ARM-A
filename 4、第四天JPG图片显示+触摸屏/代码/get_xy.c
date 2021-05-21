#include <stdio.h>
#include <sys/types.h>//open
#include <sys/stat.h>//open
#include <fcntl.h>//open
#include <unistd.h>//close
#include <string.h>
#include <linux/input.h>
/***********************
struct input_event {
	__u16 type;//�¼�������
	__u16 code;//�¼��ı��룬���¼���һ������
	__s32 value;//�¼���ֵ
};
****************************/
int main(void)
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
		switch(A.type)
		{
			//case EV_KEY:
			case EV_ABS:
				if(ABS_X == A.code)
				{
					printf("x=%d   ",A.value);
					ret++;
				}
				if(ABS_Y == A.code)
				{
					printf("y=%d\n",A.value);
					ret++;
				}
				if(ret == 2)
					break;
		}
	}
	//�رմ�����
	close(touch_fd);
	return 0;
}
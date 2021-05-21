#include <stdio.h>
#include <pthread.h>

void *erzi(void *arg)
{
	//子线程
	while(1)
	{
		printf("再打一把\n");
	}
}

int main(void)
{
	pthread_t pid;
	pthread_create(&pid, NULL, erzi, NULL);
	//主线程
	while(1)
	{
		printf("滚去做作业\n");
	}
	return 0;
}
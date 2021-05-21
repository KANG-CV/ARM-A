#ifndef __GET_XY_H__
#define __GET_XY_H__

#include <stdio.h>
#include <sys/types.h>//open
#include <sys/stat.h>//open
#include <fcntl.h>//open
#include <unistd.h>//close
#include <string.h>
#include <linux/input.h>

int get_x_y(int *x,int *y);

#endif

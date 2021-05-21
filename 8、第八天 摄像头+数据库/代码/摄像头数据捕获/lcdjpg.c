#include <stdio.h>   	
#include <fcntl.h>		 	 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "lcdjpg.h"
#include "jpeglib.h"
#include <sys/stat.h>

static char g_color_buf[FB_SIZE]={0};

static int  g_fb_fd;
static int *g_pfb_memory;

/* video_chat.c ?-?D?-??ê?μ?×?±ê */
volatile int g_jpg_in_jpg_x;
volatile int g_jpg_in_jpg_y;

unsigned long file_size_get(const char *pfile_path)
{
	unsigned long filesize = -1;	
	struct stat statbuff;
	
	if(stat(pfile_path, &statbuff) < 0)
	{
		return filesize;
	}
	else
	{
		filesize = statbuff.st_size;
	}
	
	return filesize;
}
//LCD?-μ?
void lcd_draw_point(unsigned int x,unsigned int y, unsigned int color)
{
	*(g_pfb_memory+y*800+x)=color;
}

int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size,unsigned int jpg_half)  
{
	//3?ê??ˉLCD
	g_fb_fd = open("/dev/fb0", O_RDWR);
	
	if(g_fb_fd<0)
	{
			printf("open lcd error\n");
			return -1;
	}

	g_pfb_memory  = (int *)mmap(	NULL, 				//ó3é???μ??aê?μ??·￡?éè???aNULLê±±íê?óé?μí3???¨ó3é???μ??eê?μ??·
									FB_SIZE, 				//ó3é???μ?3¤?è
									PROT_READ|PROT_WRITE, 	//?úèY?éò?±??áè?oíD′è?
									MAP_SHARED,				//12?í?ú′?
									g_fb_fd, 				//óDD§μ????t?èê?′ê
									0						//±?ó3é????ó?úèYμ??eμ?
								);
	
	/*?¨ò??a?????ó￡?′í?ó′|àí???ó*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;	
	
	char *pcolor_buf = g_color_buf;
	char 	*pjpg;
	
	unsigned int 	i=0;
	unsigned int	color =0;
	unsigned int	count =0;
	
	unsigned int 	x_s = x;
	unsigned int 	x_e ;	
	unsigned int 	y_e ;
	
			 int	jpg_fd;
	unsigned int 	jpg_size;
	
	unsigned int 	jpg_width;
	unsigned int 	jpg_height;
	
	if(pjpg_path!=NULL)
	{
		/* éê??jpg×ê?′￡?è¨?T?é?á?éD′ */	
		jpg_fd=open(pjpg_path,O_RDWR);
		
		if(jpg_fd == -1)
		{
		   printf("open %s error\n",pjpg_path);
		   
		   return -1;	
		}	
		
		/* ??è?jpg???tμ?′óD? */
		jpg_size=file_size_get(pjpg_path);	

		/* ?ajpg???téê???ú′????? */	
		pjpg = malloc(jpg_size);

		/* ?áè?jpg???t?ùóD?úèYμ??ú′? */		
		read(jpg_fd,pjpg,jpg_size);
	}
	else
	{
		jpg_size = jpg_buf_size;
		
		pjpg = pjpg_buf;
	}

	/*×￠2á3?′í′|àí*/
	cinfo.err = jpeg_std_error(&jerr);

	/*′′?¨?a??*/
	jpeg_create_decompress(&cinfo);

	/*?±?ó?a???ú′?êy?Y*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_size);
	
	/*?á???tí·*/
	jpeg_read_header(&cinfo, TRUE);

	/*?aê??a??*/
	jpeg_start_decompress(&cinfo);		
	if(jpg_half)
	{
		x_e	= x_s+(cinfo.output_width/2);
		y_e	= y  +(cinfo.output_height/2);		
		
		/*?á?a??êy?Y*/
		while(cinfo.output_scanline < cinfo.output_height)
		{		
			pcolor_buf = g_color_buf;
			
			/* ?áè?jpgò?DDμ?rgb?μ */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);			
			
			/* ?ù?áè?jpgò?DDμ?rgb?μ */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);

			for(i=0; i<(cinfo.output_width/2); i++)
			{
				/* ??è?rgb?μ */
				color = 		*(pcolor_buf+2);
				color = color | *(pcolor_buf+1)<<8;
				color = color | *(pcolor_buf)<<16;
				
				/* ??ê?????μ? */
				lcd_draw_point(x,y,color);			
				pcolor_buf +=6;			
				x++;
			}		
			/* ??DD */
			y++;						
			x = x_s;			
		}
	}
	else
	{
		x_e	= x_s+cinfo.output_width;
		y_e	= y  +cinfo.output_height;	

		/*?á?a??êy?Y*/
		while(cinfo.output_scanline < cinfo.output_height )
		{		
			pcolor_buf = g_color_buf;
			
			/* ?áè?jpgò?DDμ?rgb?μ */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);
			
			for(i=0; i<cinfo.output_width; i++)
			{
				/* ??è?rgb?μ */
				color = 		*(pcolor_buf+2);
				color = color | *(pcolor_buf+1)<<8;
				color = color | *(pcolor_buf)<<16;
				
				/* ??ê?????μ? */
				lcd_draw_point(x,y,color);				
				pcolor_buf +=3;				
				x++;
			}			
			/* ??DD */
			y++;						
			x = x_s;			
		}		
	}
	
	/*?a??íê3é*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if(pjpg_path!=NULL)
	{
		/* 1?±?jpg???t */
		close(jpg_fd);	
		
		/* êí·?jpg???t?ú′????? */
		free(pjpg);		
	}
	//LCD1?±?
	/* è????ú′?ó3é? */
	munmap(g_pfb_memory, FB_SIZE);
	
	/* 1?±?LCDéè±? */
	close(g_fb_fd);
	return 0;
}


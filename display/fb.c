
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <wchar.h>


#include "../include/disp_manager.h"
#include "../include/config.h"

struct fb_var_screeninfo g_tVar;	/* Current var */
struct fb_fix_screeninfo g_tFix;	/* Current fix */

static int g_iLine_width;
static int g_iPixel_width;
static int g_iScreensize;
static unsigned char* g_pucFbMem;
static int g_FBFd;
DispOpr g_tFBDispOpr;
static int FBDeviceInit()
{
    g_FBFd = open("/dev/fb0", O_RDWR);
	if (g_FBFd < 0)
	{
		DBG_PRTINTF("can't open /dev/fb0\n");
		return -1;
	}

	if (ioctl(g_FBFd, FBIOGET_VSCREENINFO, &g_tVar))
	{
		DBG_PRTINTF("can't get var\n");
		return -1;
	}

	if (ioctl(g_FBFd, FBIOGET_FSCREENINFO, &g_tFix))
	{
		DBG_PRTINTF("can't get fix\n");
		return -1;
	}

	g_iLine_width  = g_tVar.xres * g_tVar.bits_per_pixel / 8;
	g_iPixel_width = g_tVar.bits_per_pixel / 8;
	g_iScreensize = g_tVar.xres * g_tVar.yres * g_tVar.bits_per_pixel / 8;
	g_pucFbMem = (unsigned char *)mmap(NULL , g_iScreensize, PROT_READ | PROT_WRITE, MAP_SHARED, g_FBFd, 0);
    if(g_pucFbMem == (void*)0)
    {
        DBG_PRTINTF("can't get fix\n");
		return -1;
	}
    g_tFBDispOpr.iBpp =  g_iPixel_width;
    g_tFBDispOpr.iXres = g_tVar.xres;
    g_tFBDispOpr.iYres = g_tVar.yres;


}


static int FBShowPixel(int iPenX, int iPenY, unsigned int dwColor)
{
	unsigned char *pucPen8 = g_pucFbMem + iPenY*g_iLine_width + iPenX*g_iPixel_width;
	unsigned short *pucPen16;	
	unsigned int *pucPen32;	

	unsigned int red, green, blue;	

	pucPen16 = (unsigned short *)pucPen8;
	pucPen32 = (unsigned int *)pucPen8;

	switch (g_tVar.bits_per_pixel)
	{
		case 8:
		{
			*pucPen8 = dwColor;
			break;
		}
		case 16:
		{
			/* 565 */
			red   = (dwColor >> 16) & 0xff;
			green = (dwColor >> 8) & 0xff;
			blue  = (dwColor >> 0) & 0xff;
			dwColor = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			*pucPen16 = dwColor;
			break;
		}
		case 32:
		{
			*pucPen32 = dwColor;
			break;
		}
		default:
		{
			printf("can't surport %dbpp\n", g_tVar.bits_per_pixel);
			break;
		}
	}
}

static int FBCleanScreen(unsigned int dwBackColor)
{
	unsigned char *pucPen8 = g_pucFbMem + iPenY*g_iLine_width + iPenX*g_iPixel_width;
	unsigned short *pucPen16;	
	unsigned int *pucPen32;	

	unsigned int red, green, blue;	
    int i;
	pucPen16 = (unsigned short *)pucPen8;
	pucPen32 = (unsigned int *)pucPen8;

	switch (g_tVar.bits_per_pixel)
	{
        /* 8位的直接memset  把每个字节都设置成dwBackColor 
           16位和32位的不行，memset设置单位是字节 ，16位是2个字节一个像素，32位是4个字节一个像素。
        */
		case 8:
		{
            memset(g_pucFbMem, dwBackColor, g_iScreensize);
		}

		case 16:
		{
			/* 565 */
			red   = (dwBackColor >> 16) & 0xff;
			green = (dwBackColor >> 8) & 0xff;
			blue  = (dwBackColor >> 0) & 0xff;
			dwBackColor = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
            for(i = 0; i < g_iScreensize; i++)
            {
                *pucPen16 = dwBackColor;
                pucPen16++;
                i += 2;
            }
			break;
		}
		case 32:
		{
            for(i = 0; i < g_iScreensize; i++)
            {
                *pucPen32 = dwBackColor;
                pucPen32++;
                i += 4;
            }
			break;
		}
		default:
		{
			printf("can't surport %dbpp\n", g_tVar.bits_per_pixel);
            return -1;
    		break;
		}
	}
    return 0;
}


DispOpr g_tFBDispOpr = {
    .name = "fb",
    .DeviceInit = FBDeviceInit,
    .ShowPixel  = FBShowPixel,
    .CleanScreen = FBCleanScreen,
};

void FBDeviceRegister(void)
{
    RegisterDisplay(&g_tFBDispOpr);
}
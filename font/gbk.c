#include "../include/font_manager.h"
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static unsigned char* g_pucHZKMen;
int GBKFontInit(char* aFilename, unsigned int iFontSize)
{
    struct stat hzk16_stat;

    int fd_hzk16 = open(aFilename, O_RDONLY);
    if (fd_hzk16 < 0)
    {
        DBG_PRINTF("open %s failed\n", aFilename);
        return -1;
    }
    if (fstat(fd_hzk16, &hzk16_stat))
    {
        DBG_PRINTF("can't get stat!\n");
        return -1;
    }
    g_pucHZKMen = mmap(NULL, hzk16_stat.st_size, PROT_READ, MAP_SHARED, fd_hzk16, 0);
    if (g_pucHZKMen == (void *)-1)
    {
        DBG_PRINTF("can't mmap!\n")
    }
    return 0;
}
/* dwCode低八位是位码 高八位是区码 一共用到低16位 */
int GBKGetFontBitMap(unsigned int dwCode, PFontBitMap ptFontBitMap)
{
    unsigned int iArea;
    unsigned int iWhere;
    int iPenX = ptFontBitMap->iCurOriginX;
    int iPenY = ptFontBitMap->iCurOriginY;

    if(dwCode & 0xffff0000)
    {
        printf("can't support the dwCode!\n");
        return -1;
    }
    iArea = (dwCode >> 8) - 0xA1;
    iWhere = (dwCode & 0xff) - 0xA1;

    ptFontBitMap->iXLeft  = iPenX;
    ptFontBitMap->iYTop   = iPenY - 16;
    ptFontBitMap->iXMax   = iPenX + 16;      /* 字符的宽度 以像素为单位 */
    ptFontBitMap->iYMax   = iPenY;      /* 字符的高度 以像素为单位 */
    ptFontBitMap->iPitch  = 2;     /* bytes per line  对于单色位图 每行之间的跨度 （单位是字节） */  
    ptFontBitMap->iBpp    = 1;       /* bits per pixel 每个像素多少个字节表示  为了适配freetype位图的 MONO单像素模式（一个像素一个字节）和 ant-color模式（一个像素8个字节） */
    ptFontBitMap->iNextOriginX = iPenX + 16;
    ptFontBitMap->iNextOriginY = iPenY;
    ptFontBitMap->pucBuffer = g_pucHZKMen + (iArea * 94 + iWhere) * 32  ;
}

FontOpr g_tGBKOpr = {
    .name = "gbk",
    .FontInit = GBKFontInit,
    .GetFontBitMap = GBKGetFontBitMap,
};


void GBKFontRegister(void)
{
    RegisterFont(&g_tGBKOpr);
}
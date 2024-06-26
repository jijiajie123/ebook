#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <config.h>
#include <draw.h>
#include <encoding_manager.h>
#include <font_manager.h>
#include <disp_manager.h>
#include "../include/encoding_manager.h"
#include "../include/font_manager.h"
#include "../include/disp_manager.h"
#include "../include/draw.h"
#include "../include/config.h"
#include <string.h>


static int g_tFdTextFile;
static int g_dwFontSize;

static unsigned char *g_pucTextFileMem;
static unsigned char *g_pucTextFileMemEnd;

static unsigned char *g_pucLcdFirstPosAtFile;
static unsigned char *g_pucLcdNextPosAtFile;


static PEncodingOpr g_ptEncodingOprForFile;
static PDispOpr g_ptDispOpr;

static PPageDesc g_ptPages   = NULL;
static PPageDesc g_ptCurPage = NULL;



/* 打开文本文件
   映射文本文件到内存中
   根据文本文件的编码格式，选择对应的编码操作结构体 
*/
int OpenTextFile(char *pucFileName)
{
    struct stat tStat;
    g_tFdTextFile = open(pucFileName, O_RDONLY);
    if (g_tFdTextFile < 0)
	{
		DBG_PRINTF("can't open text file %s\n", pcFileName);
		return -1;
	}

    if (fstat(g_tFdTextFile, &tStat))
    {
 		DBG_PRINTF("can't get fstat\n");
		return -1;       
    }
    /*   映射文本文件到内存中 */
    g_pucTextFileMem = mmap(NULL, tStat.st_size, PROT_READ, MAP_PRIVATE, g_tFdTextFile, 0);
    if (g_pucTextFileMem == (void* )-1)
    {
		DBG_PRINTF("can't mmap for text file\n");
		return -1;        
    }
    g_pucTextFileMemEnd = g_pucTextFileMem + tStat.st_size;
    /* 根据文本文件编码格式选择合适的编码操作 */
    g_ptEncodingOprForFile = SelectEncodingOpr(g_pucTextFileMem);
    if (g_ptEncodingOprForFile)
    {
        g_pucLcdFirstPosAtFile = g_pucTextFileMem + g_ptEncodingOprForFile->iHeadLen;
        return 0;
    }
    else 
    {
        return -1;
    }
}


/* 根据编码操作支持的链表去尝试初始化所有支持的字库文件 
 * 初始化不成功则在链表中将对应的字体节点删除
 * 存在字体被初始化成功 返回 0  若液体个都没初始化成功则返回0
*/
int SetFontDetail(char* pucHZKFile, char *pucFreetypeFile, unsigned int dwFontSize)
{
    int iError;
    int iRet;
    PFontOpr ptTmp;
    g_dwFontSize = dwFontSize;
    PFontOpr ptFontOpr = g_ptEncodingOprForFile->ptFontOprSupportHead;
    /* 将编码所支持的所有字符集都初始化 */
    while(ptFontOpr)
    {
        if(strcmp(ptFontOpr->name, "ascii") == 0)
        {
            iError = ptFontOpr->FontInit(NULL, g_dwFontSize);
        }
        else if(strcmp(ptFontOpr->name, "gbk") == 0)
        {
            iError = ptFontOpr->FontInit(pucHZKFile, g_dwFontSize);
        }
        else if(strcmp(ptFontOpr->name, "freetype") == 0)
        {
            iError = ptFontOpr->FontInit(pucFreetypeFile, g_dwFontSize);
        }
        
        DBG_PRINTF("%s, %d\n",ptFontOpr->name, iError);
        ptTmp = ptFontOpr->ptNext;

        if (!iError)
        {
            iRet = 0;
        }
        else 
        {
            DelFontOprFrmEncoding(g_ptEncodingOprForFile, ptFontOpr);
            ptFontOpr = ptTmp;
            iRet = -1;
        }
        
    }
    return iRet;
}

int SelectAndInitDisplay(char *pucDispName)
{
    PDispOpr ptDispOpr = GetDisOpr(pucDispName);
  	int iError;
    if(!ptDispOpr)
    {
        iError = -1;
        return iError;
    }
    iError = ptDispOpr->DeviceInit();
    return iError;
}

int IncLcdX(int iX)
{

}

/* 显示下一行时 增加Y坐标
 * 成功则返回下一行Y坐标的值
 * 失败返回0  （超出lcd显示范围 已经到lcd底部了）
 */
int IncLcdY(int iY)
{
    if(g_ptDispOpr->iYres >= iY + g_dwFontSize)
    {
        return iY + g_dwFontSize;
    }
    else
    {
        return 0;
    }
}

/* 绘制字符前 先更新字符在lcd上的位置 
 * 若是满行，要重新定位，换到下一行
 * 若是满页则返回-1
*/
int RelocateFontPos(PFontBitMap ptFontBitMap)
{
    int iLcdX;
    int iLcdY;
	int iDeltaX;
	int iDeltaY;
    /* 先检查是否满页 */
    if (ptFontBitMap->iYMax > g_ptDispOpr->iYres)
    {
        return -1;
    }
    /* 再检查是否满行 */
    if (ptFontBitMap->iXMax > g_ptDispOpr->iXres)
    {
        iLcdY = IncLcdY(ptFontBitMap->iYMax);
        if (iLcdY == 0)
		{
			/* 满页了 */
			return -1;
		}
        else 
        {
            /* 没满页 更新坐标 换到下一行 */
            iDeltaX =  0 - ptFontBitMap->iCurOriginX;
            iDeltaY = iLcdY - ptFontBitMap->iCurOriginY;
            
            // iDeltaX =  0 - ptFontBitMap->iXLeft;
            // iDeltaY = iLcdY - ptFontBitMap->iYMax;

            ptFontBitMap->iCurOriginX  += iDeltaX; 
			ptFontBitMap->iCurOriginY  += iDeltaY;

			ptFontBitMap->iNextOriginX += iDeltaX;
			ptFontBitMap->iNextOriginY += iDeltaY;

			ptFontBitMap->iXLeft += iDeltaX;
			ptFontBitMap->iXMax  += iDeltaX;

			ptFontBitMap->iYTop  += iDeltaY;
			ptFontBitMap->iYMax  += iDeltaY;

            return 0;

        }
    }
    return 0;
}
/* 绘制一个字符 */
int ShowOneFont(PFontBitMap ptFontBitMap)
{
    int x;
    int y;
    unsigned char ucByte;
    int bit = 1;
    int i = 0;
    /* 单色位图 位图中一个bit表示lcd一个像素 */
    if(ptFontBitMap->iBpp == 1)
    {
        for(y = ptFontBitMap->iYTop ; y <= ptFontBitMap->iYMax; y++)
        {
            i = (y - ptFontBitMap->iYTop) * ptFontBitMap->iPitch;
            for(x = ptFontBitMap->iXLeft, bit = 7;  x <= ptFontBitMap->iXMax; x++)
            {
                if (bit == 7)
                {
                    ucByte = ptFontBitMap->pucBuffer[i++];
                }                
                if (ucByte & (1 << bit))
                {
                    g_ptDispOpr->ShowPixel(x, y, COLOR_FOREGROUND);
                }
                bit--;
                if (bit == -1)
                {
                    bit = 7;
                }
            }

        }
    }

    /* 位图中8个bit表示lcd一个像素 */
    else if (ptFontBitMap->iBpp == 8)
    {
        /* 对于8Bpp字体，直接按像素显示 */
        for (y = ptFontBitMap->iYTop; y < ptFontBitMap->iYMax; y++)
            for (x = ptFontBitMap->iXLeft; x < ptFontBitMap->iXMax; x++)
            {
                /* 根据缓冲区中的颜色值显示像素 */
                //g_ptDispOpr->ShowPixel(x, y, ptFontBitMap->pucBuffer[i++]);
                if (ptFontBitMap->pucBuffer[i++])
                    g_ptDispOpr->ShowPixel(x, y, COLOR_FOREGROUND);
            }
    }
    else
    {
        /* 不支持的位深度将打印错误并返回失败 */
        DBG_PRINTF("ShowOneFont error, can't support %d bpp\n", ptFontBitMap->iBpp);
        return -1;
    }
    return 0;

}

/* 显示一整页的内容 
 *
 *
 *
*/
int ShowOnePage(unsigned char* pucTextFileMemCurPos)
{
    int iError;
    int iLen;
    unsigned int dwCode;
    unsigned char *pucBufStart = pucTextFileMemCurPos;
    FontBitMap tFontBitMap;
    PFontOpr ptFontOpr;

    int bHasNotClrScreen = 1;  /* 1 表示没有清屏，0表示清屏了 */
    int bHasGetCode = 0;       /* 1表示已经获取了编码，0表示没有获取编码 */

    tFontBitMap.iCurOriginX = 0;
    tFontBitMap.iCurOriginY = g_dwFontSize;

    while(1)
    {
        iLen = g_ptEncodingOprForFile->GetCodeFrmBuf(pucBufStart, g_pucTextFileMemEnd, &dwCode);
        if (iLen == 0)
        {
            if (bHasGetCode == 0)
            {
                /* 还未获得过编码 文本文件为空 */
                return -1;
            }
            else
            {
                /* 文本文件显示完毕 */
                return 0;
            }
        }
        bHasGetCode = 1;
        /* 正确读取到编码 更新pucBufStart位置 */
        pucBufStart = pucBufStart + iLen;
        /* 处理回车换行  windows下是 ： \r\n   linux下是：\n */
        if (dwCode == '\n')
        {
            tFontBitMap.iNextOriginX = 0;
            tFontBitMap.iCurOriginY = IncLcdY(tFontBitMap.iNextOriginY);
        }
        else if (dwCode == '\r')
        {
            continue;
        }
        else if (dwCode == '\t')
        {
            /*遇到\t 显示一个空格*/
            dwCode = ' ';  
        }


        /* 打印调试信息 */
        DBG_PRINTF("dwCode = 0x%x\n", dwCode);
        

        /* 根据编码获取位图 */
        ptFontOpr = g_ptEncodingOprForFile->ptFontOprSupportHead;
        while (ptFontOpr)
        {
            /* 尝试获取字体位图 */
            iError = ptFontOpr->GetFontBitMap(dwCode, &tFontBitMap); /* 根据编码值获取点阵 */
            /* 如果获取成功 */
            if (0 == iError)
            {
                /* 检查字符位置是否需要重新定位 */
                if (RelocateFontPos(&tFontBitMap))
                {
                    /* 如果屏幕空间不足以显示字符，则返回0 */
                    return 0;
                }

                /* 如果还未清空屏幕，则先清空屏幕 */
                if (bHasNotClrScreen)
                {
                    /* 首先清屏 */
                    g_ptDispOpr->CleanScreen(COLOR_BACKGROUND);
                    bHasNotClrScreen = 0;
                }
                /* 显示一个字符 */
                if (ShowOneFont(&tFontBitMap))
                {
                    /* 如果显示失败，则返回-1 */
                    return -1;
                }
                
                /* 更新当前显示位置和下一页的起始位置 */
                tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
                tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
                g_pucLcdNextPosAtFile = pucBufStart;

                /* 结束当前循环，处理下一个字符 */
                break;
            }
			/* 如果没有找到支持的字库，则继续处理下一个字符 */
            ptFontOpr = ptFontOpr->ptNext;
        }        
    }

    return 0;
}

/* 记录一个页面 
 * 将页面加入页面链表
*/
void RecordPage(PPageDesc ptPageNew)
{
    PPageDesc ptTmp = g_ptPages;
    if (!g_ptPages)
    {
        g_ptPages = ptPageNew;
    }
    else
    {
        while(ptTmp)
        {
            ptTmp = ptTmp->ptNextPage;
        }
        ptTmp->ptNextPage = ptPageNew;
        ptPageNew->ptPrePage = ptTmp;
        ptPageNew->ptNextPage = NULL;
    }


}
int ShowNextPage(void)
{
    int iError;
    PPageDesc ptPage; 
    unsigned char *pucTextFileMemCurPos;
    /* 当前页不存在 即为刚开始读取文本 还没创建第一页 */
    if (g_ptCurPage)
    {
        pucTextFileMemCurPos = g_ptCurPage->pucLcdNextPageFirstPosAtFile;
    }
    else 
    {
        pucTextFileMemCurPos = g_pucLcdFirstPosAtFile;
    }

    iError = ShowOnePage(pucTextFileMemCurPos);
    if (!iError)
    {
        if(g_ptCurPage && g_ptCurPage->ptNextPage)
        {
            g_ptCurPage = g_ptCurPage->ptNextPage;
        }
        else
        {
            ptPage = malloc(sizeof(PageDesc));
            if(ptPage)
            {
                ptPage->pucLcdNextPageFirstPosAtFile = g_pucLcdNextPosAtFile;
                ptPage->pucLcdFirstPosAtFile         = pucTextFileMemCurPos;
                ptPage->ptPrePage  = NULL;
                ptPage->ptNextPage = NULL;
                g_ptCurPage = ptPage;
                DBG_PRINTF("%s %d, pos = 0x%x\n", __FUNCTION__, __LINE__, (unsigned int)ptPage->pucLcdFirstPosAtFile);
                RecordPage(ptPage);
                return 0;
            }
            else 
            {
                /* 内存分配失败，返回错误码 */
                return -1;
            }
        }
    }
    /* 如果显示不成功，返回错误码 */
    return iError;
}


/* 若是当前页指针为空 或者当前页指针的上一页也为空 就直接返回
 * 否则读取页面链表 通过ShowOnePage显示上一页。
 
 */
int ShowPrePage(void)
{
    int iError;
    unsigned char* pucTextFileMemCurPos;

	DBG_PRINTF("%s %d\n", __FUNCTION__, __LINE__);
	if (!g_ptCurPage || !g_ptCurPage->ptPrePage)
	{
		return -1;
	}

	DBG_PRINTF("%s %d, pos = 0x%x\n", __FUNCTION__, __LINE__, (unsigned int)g_ptCurPage->ptPrePage->pucLcdFirstPosAtFile);

    pucTextFileMemCurPos = g_ptCurPage->ptPrePage->pucLcdFirstPosAtFile;
    iError = ShowOnePage(pucTextFileMemCurPos);
    if (!iError)
    {
        g_ptCurPage = g_ptCurPage->ptPrePage;       
    }   
    return iError;     
}
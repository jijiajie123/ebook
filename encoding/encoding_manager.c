/* 读取编码值 */
#include "../include/encoding_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static PEncodingOpr g_ptEncodingOprs = NULL;

int RegisterEncodingOpr(EncodingOpr *ptEncodingOpr)
{
    ptEncodingOpr->ptNext = g_ptEncodingOprs;
    g_ptEncodingOprs = ptEncodingOpr;
    return 0;
}

int ShowEncodingOpr(void)
{
    int i = 0;
    PEncodingOpr ptTmp = g_ptEncodingOprs;

    while (ptTmp)
    {
        printf("EncodingOpr: %02d %s\n", i++, ptTmp->name);
        ptTmp = ptTmp->ptNext;
        
    }
    
    return 0;
}

PEncodingOpr SelectEncodingOpr(unsigned char *pucHeadBuf)
{
    PEncodingOpr ptTmp = g_ptEncodingOprs;
    while (ptTmp)
    {
        if (ptTmp->isSupport(pucHeadBuf))
        {
            return ptTmp;
        }
        ptTmp = ptTmp->ptNext;
    }
    return NULL;
}

/* 将编码支持的字体库添加到编码支持列表中 */
int AddFontOprForEncoding(PEncodingOpr ptEncodingOpr, PFontOpr ptFontOpr)
{
    PFontOpr ptTmp;
    if (!ptEncodingOpr || !ptFontOpr)
    {
        return -1;
    }
    else 
    {
        ptTmp = (PFontOpr)malloc(sizeof(FontOpr));
        if (!ptTmp)
        {
            return -1;
        }
        else
        {
            memcpy(ptTmp, ptFontOpr, sizeof(FontOpr));
            ptTmp->ptNext = ptEncodingOpr->ptFontOprSupportHead;
            ptEncodingOpr->ptFontOprSupportHead = ptTmp;
        }
    }
    return 0;
}
/* 从编码支持的字体链表中删除一个字体 */
int DelFontOprFrmEncoding(PEncodingOpr ptEncodingOpr, PFontOpr ptFontOpr)
{
    PFontOpr ptTmp;
    PFontOpr ptPre;
    if (!ptEncodingOpr || !ptFontOpr)
    {
        return -1;
    }
    else 
    {
        ptTmp = ptEncodingOpr->ptFontOprSupportHead;
        if (strcmp(ptFontOpr->name, ptTmp->name) == 0)
        {
            ptEncodingOpr->ptFontOprSupportHead = ptTmp->ptNext;
            free(ptTmp);
            return 0;
        }
        else 
        {
            ptPre = ptTmp;
            ptTmp = ptTmp->ptNext;
            while(ptTmp)
            {
                if (strcmp(ptTmp->name, ptFontOpr->name) == 0)
                {
                    ptPre->ptNext = ptTmp->ptNext;
                    free(ptTmp);
                    return 0;
                }
                else
                {
                    ptPre = ptTmp;
                    ptTmp = ptTmp->ptNext;
                }

            }
        }    
            
    }

    return -1;

}
int EncodingSystemRegister()
{
    extern void RegisterEncodingOpr_UTF8(void);   
    extern void RegisterEncodingOpr_UTF16be(void);
    extern void RegisterEncodingOpr_Ascii(void);
    extern void RegisterEncodingOpr_UTF16le(void);

    RegisterEncodingOpr_Ascii();
    RegisterEncodingOpr_UTF8();
    RegisterEncodingOpr_UTF16le();
    RegisterEncodingOpr_UTF16be();
    return 0;
}
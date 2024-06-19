#ifndef _ENCODING_MANAGER_H_
#define _ENCODING_MANAGER_H_

#include "../include/font_manager.h"

typedef struct EncodingOpr {
    char *name;
    int iHeadLen;
    PFontOpr ptFontOprSupportHead;   /* 字体支持列表头 (根据编码值去支持的字库中寻找对应字体)*/
    int (*GetCodeFrmBuf)(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);
    int (*isSupport)(unsigned char * pucHeadBuf);
    struct EncodingOpr *ptNext;
}EncodingOpr, *PEncodingOpr;


int RegisterEncodingOpr(EncodingOpr *ptEncodingOpr);
int ShowEncodingOpr(void);
PEncodingOpr SelectEncodingOpr(unsigned char *pucHeadBuf);
int AddFontOprForEncoding(PEncodingOpr ptEncodingOpr, PFontOpr ptFontOpr);
int DelFontOprFrmEncoding(PEncodingOpr ptEncodingOpr, PFontOpr ptFontOpr);
void EncodingSystemRegister();

#endif
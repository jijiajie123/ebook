#include "../include/encoding_manager.h"


/*大端存储  高位在低地址 低位在高地址  0xFE FF*/
int Utf16beGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
    if (pucBufStart >= pucBufEnd)
    {
        return 0;
    }

    *pdwCode =((unsigned int ) pucBufStart[0] << 8) + pucBufStart[1];

    return 2;
}

int isSupportUtf16be(unsigned char * pucHeadBuf)
{
    if(pucHeadBuf[1] == 0xFF && pucHeadBuf[0] == 0xFE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


EncodingOpr g_tUtf16beEncodingOpr = {
    .name = "utf-16be",
    .iHeadLen = 2,
    .GetCodeFrmBuf = Utf16beGetCodeFrmBuf,
    .isSupport = isSupportUtf16be,
};
void RegisterEncodingOpr_UTF16be(void)
{
    AddFontOprForEncoding(&g_tUtf16beEncodingOpr,GetFontOpr("freetype"));
    AddFontOprForEncoding(&g_tUtf16beEncodingOpr,GetFontOpr("ascii"));
    RegisterEncodingOpr(&g_tUtf16beEncodingOpr);
}
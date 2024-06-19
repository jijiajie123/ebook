#include "../include/encoding_manager.h"

/*小端存储  高位在高地址 低位在低地址  0xFF 0xFE*/
int Utf16leGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
    if (pucBufStart >= pucBufEnd)
    {
        return 0;
    }

    *pdwCode =((unsigned int ) pucBufStart[1] << 8) + pucBufStart[0];

    return 2;
}

int isSupportUtf16le(unsigned char * pucHeadBuf)
{
    if(pucHeadBuf[0] == 0xFF && pucHeadBuf[1] == 0xFE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


EncodingOpr g_tUtf16leEncodingOpr = {
    .name = "utf-16le",
    .iHeadLen = 2,
    .GetCodeFrmBuf = Utf16leGetCodeFrmBuf,
    .isSupport = isSupportUtf16le,
};
void RegisterEncodingOpr_UTF16le(void)
{
    AddFontOprForEncoding(&g_tUtf16leEncodingOpr,GetFontOpr("freetype"));
    AddFontOprForEncoding(&g_tUtf16leEncodingOpr,GetFontOpr("ascii"));
    RegisterEncodingOpr(&g_tUtf16leEncodingOpr);
}
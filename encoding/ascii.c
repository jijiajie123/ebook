#include "../include/encoding_manager.h"
#include <string.h>


/* 可能是一个字节的英文ascii码 也可能是两个字节的GBK码 
*  GBK码的2个字节，第一个字节的范围是0X81~0XFE，第二个字节的范围是0X40~0XFE。
*  读取pucBufStart[0] 和 pucBufStart[1]
*/
int AsciiGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
	unsigned char* pucBuf = pucBufStart;
	unsigned char c = *pucBuf;
	if (c < (unsigned char)0x81 && pucBuf < pucBufEnd)
	{
		*pdwCode = *pucBuf;
		return 1;
	}
	if (c >= (unsigned char)0x81 && pucBuf + 1 <= pucBufEnd)
	{
		*pdwCode = (unsigned int)(pucBuf[0] << 8) + pucBuf[1];
		return 2;
	}
	if (pucBuf < pucBufEnd)
	{
		*pdwCode = *pucBuf;
		return 1;
	}
	else
	{
		return 0;
	}
}

int isSupportAscii(unsigned char * pucHeadBuf)
{
	const char aStrUtf8[]    = {0xEF, 0xBB, 0xBF, 0};
	const char aStrUtf16le[] = {0xFF, 0xFE, 0};
	const char aStrUtf16be[] = {0xFE, 0xFF, 0};
  
	if (strncmp((const char*)pucHeadBuf, aStrUtf8, 3) == 0)
	{
		/* UTF-8 */
		return 0;
	}
	else if (strncmp((const char*)pucHeadBuf, aStrUtf16le, 2) == 0)
	{
		/* UTF-16 little endian */
		return 0;
	}
	else if (strncmp((const char*)pucHeadBuf, aStrUtf16be, 2) == 0)
	{
		/* UTF-16 big endian */
		return 0;
	}
	else
	{
		return 1;
	}
}


EncodingOpr g_tAsciiEncodingOpr = {
	.name = "ascii",
	.iHeadLen = 0,
    .GetCodeFrmBuf = AsciiGetCodeFrmBuf,
    .isSupport = isSupportAscii,
};

void RegisterEncodingOpr_Ascii(void)
{
    AddFontOprForEncoding(&g_tAsciiEncodingOpr,GetFontOpr("freetype"));
    AddFontOprForEncoding(&g_tAsciiEncodingOpr,GetFontOpr("ascii"));
	AddFontOprForEncoding(&g_tAsciiEncodingOpr,GetFontOpr("gbk"));
    RegisterEncodingOpr(&g_tAsciiEncodingOpr);
}
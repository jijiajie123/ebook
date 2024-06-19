#include "../include/encoding_manager.h"


/* 中文字符有freetype库支持 */
/* 英文字母有ASCII码支持    */

static int GetOneBitNum(unsigned char pucByte)
{
    int i;
    int iNum = 0;
    for(i = 0; i < 8; i++)
    {
        if((pucByte << i) & 0x80)
        {
            iNum++;
        }
    }
    return iNum;
}


/* 每次返回一个汉字或字符的编码
 *
 * 返回值 0    ---     表示文件结束
 * 返回值 正值  ---     buffer中用几个字节来表示当前的编码值
*/
static int Utf8GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
    int i;
    int iNum = 0;
    unsigned char ucVal;
    unsigned int dwSum = 0;
    /* 文件结束 */
    if(pucBufStart >= pucBufEnd)
    {
        return 0;
    }
    iNum = GetOneBitNum(pucBufStart[0]);
    if((pucBufStart + iNum) > pucBufEnd)
    {
        /* 文件结束 */
        return 0;
    }
    if(iNum == 0)
    {
        /* ASCII */
        *pdwCode = pucBufStart[0];
        return 1;
    }
    else
    {
        ucVal = pucBufStart[0] << iNum;
        ucVal = ucVal >> iNum;
        dwSum += ucVal;
        for(i = 0; i < iNum; i++)
        {
            ucVal = pucBufStart[i] & 0x3f;
            dwSum <<= 6;
            dwSum += ucVal;
        }
        *pdwCode = dwSum;
        return iNum; 
    }
}

/* utf-8            标志字节EF BB BF
 * utf16小字节序     标志头 FF FE
 * utf16大字节序     标志头 FE FF
*/
static int isUtf8Coding(unsigned char * pucBufHead)
{
    if(pucBufHead[0] == 0xEF && pucBufHead[1] == 0xBB && pucBufHead[2] == 0xBF)
        return 1;
    else 
        return 0;
}
EncodingOpr g_tUtf8EncodingOpr = {
    .iHeadLen = 3,
    .GetCodeFrmBuf = Utf8GetCodeFrmBuf,
    .isSupport = isUtf8Coding,
};


void RegisterEncodingOpr_UTF8(void)
{
    AddFontOprForEncoding(&g_tUtf8EncodingOpr,GetFontOpr("freetype"));
    AddFontOprForEncoding(&g_tUtf8EncodingOpr,GetFontOpr("ascii"));
    RegisterEncodingOpr(&g_tUtf8EncodingOpr);
}
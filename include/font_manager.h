#ifndef _FONT_MANAGER_H
#define _FONT_MANAGER_H

#define FONTDATAMAX 4096
/* 每个字体的位图
 * 设计该结构体 需要支持多种字体文件 
 * 为了兼容ascii gbk freetype三种点阵字符的方法 
 */
typedef struct FontBitMap{
    int iXLeft;
    int iYTop;
    int iXMax;      /* 字符的宽度 以像素为单位 */
    int iYMax;      /* 字符的高度 以像素为单位 */
    int iPitch;     /* bytes per line  对于单色位图 每行之间的跨度 （单位是字节） */  
    int iBpp;       /* bits per pixel 每个像素多少个字节表示  为了适配freetype位图的 MONO单像素模式（一个像素一个字节）和 ant-color模式（一个像素8个字节） */
    int iCurOriginX;
    int iCurOriginY;
    int iNextOriginX;
    int iNextOriginY;
    unsigned char* pucBuffer;
}FontBitMap, *PFontBitMap;


typedef struct FontOpr {
    char *name;
    int (*FontInit)(char* aFilename, unsigned int iFontSize);
    int (*GetFontBitMap)(unsigned int dwCode, PFontBitMap ptFontBitMap);
    struct FontOpr *ptNext;
} FontOpr, *PFontOpr;

void RegisterFont(PFontOpr ptFontOpr);
PFontOpr GetFontOpr(char *pucFontName);
void FontSystemRegister();
#endif // !_FONT_MANAGER_H
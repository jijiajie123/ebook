#include <ft2build.h>
#include "../include/font_manager.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H


int FreetypeInit(char* aFilename, unsigned int iFontSize);
int FreetypeGetFontBitMap(unsigned int dwCode, PFontBitMap ptFontBitMap);

static FT_Library    g_tLibrary;
static FT_Face       g_tFace;
static FT_Vector     g_tPen;
static FT_GlyphSlot  g_tSlot;

FontOpr g_tFreetypeOpr = {
    .name            = "freetype",
    .FontInit        = FreetypeInit,
    .GetFontBitMap   = FreetypeGetFontBitMap,
};

int FreetypeInit(char* aFilename, unsigned int iFontSize)
{
	int error;
    /* 显示矢量字体 */
	error = FT_Init_FreeType( &g_tLibrary );			   /* initialize library */
	/* error handling omitted */
    if(error)
    {
        DBG_PRINTF("FT_Init_FreeType error\n");
        return -1;
    }
	error = FT_New_Face( g_tLibrary, aFilename, 0, &g_tFace ); /* create face object */
    if(error)
    {
        DBG_PRINTF("FT_New_Face error\n");
        return -1;
    }
    /*设置字体大小，宽度高度多少个像素 */
	error =	FT_Set_Pixel_Sizes(g_tFace, iFontSize, 0); 
    if(error)
    {
        DBG_PRINTF("FT_Set_Pixel_Sizes error\n");
        return -1;
    }
    g_tSlot = g_tFace->glyph;

    return 0;
}


int FreetypeGetFontBitMap(unsigned int dwCode, PFontBitMap ptFontBitMap)
{
    int error;

    int iPenX;
    int iPenY;
    iPenX = ptFontBitMap->iCurOriginX;
    iPenY = ptFontBitMap->iCurOriginY;

    FT_Set_Transform(g_Face, 0, &g_tPen);
    error = FT_Load_Char(g_Face, dwCode, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
	if (error)
	{ 
			printf("FT_Load_Char error\n");
			return -1;
	}

    /* 坐标系变换 算出lcd坐标系下 位图的左上角坐标 */
    ptFontBitMap->iXLeft        = iPenX + g_tSlot->bitmap_left;
    ptFontBitMap->iYTop         = iPenY - g_tSlot->bitmap_top;
    ptFontBitMap->iXMax         = ptFontBitMap->iXLeft + g_tSlot->bitmap.width;  /* 并不是 iPenX + g_tSlot->bitmap_left  位图原点和屏幕左上角左对齐不一定在一条直线上 */
    ptFontBitMap->iYMax         = ptFontBitMap->iYTop + g_tSlot->bitmap.rows;   
    ptFontBitMap->iPitch        = g_tSlot->bitmap->pitch;  
    ptFontBitMap->iNextOriginX  = ptFontBitMap->iCurOriginX + g_tSlot->advance.x / 64;
    ptFontBitMap->iNextOriginY  = ptFontBitMap->iCurOriginY + g_tSlot->advance.y / 64;
    ptFontBitMap->iBpp          = 1;  /* 每个像素多少字节 */
    ptFontBitMap->iPitch        = g_tSlot->bitmap.pitch; /* 跨度 一行多少个字节 */
    ptFontBitMap->pucBuffer     = g_tSlot->bitmap.buffer;
     
    return 0;
}



 void FreetypeFontRegister()
 {
    RegisterFont(&g_tFreetypeOpr);
 }
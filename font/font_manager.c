#include "../include/font_manager.h"
#include "../include/disp_manager.h"
#include <string.h>
#include <stdio.h>

static PFontOpr g_ptFontOprs = NULL;


void RegisterFont(PFontOpr ptFontOpr)
{
    g_ptFontOprs->ptNext = g_ptFontOprs; 
    g_ptFontOprs = ptFontOpr;
}


PFontOpr GetFontOpr(char *pucFontName)
{

    PFontOpr ptTmp = g_ptFontOprs;
    while(ptTmp)
    {
        if(strcmp(ptTmp->name, pucFontName) == 0)
        {
            return ptTmp;
        }
        ptTmp = ptTmp->ptNext;
    }
    return NULL; 
}

void ShowFontOpr()
{
    int i = 0;
    PFontOpr ptTmp = g_ptFontOprs;
    while(ptTmp)
    {
        printf("%02d %s" , i++, ptTmp->name);
        ptTmp = ptTmp->ptNext;
    }
}

int FontSystemRegister()
{
    extern void FreetypeFontRegister();
    FreetypeFontRegister();
    extern void GBKFontRegister();
    GBKFontRegister();    
    extern void AsciiFontRegister();
    FreetypeFontRegister();    
    return 0;
    
}
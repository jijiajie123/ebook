#include "../include/disp_manager.h"
#include <string.h>

PDispOpr g_ptDispOprDevs = NULL;
PDispOpr g_tDispOprDefault;


void RegisterDisplay(PDispOpr ptDispOpr)
{
    ptDispOpr->ptNext = g_ptDispOprDevs;
    g_ptDispOprDevs = ptDispOpr;
}

PDispOpr GetDisOpr(char *name)
{
    PDispOpr ptTmp = g_ptDispOprDevs;
    while (ptTmp)
    {
        if (strcmp(ptTmp->name, name) == 0)
        {
            return ptTmp;
        }
        ptTmp = ptTmp->ptNext;
    }
    return NULL;
}
void ShowDispOpr(void)
{
    int i = 0;
    PDispOpr ptTmp = g_ptDispOprDevs;
    while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}
void DisplaySystemRegister()
{ 
    extern void FBDeviceRegister(void);
    FBDeviceRegister();
}
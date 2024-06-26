#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H

// #include <stdio.h>

// #include "config.h"


typedef struct DispOpr {
    char *name;
    int  iXres;
    int  iYres;
    int  iBpp;
    int  (*DeviceInit)();
    int  (*ShowPixel)(int x, int y, unsigned int dwColor);
    int  (*CleanScreen)(unsigned int dwBackColor);
    struct DispOpr *ptNext;

} DispOpr,*PDispOpr;

void RegisterDisplay(PDispOpr ptDispOpr);
PDispOpr GetDisOpr(char *name);
void ShowDispOpr(void);
int DisplaySystemRegister();

#endif // !_DISP_MANAGER_H

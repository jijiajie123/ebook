#ifndef _DRAW_H
#define _DRAW_H



/* 主要用于记录一页读取文件的开始地址和结束地址 
*  通过ShowOnePage函数读取该结构体 知道该从哪里读取文本文件内容
*  再通过ShowOneFont函数来显示字符，直到显示完一整页
*
*/

typedef struct PageDesc {
    /* 页面编号 */
    int iPage;
    /* LCD屏幕第一个字符在文件中的位置 */
    unsigned char *pucLcdFirstPosAtFile; 
    /* LCD屏幕下一页第一个字符在文件中的位置 */
    unsigned char *pucLcdNextPageFirstPosAtFile;
    /* 指向前一页的指针 */
    struct PageDesc *ptPrePage;
    /* 指向后一页的指针 */
    struct PageDesc *ptNextPage;
} PageDesc, *PPageDesc;




#endif
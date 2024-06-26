
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/config.h"
#include "../include/encoding_manager.h"
#include "../include/font_manager.h"
#include "../include/draw.h"
#include "../include/disp_manager.h"

/* 字母后面带有冒号表示后面跟参数 不带冒号表示不跟参数 
 * getopt(argc, argv, "ls:f:h:d:")
 * 例如 ./main  -l      ./main  -s 24
 * 参数存在optarg中
 * optind argv 的当前索引值。当getopt()在while循环中使用时，循环结束后，剩下的字串视为操作数，在argv[optind]至argv[argc-1]中可以找到
 * 解析完之后 getopt返回-1  剩下未解析的内容存储在argv[optind] 中
 */
 int main(int argc, char *argv[])
 {
    int iError;
    unsigned int dwFontSize = 16;
	char acFreetypeFile[128];
    char acHzkFile[128];
    char acDispFile[128];
    char acTextFile[128];
    char cOpr;
    int bList = 0;  /* 标记位  标记是否需要列出参数 */
    while ( (iError = getopt(argc, argv, "ls:f:h:d:")) != -1 )  /* s:size f:freetype h:hzk16 d:display */
    {
        switch (iError)
        {
            case 'l':
            {
                bList = 1;
                break;
            }
            case 's':
            {
                dwFontSize = strtol(optarg, NULL, 0);
                break;
            }
            case 'f':
            {
                strncpy(acFreetypeFile, optarg, 128);
                acFreetypeFile[127] = '\0';
                break;
            }
            case 'h':
            {
                strncpy(acHzkFile, optarg, 128);
                acHzkFile[127] = '\0';
                break;
            }
            case 'd':
            {
                strncpy(acDispFile, optarg, 128);
                acDispFile[127] = '\0';
                break;
            }
            default :
            {
                printf("Usage: %s [-s Size] [-d display] [-f font_size] [-h HZK] <text_file>\n", argv[0]);
                printf("Usage: %s -l\n", argv[0]);
                return -1;
                break;
            }
        }
    }

    /* 没输入选项-l 但是参数值没输入完整 即<text_file> 未指定 */
    if(!bList && (optind >= argc)) /* optind 初始值为 1 当所有的选项都处理完之后 argv[optind] 指向未处理的参数值 */
    {                
        printf("Usage: %s [-s Size] [-d display] [-f font_size] [-h HZK] <text_file>\n", argv[0]);
        printf("Usage: %s -l\n", argv[0]);
        return -1;
    }

    iError = DisplaySystemRegister();
    if(iError)
    {
        printf("DisplaySystemRegister error\n");
        return -1;
    }
    iError = FontSystemRegister();
    if(iError)
    {
        printf("FontSystemRegister error\n");
        return -1;
    }
    iError = EncodingSystemRegister();
    if(iError)
    {
        printf("EncodingSystemRegister error\n");
        return -1;

    }

    if(bList)
    {
        printf("support display:");
        ShowDispOpr();

    
        printf("support font:");
        ShowFontOpr();
        
        printf("support encoding:");
        ShowEncodingOpr();
        
        return 0;
    }

    strncpy(acTextFile, argv[optind], 128);
    acTextFile[127] = '\0';

    iError = OpenTextFile(acTextFile);
    if (iError)
    {
        printf("Open Textfile err!\n");
        return -1;
    }
    
    iError = SetFontDetail(acHzkFile, acFreetypeFile, dwFontSize);
    if(iError)
    {
        printf("Set Font Detail err!\n");
        return -1;
    }

    DBG_PRINTF("%s %S %d\n", __FILE__, __FUNCTION__, __LINE__);

    iError = SelectAndInitDisplay(acDispFile);
    if (iError)
    {
        printf("Select Disp err\n");
        return -1;
    }

    DBG_PRINTF("%s %S %d\n", __FILE__, __FUNCTION__, __LINE__);
    iError = ShowNextPage();
    DBG_PRINTF("%s %S %d\n", __FILE__, __FUNCTION__, __LINE__);
    if (iError)
    {
        printf("show first page err\n");
        return -1;
    }
    
    while (1)
    {
        printf("Enter 'n' to show next page, 'u' to show previous page, 'g' to exit\n");
        do
        {
            cOpr = getchar();
        } while (cOpr != 'n' && cOpr != 'u' && cOpr != 'g');
        if (cOpr == 'n')
        {
            ShowNextPage();
        }
        else if (cOpr == 'u')
        {
            ShowPrePage();
        }
        else
        {
            return 0;
        }
        
                

    }
 }
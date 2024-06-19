
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    unsigned int dwFontSize;
    unsigned int bList;
	char acFreetypeFile[128];
    char acHzkFile[128];
    char acDispFile[128];
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
        }
    }

}


    return 0;
 }
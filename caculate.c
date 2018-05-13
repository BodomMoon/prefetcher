#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#define D_Pmax 15
#define D_TestTime 32

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

int main()
{
    FILE *fp = fopen("Distance.txt","w");
    fclose(fp);
    char orderA[40];
    orderA[0]='\0';
    char temp[] = "./sse_prefetch_transpose_opt ";
    strcat(orderA,temp);
    //printf("%s\n",orderA );
    int avg[D_Pmax] , PFDIST;
    for(int Pcounter = 0;Pcounter<D_Pmax;Pcounter++){
        char time[5];
        avg[Pcounter] = 0;
        sprintf(time,"%d",Pcounter);
        strcat(orderA,time);
        for(int counter = 0;counter<D_TestTime;counter++){
            //printf("%s\n",orderA );
            system(orderA);
        }
        orderA[29] = '\0';
        printf("%d end\n",Pcounter);
    }
    //Get execution time end
    fp = fopen("Distance.txt","r");
    FILE *data = fopen("DistanceAVG.txt","w");
    for(int Pcounter = 0;Pcounter<D_Pmax;Pcounter++){
        for(int counter = 0;counter<D_TestTime;counter++){
            int temp;
            fscanf(fp,"%d",&temp);
            avg[Pcounter] += temp;
        }
        avg[Pcounter]/=D_TestTime;
        printf("PFDIST of %d is  %d\n", Pcounter,avg[Pcounter]);
        fprintf(data,"%d %d\n", Pcounter,avg[Pcounter]);
    }
    fclose(fp);
    fclose(data);

    int minNum = 0x7fffffff , tempNum;
    for(int counter=0;counter<D_Pmax;counter++){
        if( min(minNum,avg[counter]) != minNum)
            tempNum = counter;
        printf("%d\n", tempNum);
        minNum = min(minNum,avg[counter]);
    }
    fp = fopen("BestPFDIST.txt","w");
    fprintf(fp, "%d\n", tempNum);
    fclose(fp);


}
/*
流程

Make部份

1.MAKE 執行外部測試檔
2.外部測試檔重新產生 test.txt
3.外部測試freopen 並將stdio->test.txt stdout->out.txt
4.返回外部測試檔  外部測試檔讀取output.txt 取出clock部份資料並整理為clock.txt
5.MAKE 執行 caculate 計算標準差並取出信賴區間

編譯時必須使用  gcc bench.c `pkg-config --cflags --libs glib-2.0` -o bench
*/
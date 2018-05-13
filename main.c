#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>

#include <xmmintrin.h>


#define block_size 4

/* provide the implementations of naive_transpose,
 * sse_transpose, sse_prefetch_transpose
 */

#include IMPL

static long int diff_in_us(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000000000 + diff.tv_nsec  );
}

int main(int argc ,char *argv[])
{

#if (OPT==1)
        int TEST_W,TEST_H;
        if(argc != 0){
            FILE *fp = fopen("TEST_HW.txt","r");
            fscanf(fp,"%d",&TEST_W);
            TEST_H = TEST_W;
            fclose(fp);
        }
        else{
            TEST_H = 4096;
            TEST_W = 4096;
        }

#else
        #define TEST_W 4096
        #define TEST_H 4096
#endif
    /* verify the result of 4x4 matrix */
    {
        /*
        int testin[16] = { 0, 1,  2,  3,  4,  5,  6,  7,
                           8, 9, 10, 11, 12, 13, 14, 15
                         };
        int testout[16];
        int expected[16] = { 0, 4,  8, 12, 1, 5,  9, 13,
                             2, 6, 10, 14, 3, 7, 11, 15
                           };
        */
        //scanf("%d",&max);
        int max = 4; //use to define max

        assert( (max & (block_size-1)) == 0 &&
               "input is not a multiple of 4");
        int testin[max*max];
        int testout[max*max];
        int expected[max*max];
        for (int y = 0; y < max; y++) {
            for (int x = 0; x < max; x++){
                testin[y * max + x] = y * max + x;
                expected[y * max + x] = x * max + y;
            }
        }


        /*for (int y = 0; y < max; y++) {
            for (int x = 0; x < max; x++)
                printf(" %2d", testin[y * max + x]);
            printf("\n");
        }
        printf("\n");*/
#if defined(NAIVE)
        naive_transpose(testin, testout, max, max);
#elif defined(SSE)
        sse_transpose(testin, testout, max, max);
#elif defined(SSE_PREFETCH)
        #if(OPT == 1)
        sse_prefetch_transpose(testin, testout, max, max,8);
        #else
            sse_prefetch_transpose(testin, testout, max, max);
        #endif
#endif
        /*for (int y = 0; y < max; y++) {
            for (int x = 0; x < max; x++)
                printf(" %2d", testout[y * max + x]);
            printf("\n");
        }*/
        assert(0 == memcmp(testout, expected, max*max * sizeof(int)) &&
               "Verification fails");
    }
    //getchar();
    {
        struct timespec start, end;
        int *src  = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out0 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out1 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out2 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);

        srand(time(NULL));
        for (int y = 0; y < TEST_H; y++)
            for (int x = 0; x < TEST_W; x++)
                *(src + y * TEST_W + x) = rand();
#if defined(SSE_PREFETCH)
        #if(OPT == 1)
            int  PFDIST = (int)strtol(argv[1],NULL,10);
            //printf("P = %d\n", PFDIST);
            clock_gettime(CLOCK_REALTIME, &start);
            sse_prefetch_transpose(src, out0, TEST_W, TEST_H,PFDIST);
            clock_gettime(CLOCK_REALTIME, &end);
            FILE *fp = fopen("Distance.txt", "a");
            fprintf(fp,"%ld\n", diff_in_us(start, end));
        #else
            clock_gettime(CLOCK_REALTIME, &start);
            sse_prefetch_transpose(src, out0, TEST_W, TEST_H);
            clock_gettime(CLOCK_REALTIME, &end);
            printf("sse prefetch: \t %ld us\n", diff_in_us(start, end));
        #endif
#elif defined(SSE)
        clock_gettime(CLOCK_REALTIME, &start);
        sse_transpose(src, out1, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("sse: \t\t %ld us\n", diff_in_us(start, end));
#elif defined(NAIVE)
        clock_gettime(CLOCK_REALTIME, &start);
        naive_transpose(src, out2, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("naive: \t\t %ld us\n", diff_in_us(start, end));
#endif
        free(src);
        free(out0);
        free(out1);
        free(out2);
    }

    return 0;
}

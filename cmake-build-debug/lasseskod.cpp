#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

//#define PARALLEL

static int cmp(const void* ap, const void* bp)
{
    /* you need to modify this function to compare doubles. */
    double diff = *(double *)ap-*(double *)bp;
    if(diff==0){
        return 0;
    }
    else if(diff >0){
        return 1;
    }
    return -1;
}

static double sec(void)
{
    struct timespec requestStart;
    clock_gettime(CLOCK_REALTIME, &requestStart);
    return requestStart.tv_sec;

}

typedef struct{
    double* array;
    int index;
}Parameters;

static void* sort(void* param){
    Parameters* par = param;
    double* array=par->array;
//	printf("new thread a[0] %e\n", array[0]);
    qsort(array, par->index, sizeof array[0], cmp);
//	printf("new thread a[0] %e\n", array[0]);
}

void par_sort(
        double*		base,	// Array to sort.
        size_t		n,	// Number of elements in base.
        size_t		s,	// Size of each element.
        int		(cmp)(const void, const void*)) // Behaves like strcmp
{
    double piv0=base[0];
    double piv1=base[1];
    double piv2=base[2];

    if(piv0>piv1){
        piv1=piv0;
        piv0=base[1];
    }
    if(piv1>piv2){
        piv2=piv1;
        piv1=base[2];
    }
    if(piv0>piv1){
        double temp=piv0;
        piv0=piv1;
        piv1=temp;
    }

    printf("\npiv0:%e, \npiv1:%e, \npiv2:%e\n", piv0, piv1, piv2);

    double* a1;
    double* a2;
    double* a3;
    double* a4;
    a1=malloc(n * sizeof a1[0]);
    a2=malloc(n * sizeof a2[0]);
    a3=malloc(n * sizeof a3[0]);
    a4=malloc(n * sizeof a4[0]);
    int index1=0;
    int index2=0;
    int index3=0;
    int index4=0;


    for (int i = 0; i < n; ++i)
    {
        if(cmp(&base[i], &piv1) < 0){
            if(cmp(&base[i], &piv0) < 0){
                a1[index1] = base[i];
                ++index1;
            }else{
                a2[index2] = base[i];
                ++index2;
            }
        }
        else {
            if(cmp(&base[i], &piv2) < 0){
                a3[index3] = base[i];
                ++index3;
            }else{
                a4[index4] = base[i];
                ++index4;
            }
        }
    }

//	printf("%s\n", "a1");
//	for (int i = 0; i < n; ++i)
//	{
//		printf("%1.2f\n", a1[i]);
//	}
//	printf("%s\n", "a2");
//	for (int i = 0; i < n; ++i)
//	{
//		printf("%1.2f\n", a2[i]);
//	}
//	printf("%s\n", "a3");
//	for (int i = 0; i < n; ++i)
//	{
//		printf("%1.2f\n", a3[i]);
//	}
//	printf("%s\n", "a4");
//	for (int i = 0; i < n; ++i)
//	{
//		printf("%1.2f\n", a4[i]);
//	}




    pthread_t thread1;
    Parameters* t1Data = malloc(sizeof(Parameters));
    t1Data->array=malloc(sizeof(a1));
    t1Data->array=a1;
    t1Data->index=malloc(sizeof(index1));
    t1Data->index=index1;

    printf("a1 a[0] %e\n", a1[0]);
    printf("t1Data a[0] %e\n", (t1Data->array)[0]);

    pthread_create(&thread1, NULL, sort, (void *)t1Data);
    pthread_t thread2;
    Parameters* t2Data = malloc(sizeof(Parameters));
    t2Data->array=malloc(sizeof(a2));
    t2Data->array=a2;
    t2Data->index=malloc(sizeof(index2));
    t2Data->index=index2;
    pthread_create(&thread2, NULL, sort, (void *)t2Data);

    pthread_t thread3;
    Parameters* t3Data = malloc(sizeof(Parameters));
    t3Data->array=malloc(sizeof(a3));
    t3Data->array=a3;
    t3Data->index=malloc(sizeof(index3));
    t3Data->index=index3;
    pthread_create(&thread3, NULL, sort, (void *)t3Data);

    pthread_t thread4;
    Parameters* t4Data = malloc(sizeof(Parameters));
    t4Data->array=malloc(sizeof(a4));
    t4Data->array=a4;
    t4Data->index=malloc(sizeof(index4));
    t4Data->index=index4;
    pthread_create(&thread4, NULL, sort, (void *)t4Data);

//	qsort(a1, index1, sizeof a1[0], cmp);
//	qsort(a2, index2, sizeof a2[0], cmp);
//	qsort(a3, index3, sizeof a3[0], cmp);
//	qsort(a4, index4, sizeof a4[0], cmp);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);


//	printf("done? a[0] %e\n", t1Data->array[0]);
    for (int i = 0; i < index1; ++i)
    {
        base[i]=t1Data->array[i];
    }
    for (int i = 0; i < index2; ++i)
    {
        base[index1+i]=t2Data->array[i];
    }
    for (int i = 0; i < index3; ++i)
    {
        base[index1+index2+i]=t3Data->array[i];
    }
    for (int i = 0; i < index4; ++i)
    {
        base[index1+index2+index3+i]=t4Data->array[i];
    }
}


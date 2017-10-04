#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct{

} args;

static double sec(void)
{
    return clock();
}

static int cmp(const void* ap, const void* bp)
{
    return (*(double*)ap) - *((double*) bp);
}

void par_sort(
        void*		base,	// Array to sort.
        size_t		n,	// Number of elements in base.
        size_t		s,	// Size of each element.
        int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{
    int* pivots = malloc(sizeof(int)*3);
    pthread_t thread0;
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    double *unsorted = malloc(sizeof(double) * n);
    unsorted = (double *) base;
    int storeLow = 3;
    int storeMid = 3;
    int storeHigh = 3;
    int low = 0;
    int midLow = 0;
    int midHigh = 0;
    int high = 0;

    for(int i = 0; i<3;i++){
        pivots[i] = unsorted[i];
    }

    for(int j = 0; j < n; j++){
        if(base[j]<pivots[1]){
            double temp = unsorted[storeLow];
            unsorted[storeLow] = unsorted[i];
            unsorted[i] = temp;
            storeLow++;
        }
    }
    storeMid = storeLow;
    double temp = unsorted[storeLow - 1];
    unsorted[storeLow - 1] = pivot[1];
    unsorted[1] = temp;
    qsort(pivots, 3, sizeof(int), cmp);
}

int main(int ac, char** av)
{
    int		n = 2000000;
    int		i;
    double*		a;
    double		start, end;

    if (ac > 1)
        sscanf(av[1], "%d", &n);

    srand(getpid());

    a = malloc(n * sizeof a[0]);
    for (i = 0; i < n; i++)
        a[i] = rand();

    start = sec();

#ifdef PARALLEL
    par_sort(a, n, sizeof a[0], cmp);
#else
    qsort(a, n, sizeof a[0], cmp);
#endif

    end = sec();

    printf("%1.2f s\n", end - start);

    free(a);

    return 0;
}
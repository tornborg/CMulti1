#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>
#define PARALLEL 1;

int threadscreated;
pthread_mutex_t threadMut;

void* par_sort;

typedef struct {
    double* list;
    int n;
    int s;
}arguments;

static double sec(void)
{
    return clock();
}

static int cmp(const void* ap, const void* bp)
{
    return (*(double*)ap) - *((double*) bp);
}

/** void* par_thread(void* argum){
    arguments* things = argum;
    par_sort(things->list, things->n, things->s, cmp, things->threads);
} **/

void* par_sort(
                  void*		base,	// Array to sort.
                  size_t		n,	// Number of elements in base.
                  size_t		s,	// Size of each element.
                  int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{
    int pivot = 0;
    int storeIndex = 1;
    pthread_t thread0;
    pthread_t thread1;
    double *unsorted = malloc(sizeof(double) * n);
    unsorted = (double *) base;

    for (int i = 0; i < n; i++) {
        if (unsorted[i] < unsorted[pivot]) {
            double temp = unsorted[storeIndex];
            unsorted[storeIndex] = unsorted[i];
            unsorted[i] = temp;
            storeIndex++;
        }
    }

    double temp = unsorted[storeIndex - 1];
    unsorted[storeIndex - 1] = unsorted[pivot];
    unsorted[pivot] = temp;
    double *lower = malloc(sizeof(double)*storeIndex);
    int j = 0;
    double *upper = malloc(sizeof(double)*(n - storeIndex));
    if (threadscreated < 4) {

        for (int i = 0; i < n; i++) {
            if (i < storeIndex) {
                lower[i] = unsorted[i];
            } else {
                upper[j] = unsorted[i];
                j++;
            }
        }

        arguments *args1 = malloc(sizeof(arguments));
        arguments *args2 = malloc(sizeof(arguments));
        printf("%d: size of list \n", n);
        args1->list = malloc(sizeof(lower));
        args1->list = lower;
        args1->n = storeIndex;
        args1->s = s;


        args2->list = malloc(sizeof(upper));
        args2->list = upper;
        args2->n = (n - storeIndex);
        args2->s = s;

        printf("%d: size of s \n", s);

        if (threadscreated < 1) {
            pthread_mutex_lock(&threadMut);
            threadscreated = 2;
            pthread_mutex_unlock(&threadMut);
            pthread_create(&thread0, NULL, par_sort, (void *) args1);
            pthread_create(&thread1, NULL, par_sort, (void *) args2);
            pthread_join(thread1, NULL);
        } else if (threadscreated < 4) {
            pthread_mutex_lock(&threadMut);
            threadscreated += 1;
            pthread_mutex_unlock(&threadMut);
            pthread_create(&thread, NULL, qsort, (void *) args1);
            qsort(upper, n, s, cmp);

            pthread_join(thread, NULL);
        }
    } else {
            qsort(unsorted, n, s, cmp);
        }

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

    printf("%1.2f s\n", (end - start)/CLOCKS_PER_SEC);
    for(int i = 1; i<n; i++){
        //printf("%1.2f \n", a[i]);
        assert(a[i-1]-a[i]>=0);

    }
    free(a);

    return 0;
}

#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>

static double sec(void)
{
    return clock();
}

void par_sort(
                  void*		base,	// Array to sort.
                  size_t		n,	// Number of elements in base.
                  size_t		s,	// Size of each element.
                  int		(*cmp)(const void*, const void*), int threads) // Behaves like strcmp
{   int pivot = sizeof(base) - 1;
    int storeIndex = pivot - 1;
    double* unsorted = (double*) base;
    double temp;
      for(int i = 0; i < n; i++){
          if(unsorted[i] > unsorted[pivot]){
              temp = unsorted[storeIndex];
              unsorted[storeIndex] = unsorted[i];
              unsorted[i] = temp;
              storeIndex++;
          }
      }
    temp = unsorted[storeIndex];
    unsorted[storeIndex] = unsorted[pivot];
    unsorted[pivot] = temp;
    double lower[storeIndex];
    double upper[n-storeIndex];
    if(threads < 4) {
        for (int i = 0; i < n; i++) {
            if (i < storeIndex) {
                lower[i] = unsorted[i];
            } else {
                upper[i] = unsorted[i];
            }
        }
        if (threads < 1) {
            threads +=2;
            pthread_create(par_sort(lower, storeIndex, s, cmp, threads));
            pthread_create(par_sort(upper, n-storeIndex, s, cmp, threads));
        } else {
            threads++;
            pthread_create(par_sort(lower, storeIndex, s, cmp, threads));
            qsort(upper, n, s, cmp);
    }
        pthread_join;
    } else{
            qsort(unsorted, n, s, cmp);
        }




}

static int cmp(const void* ap, const void* bp)
{
    if(ap > bp) return 1;
    else if (bp > ap) return -1;
    return 0;
}


int main(int ac, char** av)
{
    int		n = 2000000;
    int		i;
    double*		a;
    double		start, end;
    int threadscreated = 0;


    if (ac > 1)
        sscanf(av[1], "%d", &n);

    srand(getpid());

    a = malloc(n * sizeof a[0]);
    for (i = 0; i < n; i++)
        a[i] = rand();

    start = sec();

#ifdef PARALLEL
    par_sort(a, n, sizeof a[0], cmp, threadscreated);
#else
    qsort(a, n, sizeof a[0], cmp);
#endif

    end = sec();

    printf("%1.2f s\n", (end - start)/CLOCKS_PER_SEC);

    free(a);

    return 0;
}

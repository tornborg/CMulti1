#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct {
    double* list;
    int n;
    int s;
    int threads;
}arguments;

static double sec(void)
{
    return clock();
}



void par_sort(
                  void*		base,	// Array to sort.
                  size_t		n,	// Number of elements in base.
                  size_t		s,	// Size of each element.
                  int		(*cmp)(const void*, const void*),
                  int threads) // Behaves like strcmp
{   int pivot = sizeof(base) - 1;
    int storeIndex = pivot - 1;
    pthread_t thread0;
    pthread_t thread1;
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


        arguments* args1 = malloc(sizeof(arguments));
        arguments* args2 = malloc(sizeof(arguments));

        args1->list = malloc(sizeof(lower));
        args1->list = lower;

        args1->n = storeIndex;
        args1->s = s;
        args1->threads = threads;

        args2->list = upper;
        args2.n = n-storeIndex;
        args2.s = s;
        args2.cmp = cmp;
        args2.threads = threads;

        if (threads < 1) {
            threads +=2;

            pthread_create(&thread0, NULL, par_sort, &args1);
            pthread_create(&thread1, NULL, par_sort, &args2);
        } else {
            threads++;
            pthread_create(&thread0, NULL, par_sort, &args1);
            qsort(upper, n, s, cmp);
    }
        pthread_join(thread0, NULL);
        pthread_join(thread1, NULL);
    } else{
            qsort(unsorted, n, s, cmp);
        }




}

void par_thread(void* argum){
    arguents* things = argum;
    things = (arguments) argum;
    par_sort(things->list, things->n, things->s, things->cmp, things->threads);
}

static int cmp(const void* ap, const void* bp)
{
    return (*(double*)ap) - *((double*) bp);
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
    for(int i = 0; i<50; i++)
        printf("%1.2f s\n", a[i]);
    free(a);

    return 0;
}

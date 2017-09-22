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

typedef struct {
    double* list;
    int n;
    int s;
    volatile int threads;
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
                  int		(*cmp)(const void*, const void*),
                  volatile int threads,
                pthread_mutex_t lock) // Behaves like strcmp
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
    if (threads < 4) {
        printf("print 2: %d\n", threads);

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
        args1->list = malloc(sizeof(lower));
        args1->list = lower;
        args1->n = storeIndex;
        args1->s = s;
        args1->threads = threads;

        args2->list = upper;
        args2->n = n - storeIndex;
        args2->s = s;
        args2->threads = threads;
        if (threads < 1) {
            printf(" Two more threads \n");
            pthread_mutex_trylock(&lock);
            threads += 2;
            printf("Inne i låset \n")
            pthread_mutex_unlock(&lock);

            pthread_create(&thread0, NULL, par_sort, &args1);
            pthread_create(&thread1, NULL, par_sort, &args2);
            pthread_join(thread1, NULL);
        } else {
            printf(" One more Thread \n");
            pthread_mutex_trylock(&lock);
            threads +=1;
            pthread_mutex_unlock(&lock);
            pthread_create(&thread0, NULL, par_sort, &args1);
            qsort(upper, n, s, cmp);
        }
        pthread_join(thread0, NULL);
    } else {
        printf("print 3: %d\n", threads);
        printf(" sort this shit ");
        qsort(unsorted, n, s, cmp);
    }

}


int main(int ac, char** av)
{
    int		n = 100;
    int		i;
    double*		a;
    double		start, end;
    volatile int threadscreated = 0;
    pthread_mutex_t lock;


    if (ac > 1)
        sscanf(av[1], "%d", &n);

    srand(getpid());

    a = malloc(n * sizeof a[0]);
    for (i = 0; i < n; i++)
        a[i] = rand();

    start = sec();

#ifdef PARALLEL
    par_sort(a, n, sizeof a[0], cmp, threadscreated,lock);
#else
    printf("Sorternig sker\n");
    qsort(a, n, sizeof a[0], cmp);
#endif

    end = sec();

    printf("%1.2f s\n", (end - start)/CLOCKS_PER_SEC);
    for(int i = 1; i<n; i++){
        printf("%1.2f \n", a[i]);
      }
    assert(a[i-1]-a[i]>0);
    free(a);

    return 0;
}

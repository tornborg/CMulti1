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
        int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{   ptrhread_t thread0;
  void *thread0(void *param)
  pthread_create(thread0);

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

    free(a);

    return 0;
}

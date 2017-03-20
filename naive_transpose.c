#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>

#define TEST_H 4096
#define TEST_W 4096

typedef struct _prefetch_object prefetch;
typedef void (*func_t) (prefetch *);

struct _prefetch_object {
    int *src;
    int *out;
    func_t transpose;
};

static void naive_transpose_object(prefetch *p)
{
    for (int x = 0; x < TEST_W; x++)
        for (int y = 0; y < TEST_H; y++)
            *(p->out + x * TEST_H + y) = *(p->src + y * TEST_W + x);
}

void init_prefetch(prefetch **self)
{
    *self = malloc(sizeof(prefetch));
    (*self)->src = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
    (*self)->out = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
    (*self)->transpose = naive_transpose_object;
    srand(time(NULL));
    for (int y = 0; y < TEST_H; y++)
        for (int x = 0; x < TEST_W; x++)
            *((*self)->src + y * TEST_W + x) = rand();
}

void clean(prefetch **self)
{
    free((*self)->out);
    free((*self)->src);
}

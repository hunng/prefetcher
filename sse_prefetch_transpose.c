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

static void sse_prefetch_transpose_object(prefetch *p)
{
    for (int x = 0; x < TEST_W; x += 4) {
        for (int y = 0; y < TEST_H; y += 4) {
#define PFDIST  8
            _mm_prefetch(p->src+(y + PFDIST + 0) *TEST_W + x, _MM_HINT_T1);
            _mm_prefetch(p->src+(y + PFDIST + 1) *TEST_W + x, _MM_HINT_T1);
            _mm_prefetch(p->src+(y + PFDIST + 2) *TEST_W + x, _MM_HINT_T1);
            _mm_prefetch(p->src+(y + PFDIST + 3) *TEST_W + x, _MM_HINT_T1);

            __m128i I0 = _mm_loadu_si128 ((__m128i *)(p->src + (y + 0) * TEST_W + x));
            __m128i I1 = _mm_loadu_si128 ((__m128i *)(p->src + (y + 1) * TEST_W + x));
            __m128i I2 = _mm_loadu_si128 ((__m128i *)(p->src + (y + 2) * TEST_W + x));
            __m128i I3 = _mm_loadu_si128 ((__m128i *)(p->src + (y + 3) * TEST_W + x));
            __m128i T0 = _mm_unpacklo_epi32(I0, I1);
            __m128i T1 = _mm_unpacklo_epi32(I2, I3);
            __m128i T2 = _mm_unpackhi_epi32(I0, I1);
            __m128i T3 = _mm_unpackhi_epi32(I2, I3);
            I0 = _mm_unpacklo_epi64(T0, T1);
            I1 = _mm_unpackhi_epi64(T0, T1);
            I2 = _mm_unpacklo_epi64(T2, T3);
            I3 = _mm_unpackhi_epi64(T2, T3);
            _mm_storeu_si128((__m128i *)(p->out + ((x + 0) * TEST_H) + y), I0);
            _mm_storeu_si128((__m128i *)(p->out + ((x + 1) * TEST_H) + y), I1);
            _mm_storeu_si128((__m128i *)(p->out + ((x + 2) * TEST_H) + y), I2);
            _mm_storeu_si128((__m128i *)(p->out + ((x + 3) * TEST_H) + y), I3);
        }
    }
}

void init_prefetch(prefetch **self)
{
    *self = malloc(sizeof(prefetch));
    (*self)->src = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
    (*self)->out = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
    (*self)->transpose = sse_prefetch_transpose_object;
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

CFLAGS = -msse2 --std gnu99 -O0 -Wall -Wextra

SRCS_COMMON = main.c

EXEC = naive_transpose sse_transpose sse_prefetch_transpose

GIT_HOOKS := .git/hooks/applied

naive_transpose: $(SRCS_COMMON) naive_transpose.c
	$(CC) $(CFLAGS) \
		-DIMPL="\"$@.c\"" \
		-o $@ main.c

sse_transpose: $(SRCS_COMMON) sse_transpose.c
	$(CC) $(CFLAGS) \
		-DIMPL="\"$@.c\"" \
		-o $@ main.c

sse_prefetch_transpose: $(SRCS_COMMON) sse_prefetch_transpose.c
	$(CC) $(CFLAGS) \
		-DIMPL="\"$@.c\"" \
		-o $@ main.c

cache-test: $(EXEC)
	perf stat --repeat 10 \
		-e cache-misses,cache-references,instructions,cycles \
		./naive_transpose
	perf stat --repeat 10 \
		-e cache-misses,cache-references,instructions,cycles \
		./sse_transpose
	perf stat --repeat 10 \
		-e cache-misses,cache-references,instructions,cycles \
		./sse_prefetch_transpose

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

clean:
	$(RM) EXEC

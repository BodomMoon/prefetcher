CFLAGS = -msse2 --std gnu99 -O0 -Wall -Wextra

GIT_HOOKS := .git/hooks/applied

EXEC = naive_transpose sse_transpose sse_prefetch_transpose sse_transpose_row sse_transpose_noloop

all: $(EXEC)

SRCS_common = main.c

naive_transpose: $(SRCS_common) naive_transpose.c 
	$(CC) $(CFLAGS) \
    -DIMPL="\"$@.c\"" -o $@ \
    $(SRCS_common) 

sse_transpose: $(SRCS_common) sse_transpose.c 
	$(CC) $(CFLAGS) \
	    -DIMPL="\"$@.c\"" -o $@ \
	    -D OPT=0 $(SRCS_common)

sse_transpose_row: $(SRCS_common) sse_transpose.c 
	$(CC) $(CFLAGS) \
	    -DIMPL="\"sse_transpose.c\"" -o $@ \
	    -D OPT=1 $(SRCS_common) 


sse_prefetch_transpose: $(SRCS_common) sse_prefetch_transpose.c 
	$(CC) $(CFLAGS) \
	    -DIMPL="\"$@.c\"" -o $@ \
	    $(SRCS_common) 

run: $(EXEC)
	./naive_transpose
	./sse_transpose
	./sse_prefetch_transpose

sse-cache-test: $(EXEC)
	perf stat --repeat 20 -e cache-misses,cache-references,instructions,cycles,L1-dcache-loads,L1-dcache-load-misses,L1-dcache-prefetches,L1-dcache-prefetch-misses,LLC-loads,LLC-load-misses,LLC-prefetches,LLC-prefetch-misses,r02D1,r10D1,r04D1,r20D1 ./sse_transpose
	perf stat --repeat 20 -e cache-misses,cache-references,instructions,cycles,L1-dcache-loads,L1-dcache-load-misses,L1-dcache-prefetches,L1-dcache-prefetch-misses,LLC-loads,LLC-load-misses,LLC-prefetches,LLC-prefetch-misses,r02D1,r10D1,r04D1,r20D1 ./sse_transpose_row

cache-test: $(EXEC)
	perf stat --repeat 10 \
		-e cache-misses,cache-references,instructions,cycles,L1-dcache-loads,L1-dcache-load-misses,L1-dcache-prefetches,L1-dcache-prefetch-misses,LLC-loads,LLC-load-misses,LLC-prefetches,LLC-prefetch-misses \
        ./naive_transpose
	perf stat --repeat 10 -e cache-misses,cache-references,instructions,cycles,L1-dcache-loads,L1-dcache-load-misses,L1-dcache-prefetches,L1-dcache-prefetch-misses,LLC-loads,LLC-load-misses,LLC-prefetches,LLC-prefetch-misses,r02D1,r10D1,r04D1,r20D1 ./sse_transpose_row
	perf stat --repeat 10 \
		-e cache-misses,cache-references,instructions,cycles,L1-dcache-loads,L1-dcache-load-misses,L1-dcache-prefetches,L1-dcache-prefetch-misses,LLC-loads,LLC-load-misses,LLC-prefetches,LLC-prefetch-misses \
        ./sse_prefetch_transpose

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo


clean:
	$(RM) main $(EXEC) *.o perf.*


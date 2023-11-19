.SILENT:
.DEFAULT_GOAL:=build

.PHONY: run build clean

FLAGS:=-Wall -g -O2

build: bench_orig.c bench_rdtscp.c common.c common.h
	gcc $(FLAGS) -o bench_orig bench_orig.c common.c
	gcc $(FLAGS) -o bench_rdtscp bench_rdtscp.c common.c

clean:
	rm -rf bench_orig bench_rdtscp

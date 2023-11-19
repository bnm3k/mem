.SILENT:
.DEFAULT_GOAL:=help

.PHONY: help clean

FLAGS:=-Wall -g -O2

help:
	echo 'paoloni bench'

bench_orig: bench_orig.c common.c
	gcc $(FLAGS) -o bench_orig $^

bench_rdtscp: bench_rdtscp.c common.c
	gcc $(FLAGS) -o bench_rdtscp $^

bench_lfence: bench_lfence.c common.c
	gcc $(FLAGS) -o bench_lfence $^

clean:
	rm -rf bench_orig bench_rdtscp

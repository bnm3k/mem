.SILENT:
.DEFAULT_GOAL:=run

.PHONY: run build clean bench

FLAGS:=-Wall -g -O2 -D _GNU_SOURCE
BIN:=a.out

$(BIN): c_src/main.c c_src/bench.c c_src/clock.c
	gcc $(FLAGS) -o $@ $^ -lrt

build: $(BIN)

run: $(BIN)
	./$^

bench: $(BIN)
	rm -f results/results.csv
	./$^ | tee results/results.csv

clean:
	rm -rf a.out

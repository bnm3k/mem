.SILENT:
.DEFAULT_GOAL:=build

.PHONY: run build clean bench

FLAGS:=-Wall -g -O2 -D _GNU_SOURCE
BIN:=a.out

$(BIN): src/main.c src/bench.c src/clock.c
	gcc $(FLAGS) -o $@ $^ -lrt

build: $(BIN)

bench: $(BIN)
	rm -f results/results.csv
	./$^ | tee results/results.csv

clean:
	rm -rf a.out

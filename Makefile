.SILENT:
.DEFAULT_GOAL:=run

.PHONY: run build clean bench

FLAGS:=-Wall -g -O2 -D _GNU_SOURCE
BIN:=a.out

$(BIN): src/main.c src/bench.c src/clock.c
	gcc $(FLAGS) -o $@ $^ -lrt

build: $(BIN)

run: $(BIN)
	./$^

bench: $(BIN)
	rm -f results/results.csv
	./$^ > results/results.csv

add_results:
	python3 results/add_results.py

clean:
	rm -rf a.out

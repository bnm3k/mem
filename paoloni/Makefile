.SILENT:
.DEFAULT_GOAL:=run

.PHONY: run build clean

FLAGS:=-Wall -g -O2
BIN:=bench

$(BIN): ./bench.c
	gcc $(FLAGS) -o $@ $^

build: $(BIN)

run: $(BIN)
	./$^

clean:
	rm -rf bench

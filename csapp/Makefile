.SILENT:
.DEFAULT_GOAL:=run

.PHONY: build run clean

FLAGS:=-Wall -g -O3
BIN:=bench_csapp

$(BIN): clock.c fcyc.c main.c
	gcc $(FLAGS) -o $(BIN) $^

build: $(BIN)

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(BIN)

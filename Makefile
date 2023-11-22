.SILENT:
.DEFAULT_GOAL:=run

.PHONY: run build clean

FLAGS:=-Wall -g -O2 -D _GNU_SOURCE
BIN:=a.out

$(BIN): src/main.c
	gcc $(FLAGS) -o $@ $^

build: $(BIN)

run: $(BIN)
	./$^

clean:
	rm -rf a.out

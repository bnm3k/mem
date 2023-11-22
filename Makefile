.SILENT:
.DEFAULT_GOAL:=run

.PHONY: run build clean

FLAGS:=-Wall -g -O2
BIN:=a.out

$(BIN): src/main.c
	gcc $(FLAGS) -o $@ $^

build: $(BIN)

run: $(BIN)
	taskset -cp 4 ./$^

clean:
	rm -rf a.out

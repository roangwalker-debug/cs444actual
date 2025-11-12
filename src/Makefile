# Makefile at repo root (or adjust paths)
CFLAGS = -std=c11 -O0 -g
SRC    = src/text_utils.c src/file_ops.c src/task_runner.c
OBJ    = $(SRC:.c=.o)

all: app

app: $(OBJ)
	$(CC) $(CFLAGS) -o app $(OBJ)

clean:
	rm -f app $(OBJ)


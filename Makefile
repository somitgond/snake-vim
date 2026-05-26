
all: snake.h snake.c
	gcc snake.h snake.c -o snake -lraylib -lGL -lm -lpthread -ldl -lrt -lX11



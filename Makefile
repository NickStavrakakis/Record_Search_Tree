OUT = myfind
OBJS = $(OUT) splitter_merger_node leaf_node
SOURCE = main.c splitter_merger_node.c leaf_node.c
HEADER = header.h mytime.h
OUT = myfind
CC = gcc
FLAGS= -lm -o
CLEAR = clear_screen

all: $(CLEAR) $(OBJS)

clear_screen:
	clear

$(OUT): main.c
	$(CC) main.c $(FLAGS) $(OUT)

splitter_merger_node: splitter_merger_node.c
	$(CC) splitter_merger_node.c $(FLAGS) splitter_merger_node

leaf_node: leaf_node.c
	$(CC) leaf_node.c $(FLAGS) leaf_node

clean:
	rm -f $(OBJS) $(OUT)
	rm -f a*
	rm -f b*
	rm -f root*

CC = gcc
CFLAGS = -I.

chess: boardRepresentation.o moveGenerator.o moveStack.o
	$(CC) -o chess boardRepresentation.o moveGenerator.o moveStack.o $(CFLAGS)
	chess

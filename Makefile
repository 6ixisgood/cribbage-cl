TARGET=main
OBJS=Deck.o Card.o main.o Player.o Board.o HumanPlayer.o ComputerPlayer.o
CC=g++
CPPFLAGS=-g

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) -o $(TARGET) $(OBJS)

clean:
	@rm -f $(OBJS) $(TARGET)

Deck.o: $(@:.o=.c) Card.h Deck.h

Card.o: $(@:.o=.c) Card.h

Player.o: $(@:.o=.c) Card.h

Board.o: $(@:.o=.c) Card.h Deck.h Player.h Board.h

HumanPlayer.o: $(@:.o=.c) Player.h HumanPlayer.h

ComputerPlayer.o: $(@:.o=.c) Player.h ComputerPlayer.h

main.o: $(@:.o=.c) Card.h Deck.h


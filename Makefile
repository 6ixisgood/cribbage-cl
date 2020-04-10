TARGET=main.out
OBJS=Deck.o Card.o main.o Player.o NPlayer.o Board.o LocalBoard.o HumanPlayer.o ComputerPlayer.o nutils.o
LIBS=-lncursesw
CC=g++
CPPFLAGS=-g -std=c++11

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

clean:
	@rm -f $(OBJS) $(TARGET)

Deck.o: $(@:.o=.c) Card.h Deck.h

Card.o: $(@:.o=.c) Card.h

Player.o: $(@:.o=.c) Card.h

NPlayer.o: $(@:.o=.c) NPlayer.h nutils.h

Board.o: $(@:.o=.c) Card.h Deck.h Player.h Board.h

LocalBoard.o: $(@:.o=.c) Board.h LocalBoard.h

HumanPlayer.o: $(@:.o=.c) Player.h HumanPlayer.h

ComputerPlayer.o: $(@:.o=.c) Player.h ComputerPlayer.h

nutils.o: $(@:.o=.c) nutils.h

main.o: $(@:.o=.c) Card.h Deck.h LocalBoard.h splash.xbm nutils.h

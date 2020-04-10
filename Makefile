TARGET=bin/cribbage
SRCDIR=src
BUILDDIR=build
SRC=$(wildcard src/*.cpp)
SRCEXT=cpp
OBJS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRC:.$(SRCEXT)=.o))
DEPS=$(OBJS:.o=.d))
LIB=-lncursesw
CC=g++
CFLAGS=-g -std=c++11
INC=-I include

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

-include $(DEPS)

$(BUILDDIR)/%.d: $(SRCDIR)/%.$(SRCEXT)
	@$(CPP) $(CFLAGS) $(INC) $< -MM -MT $(@:.d=.o) >$@

clean:
	$(RM) -r $(BUILDDIR)/* $(TARGET)
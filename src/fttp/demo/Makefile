DEMO	:= fttp
OBJS	:= $(patsubst %.c, %.o, $(wildcard *.c))

LIBDIR	:= ../lib
INCLUDE	:= ../include
TARDIR	:= ../bin
CFLAGS	:= -Wall -O2 -g -I$(INCLUDE)
LDFLAGS	:= -lpthread -L$(LIBDIR) -lfttp


all: $(OBJS)
	$(CC) $(OBJS) -o $(DEMO) $(LDFLAGS)
	mv $(DEMO) $(TARDIR)

clean:
	rm -f $(TARDIR)/$(DEMO) $(OBJS)

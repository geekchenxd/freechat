SUBDIRS	:= 
.PHONY:	$(SUBDIRS)

APPS	:= freechat
OBJS	:= $(patsubst %.c, %.o, $(wildcard *.c))

CROSS	:=
#CROSS	?=
CC	:= $(CROSS)gcc
AR	:= $(CROSS)ar
TARGETDIR:= $(PWD)
INCLUDE	:= $(PWD)/include
CFLAGS	:= -Wall -O2 -g -I$(INCLUDE)
LDFLAGS	:= -lpthread -lncursesw -lconfig #-L$(PWD)/lib -liec101
TOPDIR	:= $(PWD)

export CFLAGS TOPDIR CC AR

all: $(OBJS)
	@for dir in $(SUBDIRS) ; \
		do $(MAKE) -C $$dir all CC=$(CC) AR=$(AR); \
	done

	$(CC) $(OBJS) -o $(APPS) $(LDFLAGS)
	size $(APPS)
	#mv $(APPS) $(TARGETDIR)

clean:
	rm -f $(APPS) $(OBJS)

	@for dir in $(SUBDIRS); \
		do $(MAKE) -C $$dir clean; \
	done

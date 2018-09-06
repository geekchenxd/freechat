SUBDIRS	:= src
.PHONY:	$(SUBDIRS)

APPS	:= fttp
OBJS	:= $(patsubst %.c, %.o, $(wildcard *.c))

#CROSS	:= arm-embedsky-linux-gnueabi-
CROSS	?=
CC	:= $(CROSS)gcc
AR	:= $(CROSS)ar
TARGETDIR:= $(PWD)/bin
INCLUDE	:= $(PWD)/include
CFLAGS	:= -Wall -O2 -g -I$(INCLUDE)
LDFLAGS	:= -lpthread -L$(PWD)/lib -lfttp
TOPDIR	:= $(PWD)

export CFLAGS TOPDIR CC AR

all: $(OBJS)
	@for dir in $(SUBDIRS) ; \
		do $(MAKE) -C $$dir all CC=$(CC) AR=$(AR); \
	done

	$(CC) $(OBJS) -o $(APPS) $(LDFLAGS)
	size $(APPS)

clean:
	rm -f $(APPS) $(OBJS)

	@for dir in $(SUBDIRS); \
		do $(MAKE) -C $$dir clean; \
	done

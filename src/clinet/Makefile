SUBDIRS	:= 
.PHONY:	$(SUBDIRS)

APPS	:= freechat
OBJS	:= $(patsubst %.c, %.o, $(wildcard *.c))

CROSS	:=
#CROSS	?=
CC	:= $(CROSS)gcc
AR	:= $(CROSS)ar
TARGETDIR:= $(PWD)
FTTP_DIR := $(PWD)/../fttp
FTTP_INC := $(FTTP_DIR)/include
FTTP_LIB := $(FTTP_DIR)/lib/
INCLUDE1 := $(PWD)/include
INCLUDE2 := $(FTTP_INC)
INCLUDES = -I$(INCLUDE1) -I$(INCLUDE2)

CFLAGS	:= -Wall -O2 -g $(INCLUDES)
LDFLAGS	:= -lpthread -lncursesw -lconfig -lssl -lcrypto -L$(FTTP_LIB) -lfttp
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

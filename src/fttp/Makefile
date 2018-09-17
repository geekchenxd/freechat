SUBDIRS	:= src
DEMODIR	:= demo
.PHONY:	$(SUBDIRS)

DEMO	:= fttp
OBJS	:= $(patsubst %.c, %.o, $(wildcard *.c))

CROSS	?=
CC	:= $(CROSS)gcc
AR	:= $(CROSS)ar
TARGETDIR:= $(PWD)/bin
INCLUDE1 := $(PWD)/include
INCLUDE2 := $(PWD)/../client/include/
INCLUDES := -I$(INCLUDE1) -I$(INCLUDE2)
CFLAGS	:= -Wall -O2 -g $(INCLUDES)
LDFLAGS	:= -lpthread -L$(PWD)/lib -lfttp
TOPDIR	:= $(PWD)

export CFLAGS TOPDIR CC AR

all: $(OBJS)
	@for dir in $(SUBDIRS) ; \
		do $(MAKE) -C $$dir all CC=$(CC) AR=$(AR); \
	done

demo:
	$(MAKE) -C $(DEMODIR) all CC=$(CC)

clean:
	rm -f $(APPS) $(OBJS)

	@for dir in $(SUBDIRS); \
		do $(MAKE) -C $$dir clean; \
	done

	$(MAKE) -C $(DEMODIR) clean

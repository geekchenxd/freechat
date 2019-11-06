# Default compiler settings
WARNINGS = -Wall
LD_LIBS :=

#CROSS_COMPILE :=
ifeq "$(origin CROSS_COMPILE)" "undefined"
	CROSS_COMPILE = arm-linux-gnueabihf-
endif

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
STRIP = $(CROSS_COMPILE)strip
RANLIB = $(CROSS_COMPILE)ranlib
LD = $(CROSS_COMPILE)ld

SRC_DIR := $(shell pwd)
INCLUDE_DIR := $(SRC_DIR)
LIBCONFIG := config

CFLAGS  += $(WARNINGS) -O2
LDFLAGS += $(LD_LIBS) $(CONF_LDFLAGS)
CFLAGS += -I$(INCLUDE_DIR)
TARGET := lib$(LIBCONFIG).a

v_cc = @echo "  [CC]  " $@;
v_ar = @echo "  [AR]  " $@;

default:all

OBJS := $(patsubst %.c, %.o, $(wildcard *.c))

$(TARGET): $(OBJS)
	$(v_ar)
	@$(AR) cr -o $(TARGET) $(OBJS)

target_check=

install:
ifdef LIBDIR
	@echo "LIBDIR:$(LIBDIR)"
	$(shell if [ -a $(TARGET) ];then \
	   	mkdir -p $(LIBDIR); \
		mv $(TARGET) $(LIBDIR); \
	fi;)
endif

all:$(TARGET) install

clean:
	@$(RM) -r $(OBJS) $(TARGET) $(LIBDIR)

%.o:%.c
	$(v_cc)
	@$(CC) $(CFLAGS) -c -o $@ $<



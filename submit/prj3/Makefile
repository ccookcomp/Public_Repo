COURSE = cs551

PROJECT = prj3

COURSE_DIR = $(HOME)/$(COURSE)

INCLUDE_DIR = $(COURSE_DIR)/include
LIB_DIR = $(COURSE_DIR)/lib

CFLAGS = -g  -Wall -Wextra -fms-extensions -std=c11
CPPFLAGS= -D_POSIX_C_SOURCE -I$(INCLUDE_DIR)
#CPPFLAGS= -D_POSIX_C_SOURCE -I$(INCLUDE_DIR) -DDO_TRACE=1


LIBS = -L $(HOME)/$(COURSE)/lib -lcs551 -ldl
CHECK_LIBS = -lcheck -lsubunit -lm -lpthread -lrt

H_FILES = \
  fifos.h \
  common.h \
  sized-data.h \
  steg-client.h \
  steg-server.h

STEG_C_FILES = \
  fifos.c \
  common.c \
  sized-data.c \
  steg-client.c \
  stegc.c \
  stegd.c \
  steg-server.c

C_FILES = \
  $(STEG_C_FILES) \
  test.c

SRC_FILES = \
  $(C_FILES) \
  $(H_FILES) \
  Makefile \
  README

DEPENDS = $(C_FILES:.c=.depends)

CLIENT_OBJS = \
  fifos.o \
  common.o \
  sized-data.o \
  steg-client.o

DAEMON_OBJS = \
  fifos.o \
  common.o \
  sized-data.o \
  stegd.o \
  steg-server.o

#directory in which server runs; used for running client tests
SERVER_DIR = dir

TARGETS = stegc stegd 

.PHONY:		check clean all

all:		$(TARGETS)

stegc:		stegc.o $(CLIENT_OBJS)
		$(CC) $^ $(LIBS) -o $@

stegd:		$(DAEMON_OBJS)
		$(CC) $^ $(LIBS) -o $@

test:		test.o $(CLIENT_OBJS)
		$(CC) $ $^ $(LIBS) $(CHECK_LIBS) -o $@

check:		test
		for m in steg-be.mod steg-le.mod ; \
		do \
		  LD_LIBRARY_PATH=$(LIB_DIR):$(LD_LIBRARY_PATH) \
		    ./test $(SERVER_DIR) $$m; \
		done

valgrind-check:	test
		for m in steg-be.mod steg-le.mod ; \
		do \
	  	  LD_LIBRARY_PATH=$(LIB_DIR):$(LD_LIBRARY_PATH) CK_FORK=no \
		    valgrind --leak-check=full --show-leak-kinds=all \
                      ./test $(SERVER_DIR) $$m; \
		done

clean:
		rm -f *.o  *~ $(DEPENDS) $(TARGETS)

##This Perl filter is used by the .depends rule below
PERL_FILTER_INCLUDE = '$$line .= $$_; \
		       if (!/\\$$/) { \
		         @a = split(/\s+/, $$line); \
		         @b = grep { $$_ !~ "$(COURSE)" } @a; \
		         print "@b\n"; \
		         $$line = ""; \
		       } \
		       else { \
		         $$line =~ s/\\$$//; \
		       }'

#This rule creates a .depends file for the .c prerequisite.  The perl
#program removes dependencies on the course library files.
%.depends:	%.c
		@$(CC) $(CPPFLAGS) -MM $< | \
		perl -ne $(PERL_FILTER_INCLUDE) > $@

-include $(DEPENDS)


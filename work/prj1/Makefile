COURSE = cs551

PROJECT = prj1

INCLUDE_DIR = $(HOME)/$(COURSE)/include

CFLAGS = -g -Wall -fms-extensions -std=c11
CPPFLAGS=	-I$(INCLUDE_DIR)

LIBS = -L $(HOME)/$(COURSE)/lib -lcs551

H_FILES = \
  steg.h

C_FILES = \
  main.c \
  steg.c

SRC_FILES = \
  $(C_FILES) \
  $(H_FILES) \
  Makefile \
  README

OBJS = $(C_FILES:.c=.o)
DEPENDS = $(C_FILES:.c=.depends)

TARGET = steg

.PHONY:		clean all

all:		$(TARGET)

$(TARGET):	$(OBJS)
		$(CC) $(OBJS) $(LIBS) -o $@

clean:
		rm -f *.o  *~ $(DEPENDS) $(TARGET) $(PROJECT).tar.gz

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


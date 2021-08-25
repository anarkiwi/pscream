# Makefile for Primal Scream 1.0 (C) 1994-1995 Josh Bailey
# Updated by Ewen McNeill <ewen@naos.co.nz>, 1999/8/28 for hashbst.
#
# Compilation flags (I used GNU make 3.71, and gcc 2.6.2).
#
# The LDFLAG -N worked well for me, as did the OPTFLAG -funroll-all-loops,
# naturally things may be different for you (I developed this on a
# AMD 486DX/40). 

OPTFLAGS	=-O6 -fomit-frame-pointer -funroll-all-loops
#OPTFLAGS	=-pg -a -g
#OPTFLAGS	=-g
WARNFLAGS	=-pedantic -Wall

CC		=gcc
CFLAGS		=$(WARNFLAGS) $(OPTFLAGS)
LDFLAGS		=-s 
#LDFLAGS		=-a -pg -g
#LDFLAGS		=-g
#-N
LDLIBS		=-lm

# All below here should not need editing.

HDRS	=pscream.h config.h types.h protos.h Makefile
OBJS	=logger.o main.o options.o pscream.o signal.o stack.o state.o
PROGS	=brute list pscream pscreamh

all: $(PROGS)

clean:
	-rm -f $(PROGS) *.o *~
	-rm -f pscream.log pscream.out pscream.txt pscream.zip

backup: clean
	-rm -rf backup
	-mkdir backup
	-cp Makefile Parse *.[ch] backup

dist: backup
	zip -Dj pscream.zip *

# dependancies:

brute.o: $(HDRS)
brute: brute.o

list.o: $(HDRS)
list: list.o

bst.o: $(HDRS)
logger.o: $(HDRS)
main.o: $(HDRS)
options.o: $(HDRS)
pscream.o: $(HDRS)
signal.o: $(HDRS)
stack.o: $(HDRS)
state.o: $(HDRS)
hashbst.o: $(HDRS)

pscream: $(OBJS)  bst.o

pscreamh: $(OBJS)  hashbst.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

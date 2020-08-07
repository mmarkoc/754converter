#NOTE: All indented lines are indented with a tab character. This is required.

CC=gcc
CFLAGS=
DFLAGS=
PROGS= 754converter

all: $(PROGS)

%: %.c
	        $(CC) $(CFLAGS) $(DFLAGS) $< -o $@ -lm

clean:
	        rm -f $(PROGS) *.o ~*


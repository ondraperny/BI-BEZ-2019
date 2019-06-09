CC        = g++
CFLAGS    = -Wall -pedantic
LIBS      = -lcrypto -lssl

TARGETS   = main

all: $(TARGETS)

main: main.cpp
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	\rm -f $(TARGETS) *~ core

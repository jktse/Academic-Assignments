OUTFILE = sorter
OUTDIR = $(HOME)/cmpt433/public/myApps
OUTDEBUG = noworky

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D_POSIX_C_SOURCE=200809L -Werror

help:
	@echo "Type make ___"
	@echo "In the blank either all, sorter, noworky, or clean"
	@echo "It will generate the executable into $~/cmpt433/public/myApps"

all: sorter noworky done

noworky: noworky.o
	$(CC_C) $(CFLAGS) noworky.o -o $(OUTDIR)/$(OUTDEBUG) 

sorter: main.o sorter.o a2d.o display.o UDPListen.o thread.o timing.o config-pin.o
	$(CC_C) $(CFLAGS) main.o sorter.o a2d.o display.o UDPListen.o thread.o timing.o config-pin.o -o $(OUTDIR)/$(OUTFILE) -pthread

main.o: main.c
	$(CC_C) $(CFLAGS) main.c -c

sorter.o: sorter.c sorter.h
	$(CC_C) $(CFLAGS) sorter.c -c

a2d.o: a2d.c a2d.h
	$(CC_C) $(CFLAGS) a2d.c -c

display.o: display.c display.h
	$(CC_C) $(CFLAGS) display.c -c

UDPListen.o: UDPListen.c UDPListen.h
	$(CC_C) $(CFLAGS) UDPListen.c -c

thread.o: thread.c thread.h
	$(CC_C) $(CFLAGS) thread.c -c

timing.o: timing.c timing.h
	$(CC_C) $(CFLAGS) timing.c -c

config-pin.o: config-pin.c config-pin.h
	$(CC_C) $(CFLAGS) config-pin.c -c

noworky.o: noworky.c
	$(CC_C) $(CFLAGS) noworky.c -c

done: 
	@echo "Finished building application."

clean: 
	rm $(OUTDIR)/$(OUTFILE)
	rm $(OUTDIR)/$(OUTDEBUG)
	rm -f *.o


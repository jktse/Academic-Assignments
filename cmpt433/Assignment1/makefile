## From lecture note 4 on how to make a MakeFile. (I want to get some practice on making more complex ones)
OUTFILE = hello
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D_POSIX_C_SOURCE=200809L -Werror

help:
	@echo "Type make ___"
	@echo "In the blank either all, app, or clean"
	@echo "It will generate the executable into $~/cmpt433/public/myApps"

all: app done

app: hello.o led.o gpio.o logic.o timing.o
	$(CC_C) $(CFLAGS) hello.o led.o gpio.o logic.o timing.o -o $(OUTDIR)/$(OUTFILE)

hello.o: hello.c
	$(CC_C) $(CFLAGS) hello.c -c

led.o: led.c led.h
	$(CC_C) $(CFLAGS) led.c -c

gpio.o: gpio.c gpio.h
	$(CC_C) $(CFLAGS) gpio.c -c

logic.o: logic.c logic.h
	$(CC_C) $(CFLAGS) logic.c -c

timing.o: timing.c timing.h
	$(CC_C) $(CFLAGS) timing.c -c


done: 
	@echo "Finished building application."

clean: 
	rm $(OUTDIR)/$(OUTFILE)
	rm -f *.o


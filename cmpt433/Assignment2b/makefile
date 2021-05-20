OUTFILE = app
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D_POSIX_C_SOURCE=200809L -Werror

help:
	@echo "Type make ___"
	@echo "In the blank either all, app, or clean"
	@echo "It will generate the executable into $~/cmpt433/public/myApps"

all: app done

app: noworky.o
	$(CC_C) $(CFLAGS) noworky.o -o $(OUTDIR)/$(OUTFILE) 

noworky.o: noworky.c
	$(CC_C) $(CFLAGS) noworky.c -c

done: 
	@echo "Finished building application."

clean: 
	rm $(OUTDIR)/$(OUTFILE)
	rm -f *.o
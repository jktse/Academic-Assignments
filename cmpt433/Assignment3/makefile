# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
TARGET= beatbox
PROJECT_NAME= beatbox
SOURCES= main.o audioMixer.o timing.o beats.o gpio.o accelerometer.o udpListen.o
DEPLOY_PATH= $(HOME)/cmpt433/public/node/$(PROJECT_NAME)-copy


PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror


# Asound process:
# get alibsound2 lib on target:
# 	# apt-get install libasound2
# Copy target's /usr/lib/arm-linux-gnueabihf/libasound.so.2.0.0 
#      to host  ~/public/asound_lib_BBB/libasound.so
# Copy to just base library:

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB


# -pg for supporting gprof profiling.
#CFLAGS += -pg



all: wav main.o audioMixer.o timing.o beats.o gpio.o accelerometer.o udpListen.o deploy node_install
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET)  $(LFLAGS) -lpthread -lasound -lm

main.o: main.c
	$(CC_C) $(CFLAGS) main.c -c

timing.o: timing.c timing.h
	$(CC_C) $(CFLAGS) timing.c -c

audioMixer.o: audioMixer.c audioMixer.h
	$(CC_C) $(CFLAGS) audioMixer.c -c
	
beats.o: beats.c beats.h
	$(CC_C) $(CFLAGS) beats.c -c

gpio.o: gpio.c gpio.h
	$(CC_C) $(CFLAGS) gpio.c -c

accelerometer.o: accelerometer.c accelerometer.h
	$(CC_C) $(CFLAGS) accelerometer.c -c

udpListen.o: udpListen.c udpListen.h
	$(CC_C) $(CFLAGS) udpListen.c -c

# Copy wave files to the shared folder
wav:
	mkdir -p $(PUBDIR)/wave-files/
	cp wave-files/* $(PUBDIR)/wave-files/ 

deploy:
	@echo 'COPYING THE NODE.JS FILES TO $(DEPLOY_PATH)'
	@echo ''
	mkdir -p $(DEPLOY_PATH)
	chmod a+rwx $(DEPLOY_PATH)
	cp -r * $(DEPLOY_PATH)
	@echo 'Do not edit any files in this folder; they are copied!' > $(DEPLOY_PATH)/DO_NOT_EDIT-FILES_COPIED.txt
	@echo ''
	@echo 'NOTE: On the host, in $(DEPLOY_PATH), it is best to run: npm install'
	@echo '      Or, just run the node_install target in this makefile.'

node_install:
	@echo ''
	@echo ''
	@echo 'INSTALLING REQUIRED NODE PACKAGES'
	@echo '(This may take some time)'
	@echo ''
	cd $(DEPLOY_PATH) && npm install


clean:
	rm -f $(DEPLOY_PATH)
	rm -f $(OUTDIR)/$(TARGET)
	rm -f *.o

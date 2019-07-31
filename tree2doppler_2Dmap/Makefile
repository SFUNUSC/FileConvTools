CFLAGS = -O -Wall -fPIC -ansi
ROOT = $(shell $(ROOTSYS)/bin/root-config --glibs) $(shell $(ROOTSYS)/bin/root-config --cflags)

all: tree2doppler_2Dmap.c tree2doppler_2Dmap.h read_config.c
	g++ tree2doppler_2Dmap.c $(CFLAGS) $(ROOT) -o ../bin/tree2doppler_2Dmap

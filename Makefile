# -*- Makefile -*-
#
# $Id: Makefile 801 2005-09-08 00:39:46Z dsevilla $
#
CFLAGS=-O2 -ansi -pedantic -Wall

#ALL = $(patsubst %.c,%,$(wildcard *.c))

ALL= cliente_tcp servidor_tcp cliente_udp servidor_udp

all: $(ALL)

clean:
	-rm -rf *.o $(ALL)  core *~

ejemplos_p1_b1.tar.gz: $(wildcard *.c)
	tar zcvf $@ cliente_tcp.c cliente_udp.c servidor_tcp.c servidor_udp.c common.h getmyip.c Makefile

.PHONY: clean all

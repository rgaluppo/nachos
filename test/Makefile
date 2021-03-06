# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as
#GCCDIR = ~/gnu/local/decstation-ultrix/bin/
GCCDIR = ~/nachos/mips-dec-ultrix42/mips-
#GCCDIR = ~/nachos/test/mips-dec-ultrix42/mips-
LDFLAGS = -T script -N
ASFLAGS = -mips1
CPPFLAGS = $(INCDIR)


# if you aren't cross-compiling:
#GCCDIR =
#LDFLAGS = -N -T 0
#ASFLAGS =
#CPPFLAGS = -P $(INCDIR)


CC = $(GCCDIR)gcc
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = /lib/cpp
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR) -mips1

all: halt shell matmult sort testConsole readfile \
     writefile exec join cat cp clear

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

testConsole.o: testConsole.c
	$(CC) $(CFLAGS) -c testConsole.c
testConsole: testConsole.o start.o
	$(LD) $(LDFLAGS) start.o testConsole.o -o testConsole.coff
	../bin/coff2noff testConsole.coff testConsole

readfile.o: readfile.c
	$(CC) $(CFLAGS) -c readfile.c
readfile: readfile.o start.o
	$(LD) $(LDFLAGS) start.o readfile.o -o readfile.coff
	../bin/coff2noff readfile.coff readfile

writefile.o: writefile.c
	$(CC) $(CFLAGS) -c writefile.c
writefile: writefile.o start.o
	$(LD) $(LDFLAGS) start.o writefile.o -o writefile.coff
	../bin/coff2noff writefile.coff writefile

exec.o: exec.c
	$(CC) $(CFLAGS) -c exec.c
exec: exec.o start.o
	$(LD) $(LDFLAGS) start.o exec.o -o exec.coff
	../bin/coff2noff exec.coff exec

join.o: join.c
	$(CC) $(CFLAGS) -c join.c
join: join.o start.o
	$(LD) $(LDFLAGS) start.o join.o -o join.coff
	../bin/coff2noff join.coff join

cat.o: cat.c
	$(CC) $(CFLAGS) -c cat.c
cat: cat.o start.o
	$(LD) $(LDFLAGS) start.o cat.o -o cat.coff
	../bin/coff2noff cat.coff cat

cp.o: cp.c
	$(CC) $(CFLAGS) -c cp.c
cp: cp.o start.o
	$(LD) $(LDFLAGS) start.o cp.o -o cp.coff
	../bin/coff2noff cp.coff cp

clear.o: clear.c
	$(CC) $(CFLAGS) -c clear.c
clear: clear.o start.o
	$(LD) $(LDFLAGS) start.o clear.o -o clear.coff
	../bin/coff2noff clear.coff clear

# Estas reglas sirven para compilar programas simples,
# que consistan en un �nico fuente.
# Las reglas anteriores para construir los ejecutables
# halt, shell, sort y filetest se podr�an suprimir

%: %.c 
	make start.o 
	$(CC) $(CFLAGS) -c $<
	$(LD) $(LDFLAGS) start.o $*.o -o $*.coff
	../bin/ $*.coff $@
	


#
#   SEGA SATURN Graphic library make file for GNU
#
# modified by to be used with c++ by The Rockin'-B, 05/02/09

# INCLUDE = -I"C:\Programme\KPIT Cummins\GNUSHv0403-COFF\sh-coff\include\c++\3.4-GNUSH_v0403" -I"C:\Programme\KPIT Cummins\GNUSHv0403-COFF\sh-coff\include\c++\3.4-GNUSH_v0403\backward" -I"C:\Programme\KPIT Cummins\GNUSHv0403-COFF\sh-coff\sh-coff\include"
RBINC = ./RB_LIB/INC
# RBLIB = "D:\dev\SATURN\SGL\PROJECTS\RB\BUILD\Saturn"

# macro
CC = sh-elf-gcc
CPP = sh-elf-g++
LD = sh-elf-ld
RM = rm
CONV = sh-elf-objcopy

# directory
SGLDIR = ../..
SGLIDR = $(SGLDIR)/inc
SGLLDR = $(SGLDIR)/lib

CMNDIR = ../common
OBJECTS = ./objects

# option -g <- debug info?
CCFLAGS = -Os -m2 -c -I$(RBINC) -I$(SGLDIR)/inc
CPPFLAGS = $(CCFLAGS) -Wno-deprecated -fno-rtti #-fno-exceptions


# -m2 must be specified in LDFLAGS so the linker will search the SH2 lib dirs
# Specify path of libsgl.a by using "-L" option

LDFLAGS = -m2 -L$(SGLLDR) -L$(RBLIB) -Xlinker -T$(LDFILE) -Xlinker -Map \
          -Xlinker $(MPFILE) -Xlinker -S -Xlinker -e -Xlinker ___Start -nostartfiles
DFLAGS =
# source_program
include $(OBJECTS)

TARGET_PREFIX = SoundPlayer

TARGET   = $(TARGET_PREFIX).coff
TARGETH  = $(TARGET_PREFIX)H.bin        # main binary for high work RAM
TARGETL  = #$(TARGET_PREFIX)L.bin       # to be placed in low work RAM
#LDFILE	 = $(CMNDIR)/$(TARGET:.coff=.lnk)
LDFILE	 = $(TARGET_PREFIX).lnk
MPFILE   = $(TARGET_PREFIX).map
MAKEFILE = Makefile

all: $(TARGET) $(TARGETH) $(TARGETL)

# Use gcc to link so it will automagically find correct libs directory

$(TARGET) : $(SYSOBJS) $(OBJS) $(MAKEFILE) $(OBJECTS) $(LDFILE)
	$(CC) $(LDFLAGS) $(SYSOBJS) $(OBJS) $(LIBS) -o $@

#$(TARGETL) : $(SYSOBJS) $(OBJS) $(MAKEFILE) $(LDFILE)
#	$(CONV) -O binary -j RARE $(TARGET) $(TARGETL)

$(TARGETH) : $(SYSOBJS) $(OBJS) $(MAKEFILE) $(LDFILE)
	$(CONV) -O binary $(TARGET) $(TARGETH)
#	$(CONV) -O binary -j SLSTART -j .text -j SLPROG -j .tors -j .data -j .rodata -j .bss $(TARGET) $(TARGETH)

#$(LDFILE) : $(MAKEFILE)
#	@echo Making $(LDFILE)
#	@echo SECTIONS {		> $@
#	@echo 	SLSTART 0x06004000 : {	>> $@
#	@echo 		___Start = .;	>> $@
#	@echo 		*(SLSTART)	>> $@
#	@echo 	}			>> $@
#	@echo 	.text ALIGN(0x20) :			>> $@
#	@echo 	{			>> $@
#	@echo 		* (.text)			>> $@
#	@echo 		*(.strings)			>> $@
#	@echo 		__etext = .;			>> $@
#	@echo 	}			>> $@
#	@echo 	SLPROG ALIGN(0x20): {	>> $@
#	@echo 		__slprog_start = .;	>> $@
#	@echo 		*(SLPROG)	>> $@
#	@echo 		__slprog_end = .;	>> $@
#	@echo 	}			>> $@
#	@echo 	.tors  ALIGN(0x10) :			>> $@
#	@echo 	{			>> $@
#	@echo 		___ctors = . ;			>> $@
#	@echo 		*(.ctors)			>> $@
#	@echo 		___ctors_end = . ;			>> $@
#	@echo 		___dtors = . ;			>> $@
#	@echo 		*(.dtors)			>> $@
#	@echo 		___dtors_end = . ;			>> $@
#	@echo 	}			>> $@
#	@echo 	.data ALIGN(0x10):			>> $@
#	@echo 	{			>> $@
#	@echo 		* (.data)			>> $@
#	@echo 		_edata = . ;			>> $@
#	@echo 	}			>> $@
#	@echo 	.bss ALIGN(0x10) (NOLOAD):			>> $@
#	@echo 	{			>> $@
#	@echo 		__bstart = . ;			>> $@
#	@echo 		*(.bss)			>> $@
#	@echo 		* ( COMMON )			>> $@
#	@echo 		__bend = . ;			>> $@
#	@echo 	_end = .;			>> $@
#	@echo 	}			>> $@
#	@echo }				>> $@

# suffix
.SUFFIXES: .asm

.c.o:
	$(CC) $< $(DFLAGS) $(RBINC) $(CCFLAGS) -o $@

.cpp.o:
	$(CPP) $< $(DFLAGS) $(RBINC) $(INCLUDE) $(CPPFLAGS) -o $@

 

clean:
	$(RM) $(OBJS) $(TARGET) $(TARGETL) $(TARGETH)


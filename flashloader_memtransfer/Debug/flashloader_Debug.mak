SHELL = cmd.exe

#
# ZDS II Make File - flashloader project, Debug configuration
#
# Generated by: ZDS II - eZ80Acclaim! 5.3.5 (Build 23020901)
#   IDE component: d:5.3.0:23020901
#   Install Path: C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5\
#

RM = del

# ZDS base directory
ZDSDIR = C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5
ZDSDIR_ESCSPACE = C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5

# ZDS bin directory
BIN = $(ZDSDIR)\bin

# ZDS include base directory
INCLUDE = C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5\include
INCLUDE_ESCSPACE = C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5\include

# ZTP base directory
ZTPDIR = C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5\ZTP2.5.1
ZTPDIR_ESCSPACE = C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5\ZTP2.5.1

# project directory
PRJDIR = C:\source\agon-vdpflash\flashloader_memtransfer
PRJDIR_ESCSPACE = C:\source\agon-vdpflash\flashloader_memtransfer

# intermediate files directory
WORKDIR = C:\source\agon-vdpflash\flashloader_memtransfer\Debug
WORKDIR_ESCSPACE = C:\source\agon-vdpflash\flashloader_memtransfer\Debug

# output files directory
OUTDIR = C:\source\agon-vdpflash\flashloader_memtransfer\Debug\
OUTDIR_ESCSPACE = C:\source\agon-vdpflash\flashloader_memtransfer\Debug\

# tools
CC = @"$(BIN)\eZ80cc"
AS = @"$(BIN)\eZ80asm"
LD = @"$(BIN)\eZ80link"
AR = @"$(BIN)\eZ80lib"
WEBTOC = @"$(BIN)\mkwebpage"

CFLAGS =  \
-define:_DEBUG -define:_EZ80F92 -define:_EZ80ACCLAIM! -genprintf  \
-keepasm -keeplst -NOlist -NOlistinc -NOmodsect -optsize -promote  \
-NOreduceopt  \
-stdinc:"\"..;C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5\include\std;C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5\include\zilog\""  \
-usrinc:"\"..;\"" -NOmultithread -NOpadbranch -debug -cpu:eZ80F92  \
-asmsw:"   \
	-define:_EZ80ACCLAIM!=1  \
	-include:\"..;C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5\include\std;C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5\include\zilog\"  \
	-list -NOlistmac -pagelen:0 -pagewidth:80 -quiet -sdiopt -warn  \
	-debug -NOigcase -cpu:eZ80F92"

ASFLAGS =  \
-define:_EZ80ACCLAIM!=1  \
-include:"\"..;C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5\include\std;C:\ZiLOG\ZDSII_eZ80Acclaim!_5.3.5\include\zilog\""  \
-list -NOlistmac -name -pagelen:0 -pagewidth:80 -quiet -sdiopt  \
-warn -debug -NOigcase -cpu:eZ80F92

LDFLAGS = @..\flashloader.linkcmd
build: flashloader relist

buildall: clean flashloader relist

relink: deltarget flashloader

deltarget: 
	@if exist "$(WORKDIR)\flashloader.lod"  \
            $(RM) "$(WORKDIR)\flashloader.lod"
	@if exist "$(WORKDIR)\flashloader.hex"  \
            $(RM) "$(WORKDIR)\flashloader.hex"
	@if exist "$(WORKDIR)\flashloader.map"  \
            $(RM) "$(WORKDIR)\flashloader.map"

clean: 
	@if exist "$(WORKDIR)\flashloader.lod"  \
            $(RM) "$(WORKDIR)\flashloader.lod"
	@if exist "$(WORKDIR)\flashloader.hex"  \
            $(RM) "$(WORKDIR)\flashloader.hex"
	@if exist "$(WORKDIR)\flashloader.map"  \
            $(RM) "$(WORKDIR)\flashloader.map"
	@if exist "$(WORKDIR)\init.obj"  \
            $(RM) "$(WORKDIR)\init.obj"
	@if exist "$(WORKDIR)\init.lis"  \
            $(RM) "$(WORKDIR)\init.lis"
	@if exist "$(WORKDIR)\init.lst"  \
            $(RM) "$(WORKDIR)\init.lst"
	@if exist "$(WORKDIR)\main.obj"  \
            $(RM) "$(WORKDIR)\main.obj"
	@if exist "$(WORKDIR)\main.lis"  \
            $(RM) "$(WORKDIR)\main.lis"
	@if exist "$(WORKDIR)\main.lst"  \
            $(RM) "$(WORKDIR)\main.lst"
	@if exist "$(WORKDIR)\main.src"  \
            $(RM) "$(WORKDIR)\main.src"
	@if exist "$(WORKDIR)\diskio.obj"  \
            $(RM) "$(WORKDIR)\diskio.obj"
	@if exist "$(WORKDIR)\diskio.lis"  \
            $(RM) "$(WORKDIR)\diskio.lis"
	@if exist "$(WORKDIR)\diskio.lst"  \
            $(RM) "$(WORKDIR)\diskio.lst"
	@if exist "$(WORKDIR)\diskio.src"  \
            $(RM) "$(WORKDIR)\diskio.src"
	@if exist "$(WORKDIR)\ff.obj"  \
            $(RM) "$(WORKDIR)\ff.obj"
	@if exist "$(WORKDIR)\ff.lis"  \
            $(RM) "$(WORKDIR)\ff.lis"
	@if exist "$(WORKDIR)\ff.lst"  \
            $(RM) "$(WORKDIR)\ff.lst"
	@if exist "$(WORKDIR)\ff.src"  \
            $(RM) "$(WORKDIR)\ff.src"
	@if exist "$(WORKDIR)\ffunicode.obj"  \
            $(RM) "$(WORKDIR)\ffunicode.obj"
	@if exist "$(WORKDIR)\ffunicode.lis"  \
            $(RM) "$(WORKDIR)\ffunicode.lis"
	@if exist "$(WORKDIR)\ffunicode.lst"  \
            $(RM) "$(WORKDIR)\ffunicode.lst"
	@if exist "$(WORKDIR)\ffunicode.src"  \
            $(RM) "$(WORKDIR)\ffunicode.src"
	@if exist "$(WORKDIR)\sd.obj"  \
            $(RM) "$(WORKDIR)\sd.obj"
	@if exist "$(WORKDIR)\sd.lis"  \
            $(RM) "$(WORKDIR)\sd.lis"
	@if exist "$(WORKDIR)\sd.lst"  \
            $(RM) "$(WORKDIR)\sd.lst"
	@if exist "$(WORKDIR)\sd.src"  \
            $(RM) "$(WORKDIR)\sd.src"
	@if exist "$(WORKDIR)\spi.obj"  \
            $(RM) "$(WORKDIR)\spi.obj"
	@if exist "$(WORKDIR)\spi.lis"  \
            $(RM) "$(WORKDIR)\spi.lis"
	@if exist "$(WORKDIR)\spi.lst"  \
            $(RM) "$(WORKDIR)\spi.lst"
	@if exist "$(WORKDIR)\spi.src"  \
            $(RM) "$(WORKDIR)\spi.src"
	@if exist "$(WORKDIR)\timer.obj"  \
            $(RM) "$(WORKDIR)\timer.obj"
	@if exist "$(WORKDIR)\timer.lis"  \
            $(RM) "$(WORKDIR)\timer.lis"
	@if exist "$(WORKDIR)\timer.lst"  \
            $(RM) "$(WORKDIR)\timer.lst"
	@if exist "$(WORKDIR)\timer.src"  \
            $(RM) "$(WORKDIR)\timer.src"

relist: 
	$(AS) $(ASFLAGS) -relist:"C:\source\agon-vdpflash\flashloader_memtransfer\Debug\flashloader.map" \
            C:\source\agon-vdpflash\flashloader_memtransfer\init.asm
	$(AS) $(ASFLAGS) -relist:"C:\source\agon-vdpflash\flashloader_memtransfer\Debug\flashloader.map" \
            C:\source\agon-vdpflash\flashloader_memtransfer\Debug\main.src
	$(AS) $(ASFLAGS) -relist:"C:\source\agon-vdpflash\flashloader_memtransfer\Debug\flashloader.map" \
            C:\source\agon-vdpflash\flashloader_memtransfer\Debug\diskio.src
	$(AS) $(ASFLAGS) -relist:"C:\source\agon-vdpflash\flashloader_memtransfer\Debug\flashloader.map" \
            C:\source\agon-vdpflash\flashloader_memtransfer\Debug\ff.src
	$(AS) $(ASFLAGS) -relist:"C:\source\agon-vdpflash\flashloader_memtransfer\Debug\flashloader.map" \
            C:\source\agon-vdpflash\flashloader_memtransfer\Debug\ffunicode.src
	$(AS) $(ASFLAGS) -relist:"C:\source\agon-vdpflash\flashloader_memtransfer\Debug\flashloader.map" \
            C:\source\agon-vdpflash\flashloader_memtransfer\Debug\sd.src
	$(AS) $(ASFLAGS) -relist:"C:\source\agon-vdpflash\flashloader_memtransfer\Debug\flashloader.map" \
            C:\source\agon-vdpflash\flashloader_memtransfer\Debug\spi.src
	$(AS) $(ASFLAGS) -relist:"C:\source\agon-vdpflash\flashloader_memtransfer\Debug\flashloader.map" \
            C:\source\agon-vdpflash\flashloader_memtransfer\Debug\timer.src

# pre-4.11.0 compatibility
rebuildall: buildall 

LIBS = 

OBJS =  \
            $(WORKDIR_ESCSPACE)\init.obj  \
            $(WORKDIR_ESCSPACE)\main.obj  \
            $(WORKDIR_ESCSPACE)\diskio.obj  \
            $(WORKDIR_ESCSPACE)\ff.obj  \
            $(WORKDIR_ESCSPACE)\ffunicode.obj  \
            $(WORKDIR_ESCSPACE)\sd.obj  \
            $(WORKDIR_ESCSPACE)\spi.obj  \
            $(WORKDIR_ESCSPACE)\timer.obj

flashloader: $(OBJS)
	 $(LD) $(LDFLAGS)

$(WORKDIR_ESCSPACE)\init.obj :  \
            $(PRJDIR_ESCSPACE)\init.asm  \
            $(INCLUDE_ESCSPACE)\zilog\ez80F92.inc  \
            $(PRJDIR_ESCSPACE)\equs.inc
	 $(AS) $(ASFLAGS) "$(PRJDIR)\init.asm"

$(WORKDIR_ESCSPACE)\main.obj :  \
            $(PRJDIR_ESCSPACE)\main.c  \
            $(INCLUDE_ESCSPACE)\std\FLOAT.H  \
            $(INCLUDE_ESCSPACE)\zilog\cio.h  \
            $(INCLUDE_ESCSPACE)\zilog\defines.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80190.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F91.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F92.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F93.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80L92.h  \
            $(INCLUDE_ESCSPACE)\zilog\ez80.h  \
            $(INCLUDE_ESCSPACE)\zilog\gpio.h  \
            $(INCLUDE_ESCSPACE)\zilog\uart.h  \
            $(INCLUDE_ESCSPACE)\zilog\uartdefs.h  \
            $(PRJDIR_ESCSPACE)\spi.h  \
            $(PRJDIR_ESCSPACE)\src_fatfs\ff.h  \
            $(PRJDIR_ESCSPACE)\src_fatfs\ffconf.h
	 $(CC) $(CFLAGS) "$(PRJDIR)\main.c"

$(WORKDIR_ESCSPACE)\diskio.obj :  \
            $(PRJDIR_ESCSPACE)\src_fatfs\diskio.c  \
            $(INCLUDE_ESCSPACE)\std\FLOAT.H  \
            $(PRJDIR_ESCSPACE)\src_fatfs\diskio.h  \
            $(PRJDIR_ESCSPACE)\src_fatfs\ff.h  \
            $(PRJDIR_ESCSPACE)\src_fatfs\ffconf.h
	 $(CC) $(CFLAGS) "$(PRJDIR)\src_fatfs\diskio.c"

$(WORKDIR_ESCSPACE)\ff.obj :  \
            $(PRJDIR_ESCSPACE)\src_fatfs\ff.c  \
            $(INCLUDE_ESCSPACE)\std\FLOAT.H  \
            $(INCLUDE_ESCSPACE)\std\Math.h  \
            $(INCLUDE_ESCSPACE)\std\Stdarg.h  \
            $(INCLUDE_ESCSPACE)\std\String.h  \
            $(PRJDIR_ESCSPACE)\src_fatfs\diskio.h  \
            $(PRJDIR_ESCSPACE)\src_fatfs\ff.h  \
            $(PRJDIR_ESCSPACE)\src_fatfs\ffconf.h
	 $(CC) $(CFLAGS) "$(PRJDIR)\src_fatfs\ff.c"

$(WORKDIR_ESCSPACE)\ffunicode.obj :  \
            $(PRJDIR_ESCSPACE)\src_fatfs\ffunicode.c  \
            $(INCLUDE_ESCSPACE)\std\FLOAT.H  \
            $(PRJDIR_ESCSPACE)\src_fatfs\ff.h  \
            $(PRJDIR_ESCSPACE)\src_fatfs\ffconf.h
	 $(CC) $(CFLAGS) "$(PRJDIR)\src_fatfs\ffunicode.c"

$(WORKDIR_ESCSPACE)\sd.obj :  \
            $(PRJDIR_ESCSPACE)\sd.c  \
            $(INCLUDE_ESCSPACE)\std\Format.h  \
            $(INCLUDE_ESCSPACE)\std\Stdarg.h  \
            $(INCLUDE_ESCSPACE)\std\Stdio.h  \
            $(INCLUDE_ESCSPACE)\std\String.h  \
            $(INCLUDE_ESCSPACE)\zilog\cio.h  \
            $(INCLUDE_ESCSPACE)\zilog\defines.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80190.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F91.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F92.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F93.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80L92.h  \
            $(INCLUDE_ESCSPACE)\zilog\ez80.h  \
            $(INCLUDE_ESCSPACE)\zilog\gpio.h  \
            $(INCLUDE_ESCSPACE)\zilog\uart.h  \
            $(INCLUDE_ESCSPACE)\zilog\uartdefs.h  \
            $(PRJDIR_ESCSPACE)\sd.h  \
            $(PRJDIR_ESCSPACE)\spi.h  \
            $(PRJDIR_ESCSPACE)\timer.h
	 $(CC) $(CFLAGS) "$(PRJDIR)\sd.c"

$(WORKDIR_ESCSPACE)\spi.obj :  \
            $(PRJDIR_ESCSPACE)\spi.c  \
            $(INCLUDE_ESCSPACE)\zilog\cio.h  \
            $(INCLUDE_ESCSPACE)\zilog\defines.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80190.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F91.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F92.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F93.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80L92.h  \
            $(INCLUDE_ESCSPACE)\zilog\ez80.h  \
            $(INCLUDE_ESCSPACE)\zilog\gpio.h  \
            $(INCLUDE_ESCSPACE)\zilog\uart.h  \
            $(INCLUDE_ESCSPACE)\zilog\uartdefs.h  \
            $(PRJDIR_ESCSPACE)\spi.h  \
            $(PRJDIR_ESCSPACE)\timer.h
	 $(CC) $(CFLAGS) "$(PRJDIR)\spi.c"

$(WORKDIR_ESCSPACE)\timer.obj :  \
            $(PRJDIR_ESCSPACE)\timer.c  \
            $(INCLUDE_ESCSPACE)\zilog\cio.h  \
            $(INCLUDE_ESCSPACE)\zilog\defines.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80190.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F91.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F92.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80F93.h  \
            $(INCLUDE_ESCSPACE)\zilog\eZ80L92.h  \
            $(INCLUDE_ESCSPACE)\zilog\ez80.h  \
            $(INCLUDE_ESCSPACE)\zilog\gpio.h  \
            $(INCLUDE_ESCSPACE)\zilog\uart.h  \
            $(INCLUDE_ESCSPACE)\zilog\uartdefs.h  \
            $(PRJDIR_ESCSPACE)\timer.h
	 $(CC) $(CFLAGS) "$(PRJDIR)\timer.c"

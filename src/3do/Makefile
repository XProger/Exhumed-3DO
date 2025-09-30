SDK        = C:/Projects/3do-devkit
FILESYSTEM = CD
EXENAME	   = $(FILESYSTEM)/LaunchMe
ISONAME    = Exhumed-3DO.iso
STACKSIZE  = 4096
BANNER	   = banner.bmp

CC	   = armcc
CXX	   = armcpp
AS 	   = armasm
LD	   = armlink
RM	   = rm
MODBIN     = modbin
MAKEBANNER = MakeBanner
3DOISO     = 3doiso
3DOENCRYPT = 3DOEncrypt

OPT      = -O2
CFLAGS   = $(OPT) -bi -za1 -zas1 -wn -ff -fa -d AP_3DO=1 -d GAPI_CEL=1 -d NDEBUG -cpu ARM6
CXXFLAGS = $(CFLAGS)
ASFLAGS	 = -BI -i $(SDK)/include/3do
INCPATH	 = -I $(SDK)/include/3do -I $(SDK)/include/ttl -I ..
LIBPATH	 = $(SDK)/lib/3do
LDFLAGS	 = -aif -reloc -ro-base 0 -libpath $(LIBPATH) -nodebug -remove -info Sizes -xref -map
STARTUP	 = $(LIBPATH)/cstartup.o

LIBS = \
	$(LIBPATH)/clib.lib			\
	$(LIBPATH)/swi.lib			\
	$(LIBPATH)/lib3do.lib		\
	$(LIBPATH)/audio.lib		\
	$(LIBPATH)/music.lib		\
	$(LIBPATH)/filesystem.lib	\
	$(LIBPATH)/graphics.lib		\
	$(LIBPATH)/input.lib		\

SRC_S   = $(wildcard asm/*.asm)
SRC_C   = $(wildcard ../*.c)
SRC_C  += $(wildcard *.c)

OBJ += $(SRC_S:%.asm=%.asm.o)
OBJ += $(SRC_C:%.c=%.c.o)

all: clean launchme modbin banner iso run

usb: clean launchme modbin banner iso copy_usb run

launchme: builddir $(OBJ)
	$(LD) -dupok -o $(EXENAME) $(LDFLAGS) $(STARTUP) $(LIBS) $(addprefix _build/, $(notdir $(OBJ)))

modbin:
	$(MODBIN) --stack=$(STACKSIZE) $(EXENAME) $(EXENAME)

banner:
	$(MAKEBANNER) $(BANNER) $(FILESYSTEM)/BannerScreen

iso:
	$(3DOISO) -in $(FILESYSTEM) -out $(ISONAME)
	$(3DOENCRYPT) genromtags $(ISONAME)

run:
	/c/RetroArch/retroarch.exe -L C:\RetroArch\cores\opera_libretro.dll C:\Projects\Exhumed-3DO\src\3do\$(ISONAME)

builddir:
	mkdir -p _build/

%.asm.o: %.asm
	$(AS) $(INCPATH) $(ASFLAGS) $< -o _build/$(notdir $@)

%.c.o: %.c
	$(CC) $(INCPATH) $(CFLAGS) -c $< -o _build/$(notdir $@)
    
clean:
	$(RM) -vfr _build $(EXENAME) $(EXENAME).sym $(ISONAME)

.PHONY: clean modbin banner iso

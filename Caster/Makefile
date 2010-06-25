all: Caster

clean:
	rm *.o Caster

#CROSS:=~/openwrt/trunk/staging_dir/toolchain-mips_gcc-4.3.3+cs_uClibc-0.9.30.1
#CROSSBIN:=$(CROSS)/usr/bin/mips-openwrt-linux-
CROSS=/
CROSSBIN=/usr/bin/

AR:=$(CROSSBIN)ar
LD:=$(CROSSBIN)ld
GCC:=$(CROSSBIN)gcc
CC:=$(GCC)
CXX:=$(CROSSBIN)g++

#CPPFLAGS:= -Os -I Dispatcher -I Ntrip -I UsefulStuff -I --sysroot=$(CROSS)
CPPFLAGS:= -g -I Dispatcher -I Ntrip -I UsefulStuff -I --sysroot=$(CROSS)
CFLAGS:= $(CPPFLAGS) 
#LDFLAGS:= -static -lgcc -s
LDFLAGS:= -static -lgcc -g

SOURCES:=$(wildcard */*.cpp)
HEADERS:=$(wildcard */*.h)
OBJECTS:=$(SOURCES:.cpp=.o)

Caster: $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) $(LDFLAGS) ~/Kinematic/bin/mips-openwrt/Kinematic.a -o Caster
	

$(OBJECTS) : $(HEADERS)

all: Caster

clean:
	rm *.o Caster

CROSS:=~/toolchain-armeb
CROSSBIN:=$(CROSS)/usr/armeb-openwrt-linux-uclibcgnueabi/bin
#CROSS=
#CROSSBIN=/usr/bin

AR:=$(CROSSBIN)/ar
LD:=$(CROSSBIN)/ld
GCC:=$(CROSSBIN)/gcc
CC:=$(GCC)
CXX:=$(CROSSBIN)/g++

CPPFLAGS:= -ggdb -I $(CROSS)/usr/include -I $(CROSS)/include
CFLAGS:= $(CPPFLAGS)
LDFLAGS:= -static -L $(CROSS)/usr/lib -L $(CROSS)/lib -lgcc

SOURCES:=$(wildcard *.cpp)
HEADERS:=$(wildcard *.h)
OBJECTS:=$(SOURCES:.cpp=.o)

Caster: $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o Caster
	

$(OBJECTS) : $(HEADERS)

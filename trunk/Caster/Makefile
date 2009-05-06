all: Caster

CROSS:=~/Desktop/kamikaze_7.09/staging_dir_armeb
CROSSBIN:=$(CROSS)/armeb-linux/bin

AR:=$(CROSSBIN)/ar
LD:=$(CROSSBIN)/ld
GCC:=$(CROSSBIN)/gcc
CC:=$(GCC)
CXX:=$(CROSSBIN)/g++

CPPFLAGS:=-I $(CROSS)/usr/include -I $(CROSS)/include
CFLAGS:= -ggdb3 $(CPPFLAGS)
LDFLAGS:= -static -L $(CROSS)/usr/lib -L $(CROSS)/lib -lgcc

SOURCES:=$(wildcard *.cpp)
HEADERS:=$(wildcard *.h)
OBJECTS:=$(SOURCES:.cpp=.o)

Caster: $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o Caster
	

$(OBJECTS) : $(HEADERS)

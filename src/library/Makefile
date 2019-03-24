####################################################################################################
#                                     Make file LargeInt 	                                   #
####################################################################################################
INC:=-I../../inc/library
CFLAGS:=-c -Wall $(INC) --std=c++1y -fPIC
SOURCES:= \
	tools.cpp \
	protocol.cpp \

OBJDIR:=../../obj
BINDIR:=../../bin
LIBDIR:=../../lib
LDFLAGS:= -shared -fPIC
OBJECTS:=$(SOURCES:.cpp=.o)
CC:=g++
MAJOR:= 0
MINOR:= 1
NAME:= simpleftp
VERSION := $(MAJOR).$(MINOR)

all: clean MAKEDIR $(SOURCES) lib install

lib: lib$(NAME).so.$(VERSION)

lib$(NAME).so.$(VERSION): $(OBJECTS)
	$(CC) $(LDFLAGS) $(addprefix $(OBJDIR)/, $(OBJECTS)) -o $(LIBDIR)/$@ 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $(OBJDIR)/$@

clean:
	rm -rf $(LIBDIR)/lib$(NAME).so* $(OBJDIR)/$(OBJECTS)

MAKEDIR:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)

install:
	ln -sf lib$(NAME).so.$(VERSION) $(LIBDIR)/lib$(NAME).so.$(MAJOR)
	ln -sf lib$(NAME).so.$(VERSION) $(LIBDIR)/lib$(NAME).so

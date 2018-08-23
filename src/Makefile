####################################################################################################
#                                     Make file LargeInt 	                                   #
####################################################################################################
CFLAGS:=-c -Wall --std=c++11 -DD_DEBUG
SOURCES:= cpy.cpp socket.cpp simpleftp.cpp main.cpp
OBJDIR:=../obj
BINDIR:=../bin
LIBDIR:=../lib
LDFLAGS:=-L$(LIBDIR) 
OBJECTS:=$(SOURCES:.cpp=.o)
CC:=g++
EXECUTABLE:=server.out
LIBRARIES:= -pthread

all: clean MAKEDIR $(SOURCES) $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(addprefix $(OBJDIR)/, $(OBJECTS)) -o $(BINDIR)/$@ $(LIBRARIES)
	cp $(BINDIR)/$@ ../../server
	cp $(BINDIR)/$@ ../../client

.cpp.o:
	$(CC) $(CFLAGS) $< -o $(OBJDIR)/$@

clean:
	rm -rf $(BINDIR)/$(EXECUTABLE) $(addprefix $(OBJDIR)/, $(OBJECTS)) $(BINDIR) $(OBJDIR)

MAKEDIR:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)
	mkdir -p ../../server
	mkdir -p ../../client



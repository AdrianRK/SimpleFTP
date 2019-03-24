####################################################################################################
#                                     Make file LargeInt 	                                   #
####################################################################################################
INC:=-I../inc/
CFLAGS:=-c -Wall $(INC) --std=c++11 -DD_DEBUG
SOURCES:= cpy.cpp \
		config.cpp \
		socket.cpp \
		simpleftp.cpp \
		ui.cpp \
		protocol.cpp \
		main.cpp

OBJDIR:=../obj
BINDIR:=../bin
LIBDIR:=../lib
LDFLAGS:=-L$(LIBDIR) 
OBJECTS:=$(SOURCES:.cpp=.o)
CC:=g++
EXECUTABLE:=simpleftp
LIBRARIES:= -pthread

all: cln MAKEDIR $(SOURCES) $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(addprefix $(OBJDIR)/, $(OBJECTS)) -o $(BINDIR)/$@ $(LIBRARIES)
	cp $(BINDIR)/$@ ../../server
	cp $(BINDIR)/$@ ../../client

.cpp.o:
	$(CC) $(CFLAGS) $< -o $(OBJDIR)/$@

cln:
	rm -rf $(BINDIR)/$(EXECUTABLE)
	
clean:
	rm -rf $(BINDIR)/$(EXECUTABLE) $(addprefix $(OBJDIR)/, $(OBJECTS)) $(BINDIR) $(OBJDIR)

MAKEDIR:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)
	mkdir -p ../../server
	mkdir -p ../../client



#Project Makefile
#Gonga, Antonio Oliveira
#Created on Sep 15, 2008
#Last Modified Oct 8th, 2008
#
OBJF1=histogram.o
OBJF2=core_program_module.o
OBJF3=functions.o
OBJF4=snmp_functions.o

OBJFILES=histogram.o core_program_module.o  functions.o snmp_functions.o

#Messages
CMPL =Compiling Source Files .......
LKNG =Linking Object Files .......
CLRM =Clearing Object Files .......
MKCLN=Clearing Object and executable files .......


HFILES  =headers.h prototypes.h

#
CFILES1  =histogram.c
CFILES2  =core_program_module.c
CFILES3  =functions.c
CFILES4  =snmp_functions.c

CFILES   =$(CFILES1) $(CFILES2) $(CFILES3) $(CFILES4)

#
CC       =gcc
CFLAGS   =-I. `net-snmp-config --cflags`
BUILDLIBS=`net-snmp-config --libs`

TARGETS  =histogram 

# shared library flags (assumes gcc)
DLFLAGS  =-fPIC -shared


#
#
all: $(TARGETS)

#
#
$(TARGETS): $(CFILES) $(HFILES)
	echo $(LKNG)
	$(CC)  $(CFILES)  -o $(TARGETS) $(BUILDLIBS) $(CFLAGS)  
	echo $(CLRM)
	rm -rf $(OBJF1) $(OBJF2) $(OBJF3) $(OBJF4)


clean: 
	echo $(MKCLN)
	rm -rf *.o $(TARGETS) *.txt

# Source du Makefile : https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html#using
# 'make'        build executable files 'client' and 'server'
# 'make clean'  removes all .o and executable files

# define the C compiler to use
CC = gcc

# define any compile-time flags
CFLAGS = -Wall -g

# define any directories containing header files other than /usr/include
INCLUDES =

# define any libraries to link into executable:
LIBS = -lrt -lm

# define the C source files
CLIENT = client.c fct_mini.c game.c
SERVER = server.c game.c

# define the C object files
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
COBJS = $(CLIENT:.c=.o)
SOBJS = $(SERVER:.c=.o)

# define the executable file
CLIENTEXEC = client.out
SERVEREXEC = server.out

#
# The following part of the makefile is generic; it can be used to
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all:	$(CLIENTEXEC) $(SERVEREXEC)
	@echo  Success executable files has been named \'client.out\' and \'server.out\'\'

$(SERVEREXEC): $(SOBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(SERVEREXEC) $(SOBJS) $(LFLAGS) $(LIBS)

$(CLIENTEXEC): $(COBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(CLIENTEXEC) $(COBJS) $(LFLAGS) $(LIBS)



# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file)
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) *.o *~ $(CLIENTEXEC) $(SERVEREXEC)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

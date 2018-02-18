MYNUM = 02
OBJO = m$(MYNUM).o

#incase I want to change compiler suddenly, or gcc changes names
CC = gcc

#current path to prg
PRG = "/home/nudon/prg/gam/sdl/"
#util path
UTIL = $(PRG)"util/"

#DIRNAV = $(UTIL)"dirNav/"
#DIRNAVSRC = $(DIRNAV)"dirNav.c"

MAP = $(M02)"myMap.o"
IMAGE = $(MO2)"myImage.o"

#linked library
LL = $(DIRNAVSRC) 

#Linkied objects
LO = myImage.o myMap.o myNPC.o temp.o

ALLOBJO = $(OBJO) $(LO)

ALLOBJ = $(OBJO) $(LO)

COMPILER_FLAGS = -Wall -g

#Special SDL config code that links libraries magically
SDL_CONFIG  = `sdl2-config --cflags --libs` -lSDL2_image

#things for threads
THREAD = "-lpthread"

TOTCONFIG = $(SDL_CONFIG) $(THREAD) $(GETMATH)
GETMATH = "-lm"

# $@ is a macro for name of rule
# $^ is all the deps for the rule
# $< is just the first dep for rule.

m02 : $(ALLOBJO)
	$(CC) $(COMPILER_FLAGS) -o $@ $^ $(TOTCONFIG) 
m02.o : m02.c
	$(CC) $(COMPILER_FLAGS) -c -o $@ $<
myMap.o : myMap.c myImage.c
	$(CC) $(COMPILER_FLAGS) -c -o $@ $<
myImage.o : myImage.c
	$(CC) $(COMPILER_FLAGS) -c -o $@ $<
myNPC.o : myNPC.c
	$(CC) $(COMPILER_FLAGS) -c -o $@ $<
temp.o : temp.c
	$(CC) $(COMPILER_FLAGS) -c -o $@ $<
clear:
	rm $(ALLOBJO) m02

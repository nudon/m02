MYNUM = 02
OBJO = m$(MYNUM).o

#incase I want to change compiler suddenly, or gcc changes names
CC = gcc

#Linkied objects
LO = myImage.o myMap.o myNPC.o temp.o myInput.o myList.o myMenu.o gameState.o systemLimits.o

ALLOBJO = $(OBJO) $(LO)

COMPILER_FLAGS = -Wall -g

#Special SDL config code that links libraries magically
SDL_CONFIG  = `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf

#things for threads
THREAD = "-lpthread"

GETMATH = "-lm"

TOTCONFIG = $(SDL_CONFIG) $(THREAD) $(GETMATH)


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
myInput.o : myInput.c
	$(CC) $(COMPILER_FLAGS) -c -o $@ $<
myList.o : myList.c
	$(CC) $(COMPILER_FLAGS) -c -o $@ $<
myMenu.o : myMenu.c
	$(CC) $(COMPILER_FLAGS) -c -o $@ $<
gameState.o : gameState.c
	$(CC) $(COMPILER_FLAGS) -c -o $@ $<
systemLimits.o : systemLimits.c
	$(CC) $(COMPILER_FLAGS) -c -o $@ $<
clear:
	rm $(ALLOBJO) m02

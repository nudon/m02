#include "myMap.h"
//struct -> string

extern const int SIZE_OF_PREAMBLE;

void testSave();

char* generatePreamble(char* text);

int getVersionNumber(char* text);

int getTotalLength(char* text);

char* getMapSegFromSaveFile(char* savedText);

char* intToString(int number);

char* tileToString(tile* tile);


char* tilePosToString(tilePos* tPos);

char* tileMapToString(tileMap* tMap);

char* mapToString(map* theMap);

//insert string and free
void isaf(char* dest, char * mallocedString, int* index);
//pair field and value
void pfav (char* dest, char* field, char* value, int *index);
//copy and increment len
void cail(char* dest, char* src, int* len);

//string -> struct

int stringToInt(char* string);

tilePos* stringToTilePos(char* string);

tile* stringToTile(char* string);

tileMap* stringToTileMap(char* text);

map* stringToMap(char* text);

//get struct string segment
char* gsss(char* text, char* structName);

//get index of start of search
int giosos(char* text, char* search);

//get value of field
char* gvof(char* structSeg, char* fieldName);

//turn string to int and free
int tstiaf(char* text);

//copy string to string and free
void cstsaf(char* dest, char* text);

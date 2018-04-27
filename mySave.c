#include <assert.h>
#include "mySave.h"
//#include "myEnv.h"
//#include "myMap.h"

//kind of wondering if I could write code to take struct definitions as a source file
//and output the correct function definitions to handle all the structs
//seems doable

//also there's probably memory leaks everywhere.

static int sizeofMap = 80000;
//preamble isn't really constant lenght, just wanted some max lenght
//as long as I don't start throwing strings around, should be good.
const int SIZE_OF_PREAMBLE = 16;
void testVersionAndLen();

void testSave() {
  tile* aTile = createTile(4,4);
  char* saved = tileToString(aTile);
  fprintf(stderr, "\n1|%s|\n", saved);
  tile* reTile = stringToTile(saved);
  char* resaved = tileToString(reTile);
  fprintf(stderr, "2|%s|\n", resaved);
  tileMap* doggy = initTiles(4,4);
  char* superTest = tileMapToString(doggy);
  fprintf(stderr, "1|%s|\n", superTest);
  tileMap* newDoggy = stringToTileMap(superTest);
  char* endTest = tileMapToString(newDoggy);
  fprintf(stderr, "2|%s|\n", endTest);
  if (!strcmp(superTest, endTest)) {
    fprintf(stderr, "YYYEEEEAAAAAAAAAHHHHHH!!!!!!!\n");
  }
  testVersionAndLen();
}

void testVersionAndLen() {
  char* text = "four";
  char* preamble = generatePreamble(text);
  printf("version is %d\n", getVersionNumber(preamble));
  printf("len is %d\n", getTotalLength(preamble));
}



//usually sufficient, but I put the map into a struct, so that breaks it.
//will increase to 5000. was 1025
//also kind of want a second len, because in some cases I have char asdf[bufferlen] to hold field names like "mapTiles"
static int bufferLen = 5000;

//structs
char* mapStruct = "map";
char* mapTiles = "mapTiles";
char* tileStruct = "tile";
char* tilePosStruct = "tilePos";
//field names

//for tiles
char* wallField = "wall";
char* tilePathField = "path";

//for tilePos
char* tPosX = "x";
char* tPosY = "y";

//internal things
char structStart = '(';
char* structStartP = "( ";
char structEnd = ')';
char* structEndP = ") ";
char fvDelim = ':';
char* fvDelimP = ":";
char pairDelim = ',';
char* pairDelimP = " , ";

char* arrayStruct = "array";
char arrayStart = '{';
char* arrayStartP = "{ ";
char arrayEnd = '}';
char* arrayEndP = "}";
char* arrayRows = "rows";
char* arrayCols = "cols";
char* arrayType = "type";
char* arrayElement = "arrayElement";
char* arrayElementRow = "row";
char* arrayElementCol = "col";

//some special versions for getting things from save file
//potentially save file has multiple top-level structs,
char* getMapSegFromSaveFile(char* savedText) {
  char* mapSeg = gsss(savedText, mapStruct);
  return mapSeg;
}

//want to provide some way of describing a save version number
//as well as character length
int versionNumber = 1;
char* generatePreamble(char* text) {
  int len = strlen(text);
  char* lenString = intToString(len);
  char* versionNumberString = intToString(versionNumber);
  //need to decide on one standard for always writing/reading the version number and total length, independant of version
  //could just have the two be in plaintext in a constant order, then maybe end preamble on some special character (dependant on version)
  //think it makes sense to have version number be first
  char* basicDelim = " ";
  int preambleLen = strlen(lenString) + strlen(versionNumberString) + strlen(basicDelim);
  char* combine = malloc(sizeof(char) * (preambleLen + 1));
  int* index = &(int){0};
  cail(combine, versionNumberString, index);
  cail(combine, basicDelim, index);
  cail(combine, lenString, index);
  cail(combine, basicDelim, index);
  assert(*index < SIZE_OF_PREAMBLE);
  while(*index < SIZE_OF_PREAMBLE) {
    cail(combine, " ", index);
  }
  free(versionNumberString);
  free(lenString);

  
  return combine;
}

int getVersionNumber(char* text) {
  int index = 0, verStart, verEnd;
  while(isspace(text[index])) {
    index++;
  }
  verStart = index;
  while(!isspace(text[index])) {
    index++;
  }
  verEnd = index;
  char versionNumString[verEnd - verStart + 1];
  memcpy(versionNumString, text + verStart, verEnd - verStart);
  versionNumString[verEnd] = '\0';
  return stringToInt(versionNumString);
  
}

int getTotalLength(char* text) {
  int index = 0, lenStart, lenEnd;
  //this part skips over the version number
  while(isspace(text[index])) {
    index++;
  }
  while(!isspace(text[index])) {
    index++;
  }
  //this part gets the total length
  while(isspace(text[index])) {
    index++;
  }
  lenStart = index;
  while(!isspace(text[index])) {
    index++;
  }
  lenEnd = index;
  char lenString[lenEnd - lenStart + 1];
  memcpy(lenString, text + lenStart, lenEnd - lenStart);
  lenString[lenEnd] = '\0';
  return stringToInt(lenString);
  
}

char* tileToString(tile* tile) {
  char* text = malloc(sizeof(char) * bufferLen + 1);
  int* index = &(int){0};
  cail(text, tileStruct, index);
  cail(text, structStartP, index);
  
  pfav(text, wallField, intToString(tile->isWall), index);
  pfav(text, tilePathField, tile->tilePath, index);
  //skipping item for now
  isaf(text, tilePosToString(tile->tilePosition), index);
  //copying the pairDelim because didn't want isaf to implicity do that
  cail(text, pairDelimP, index);  
  cail(text, structEndP, index);
  text[*index] = '\0';
  return text;
}


char* tilePosToString(tilePos* tPos) {
  char* text = malloc(sizeof(char) * bufferLen + 1);
  int* index = &(int){0};
  *index = 0;
  cail(text, tilePosStruct, index);
  cail(text, structStartP, index);

  pfav(text, tPosX, intToString(tPos->x), index);
  pfav(text, tPosY, intToString(tPos->y), index);
  
  cail(text, structEndP, index);
  return text; 
}

//so array(rows: int,  col: int, type:struct, arrayElement(row:int, col:int, struct(...)))
char* tileMapToString(tileMap* tMap) {
  char* text = malloc(sizeof(char) *sizeofMap);
  int* index = &(int){0};
  //*index = 0;

  cail(text, mapTiles, index);
  cail(text, structStartP, index);
  cail(text, arrayStruct, index);
  cail(text, arrayStartP, index);

  pfav(text, arrayCols, intToString(tMap->cols), index);
  pfav(text, arrayRows, intToString(tMap->rows), index);
  pfav(text, arrayType, tileStruct, index);
  for (int colIndex = 0; colIndex < tMap->cols; colIndex++) {
    for(int rowIndex = 0; rowIndex < tMap->rows; rowIndex++) {
      cail(text, arrayElement, index);
      cail(text, structStartP, index);

      pfav(text, arrayElementRow, intToString(rowIndex), index);
      pfav(text, arrayElementCol, intToString(colIndex), index);
      isaf(text, tileToString(getTileFromTileMapCord(tMap, colIndex, rowIndex)), index);
	
      
      cail(text, structEndP, index);
    }
  }  
  cail(text, arrayEndP, index);
  cail(text, structEndP, index);
  return text;
}

char* mapToString(map* theMap) {
  char* text = malloc(sizeof(char) * sizeofMap);
  int* index = &(int){0};
  cail(text, mapStruct, index);
  cail(text, structStartP, index);
  isaf(text, tileMapToString(theMap->tileMap), index);
  cail(text, structEndP, index);
  return text;
}



//insert string and free
void isaf(char* dest, char * mallocedString, int* index) {
  cail(dest, mallocedString, index);

  free(mallocedString);
}


//pair field and value
void pfav (char* dest, char* field, char* value, int *index) {
  cail(dest, field, index);
  cail(dest, fvDelimP, index);
  cail(dest, value, index);
  cail(dest, pairDelimP, index);  
}

//copy and increment len
void cail(char* dest, char* src, int* len) {
  int locLen = strlen(src);
  memcpy(dest + *len, src, locLen);
  *len = *len + locLen;

  //also null terminate?
  dest[*len] = '\0';
}

tilePos* stringToTilePos(char* string) {
  tilePos* tpos = createTilePos(0,0);
  tpos->x = tstiaf(gvof(string, tPosX));
  tpos->y = tstiaf(gvof(string, tPosY));
  return tpos;
}

tile* stringToTile(char* string) {
  int x, y;
  x = tstiaf(gvof(string, tPosX));
  y = tstiaf(gvof(string, tPosY));
  tile* aTile = createTile(x, y);
  aTile->isWall = tstiaf(gvof(string,wallField));
  cstsaf(aTile->tilePath, gvof(string, tilePathField));
  return aTile;
}

//so array(rows: int,  col: int, type:struct, arrayElement(row:int, col:int, struct(...)))
//since there are multiple arrayElements and gsss is kind of stupid in that regard(grabs first occurence)
//set index after gsss to start of arrayElement found, and apply then add to that the strlen of the struct segment
//so index jumps over to the next arrayElement
tileMap* stringToTileMap(char* text) {
  int rows = tstiaf(gvof(text, arrayRows));
  int cols = tstiaf(gvof(text, arrayCols));
  int row, col;
  int index = 0;
  char* element;
  char* tileSegment;
  tile* aTile;
  tileMap* tMap = initTiles(cols, rows);
  for (int colIndex = 0; colIndex < cols; colIndex++) {
    for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
      element = gsss(text + index, arrayElement);
      //have to add 1 and strlen(arrayElement to move past the : associated with arrayElement
      //otherwise the next call to gsss won't actually increment to the next array element
      index += giosos(text + index, arrayElement) + 1 + strlen(arrayElement);
      row = tstiaf(gvof(element, arrayElementRow));
      col = tstiaf(gvof(element, arrayElementCol));
      tileSegment = gsss(element, tileStruct);
      aTile = stringToTile(tileSegment);
      cloneTile(getTileFromTileMapCord(tMap, col, row), aTile);
            
      free(aTile);
      free(tileSegment);
      free(element);
		   
    }
  }
  return tMap;
}

map* stringToMap(char* text) {
  map* new = malloc(sizeof(map));
  char* tileMapSeg = gsss(text, mapTiles);
  new->tileMap = stringToTileMap(tileMapSeg);
  
  new->mapBG = cinterTiles(new->tileMap);

  return new;
}

//get struct string segment
char* gsss(char* text, char* structName) {
  char* structString = malloc(sizeof(char) * bufferLen + 1);
  int startCount = 0;
  int fi;

  int startOfSearch = giosos(text, structName);
  if (startOfSearch >= 0) {
    startCount = 0;
    fi = 0;
    while(text[startOfSearch + fi] != structStart) {
      fi++;
    }
    do{
      if (text[startOfSearch + fi] == structEnd) {
	startCount--;
      }
      else if (text[startOfSearch + fi] == structStart) {
	startCount++;
      }
      fi++;
    } while(text[startOfSearch + fi] != '\0' && startCount != 0);
    memcpy(structString, (text + startOfSearch), fi + 1);
    structString[fi + 1] = '\0';
  }
  return structString;;
}

//get index of start of search
int giosos(char* text, char* search) {
  int retVal = 0;
  int isDone = 0;
  char name[bufferLen];
  int index = 0;
  int bi;
  while(!isDone) {
    while (text[index] != '\0' && text[index] != structStart) {
      index++;
    }
    if (text[index] != '\0') {
      //go backwards until finding whitespace
      bi= 0;
      while(!isspace(text[index - bi]) && index - bi >= 0) {
	bi++;
      }
      memcpy(name, &(text[index - bi + 1]), bi - 1);
      name[bi - 1] = '\0';
      if (!strcmp(name, search)) {
	isDone = 1;
      }
      else {
	index++;
      }
    }
    else {
      isDone = -1;
    }
  }
  if (isDone == 1) {
    retVal = index - bi + 1;
  }
  else {
    retVal = -1;
  }
  return retVal;
}

//get value of field
char* gvof(char* structSeg, char* fieldName) {
  int isDone = 0;
  int index = 0;
  int bi, fi;
  char foundField[bufferLen + 1]; 
  char* value = malloc(sizeof(bufferLen + 1));
  while(!isDone) {
    bi = 0;
    while (structSeg[index] != fvDelim) {
      index++;
    }
    while(!isspace(structSeg[index - bi])) {
      bi++;
    }
    memcpy(foundField, structSeg + index - bi + 1, bi - 1);
    foundField[bi - 1] = '\0';
    if (!strcmp(foundField, fieldName)) {
      isDone = 1;
    }
    else {
      index++;
    }
  }
  fi = 0;
  while(structSeg[index + fi] != pairDelim && !isspace(structSeg[index + fi])) {
    fi++;
  }
  memcpy(value, structSeg + index + 1, fi - 1);
  value[fi - 1] = '\0';
  return value; 
}

//turn string to int and free
int tstiaf(char* text) {
  int ret = stringToInt(text);
  free(text);
  return ret;
}

//copy string to string and free
void cstsaf(char* dest, char* text) {
  memcpy(dest, text, strlen(text));
  dest[strlen(text)] = '\0';
  free(text);
}

int stringToInt(char* string) {
  return atoi(string);
}

char* intToString(int number) {
  int size = sizeof(int) * 8;
  char* text = malloc(size);
  snprintf(text, size, "%d\0", number);
  return text;
}


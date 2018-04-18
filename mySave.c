#include "mySave.h"
//#include "myMap.h"

//kind of wondering if I could write code to take struct definitions as a source file
//and output the correct function definitions to handle all the structs
//seems doable

//also there's probably memory leaks everywhere.

static int sizeofMap = 80000;

void testSave() {
  tile* aTile = createTile(4,4);
  char* saved = tileToString(aTile);
  fprintf(stderr, "1|%s|\n", saved);
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
}



static int bufferLen = 1024;

//structs
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
  *index = 0;
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
//since there are multiple arrayElements and gsss is kind of stupid in that regard(grabs first)
//will add to index the strlen of the previouse gsss. Will break if there are internal arrayElements
//other option is find start of arrayElement, and apply the index offset off of that index.
//wanted to do first, but seems to janky. 
tileMap* stringToTileMap(char* text) {
  //int index = 0;
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

//get struct string segment
//fucks up if nested structs. need to keep count of structStarts
char* gsss(char* text, char* structName) {
  int isDone = 0;
  char* structString = malloc(sizeof(char) * bufferLen + 1);
  char name[bufferLen];
  int index = 0;
  int startCount = 0;
  int bi, fi;
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
      if (!strcmp(name, structName)) {
	isDone = 1;
      }
      else {
	index++;
      }
    }
    else {
      //reached end of string, didn't find structName
      free(structString);
      isDone = -1;
      //text = NULL;
    }
  }
  if (isDone == 1) {
    startCount = 0;
    fi = 0;
    do{
      if (text[index + fi] == structEnd) {
	startCount--;
      }
      else if (text[index + fi] == structStart) {
	startCount++;
      }
      fi++;
    } while(text[index + fi] != '\0' && startCount != 0);
    memcpy(structString, (text + index - bi + 1), fi + bi - 1);
    structString[bi + fi - 1] = '\0';
  }
  return structString;;
}

//get index of start of search

//get next occurence of search? 
int giosos(char* text, char* search) {
  int retVal = 0;
  int isDone = 0;
  char* structString = malloc(sizeof(char) * bufferLen + 1);
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
      while(!isspace(text[index - bi])) {
	bi++;
      }
      memcpy(name, &(text[index - bi + 1]), bi - 1);
      if (strcmp(name, search)) {
	isDone = 1;
      }   
    }
    else {
      //reached end of string, didn't find structName
      free(structString);
      isDone = -1;
      //text = NULL;
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
//also busted, doesn't actually search for fieldName
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


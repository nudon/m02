#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "systemLimits.h"

int SCREEN_WIDTH = 480;
int SCREEN_HEIGHT = 360;
char* MEDIA_DIR = NULL;
//probably better to interpret the slashes as collapsing things
//or put medai folder withing running dir. or do symbolic links
static char* fromLocalToMedia = "../../../media/";

void writeFilePath(char* dest, char* textPath) {
  int len = strlen(textPath);
  if (len <= MAXPATHLEN ) {
    memcpy(dest,textPath, len);
    //copyWithMediaPrefix(dest, textPath);
    dest[len] = '\0';
  }
  else {
    fprintf(stderr, "String %s\nis %d characters long, %d is the limit\n",
	    textPath, len, MAXPATHLEN );
  }
}

int setMediaDir() {
  int ret;
  char* temp = SDL_GetBasePath();
  if (temp != NULL) {
    int tempLen = strlen(temp);
    int fltmLen = strlen(fromLocalToMedia);
    int len = tempLen + fltmLen;
    free(MEDIA_DIR);
    MEDIA_DIR = malloc(sizeof(char) * len + 1);
    memcpy(MEDIA_DIR, temp, tempLen);
    memcpy(&(MEDIA_DIR[tempLen]), fromLocalToMedia, fltmLen);
    MEDIA_DIR[tempLen + fltmLen] = '\0';
    fprintf(stderr, "%s\n", temp);
    fprintf(stderr, "%s\n", fromLocalToMedia);
    fprintf(stderr, "%s\n", MEDIA_DIR);
    ret = 0;
  }
  else {
    fprintf(stderr, "unable to get working dir, enjoy\n");
    ret = 1;
  }
  return ret;
}

void copyWithMediaPrefix(char* dest, char* text) {
  int preLen = strlen(MEDIA_DIR);
  int textLen = strlen(text);
  
  memcpy(dest, MEDIA_DIR, preLen);
  memcpy(&(dest[preLen]), text, textLen);
  dest[preLen + textLen] = '\0';
}

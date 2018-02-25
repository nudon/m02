#ifndef FILE_MYIMAGE_SEEN
#define FILE_MYIMAGE_SEEN
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>

SDL_Surface* createSurfaceFromDim(int w, int h);

SDL_Surface* loadToSurface(char* path);

SDL_Texture* loadTextureAlt(char* path);

SDL_Texture* loadSurfaceToTexture(SDL_Surface* loadSurf);

SDL_Texture* loadTexture(char* path);


SDL_Renderer* getRenderer();

void setRenderer(SDL_Renderer* new);

void freeRenderer();


//temporary stuff for draw primitives

void myDrawRect(int x1, int y1, int x2, int y2);


void myDrawCirc(int x, int y, int rad);

double mSq(double b);

void myDrawFunRect(int x1 , int y1, int x2, int y2, int layers);


#endif

#include "myImage.h"

static SDL_Renderer* rend;

SDL_Surface* loadToSurface(char* path) {
  SDL_Surface* surface = IMG_Load(path);
  if (surface == NULL) {
    fprintf(stderr, "Error in loading picture %s to surface: %s \n",
	    path, 
	    SDL_GetError());
  }
  return surface;  
}

SDL_Texture* loadTextureAlt(char* path) {
  SDL_Texture* newText = IMG_LoadTexture(rend, path);
  if (newText == NULL) {
    fprintf(stderr, "Error in converting surface to Texture: %s \n",
	    SDL_GetError());
  }
  return newText;
}

SDL_Texture* loadSurfaceToTexture(SDL_Surface* loadSurf) {
  SDL_Texture* newText;
  newText = SDL_CreateTextureFromSurface(rend, loadSurf);
  if (newText == NULL) {
    fprintf(stderr, "Error in converting surface to Texture: %s \n",
	    SDL_GetError());
    newText = NULL;
  }
  return newText;
}

SDL_Texture* loadTexture(char* path) {
  SDL_Texture* newText;
  SDL_Surface* loadSurf = loadToSurface(path);
  if (loadSurf != NULL) {
    newText = loadSurfaceToTexture(loadSurf);
  }
  else {
    fprintf(stderr, "Error in loading file %s: %s \n",
	    path,
	    IMG_GetError());
    newText = NULL;
  }
  return newText;
}

SDL_Surface* createSurfaceFromDim(int w, int h) {
  //I stole this from https://wiki.libsdl.org/SDL_CreateRGBSurface
  SDL_Surface* surface;
  Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
#else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
#endif
  surface = SDL_CreateRGBSurface(0, w, h,32,rmask, gmask,  bmask, amask);
  //my crimes end here.
  return surface;
}

SDL_Renderer* getRenderer() {
  return rend;
}

void setRenderer(SDL_Renderer* new) {
  rend = new;
}

void freeRenderer() {
  SDL_DestroyRenderer(rend);
}

//#include <SDL2/SDL.h>
#include "myImage.h"
#include <math.h>
#include "myMenu.h"

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

void drawText(TTF_Font* font, char* text, SDL_Color* textColor, SDL_Rect* dstRect) {
  SDL_Texture* texture = drawTextToTexture(font, text, textColor);
  SDL_RenderCopy(getRenderer(), texture, NULL, dstRect);
  SDL_DestroyTexture(texture);
}

SDL_Texture* drawTextToTexture(TTF_Font* font, char* text, SDL_Color* textColor) {
  SDL_Surface* textSurf;
  SDL_Texture* textText;
  textSurf = TTF_RenderText_Solid(font, text, *textColor);
  textText = loadSurfaceToTexture(textSurf);
  SDL_FreeSurface(textSurf);
  return textText;
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


//temporary stuff for draw primitives
#include <math.h>
void myDrawRect(int x1, int y1, int x2, int y2) {
  SDL_Rect rect;
  rect.x = x1;
  rect.y = y1;
  rect.w = x2 - x1;
  rect.h = y2 - y1;
  SDL_RenderFillRect(getRenderer(), &rect);
}


void myDrawCirc(int x, int y, int rad) {
  double theta = 0;
  double halfACircle = M_PI;
  int quality = 100;
  double dTheta = M_PI / quality;
  int c1x, c2x, c1y, c2y;
  while(theta < halfACircle) {
    c1x = x + rad * cos(theta);
    c1y = y + rad * sin(theta);
    c2x = x;
    c2y = y - rad * sin(theta);
    myDrawRect(c1x, c1y, c2x, c2y);
    theta += dTheta;
  }

}

double mSq(double b) {
  return b*b;
}

void myDrawFunRect(int x1 , int y1, int x2, int y2, int layers) {
  if (layers > 0) {
    SDL_Color old;
    SDL_Renderer* rend = getRenderer();
    if (SDL_GetRenderDrawColor(rend, &old.r, &old.g, &old.b, &old.a) == 0) {
      int r;
      int g;
      int b;
      int scrmble = layers;
      int dx, dy;
      dx = ((x2 - x1) / 2) / layers;
      dy = ((y2 - y1) / 2) / layers;
      for (int i = 0; i <  layers; i++) {
	r = 255 * mSq(sin(i * M_PI/scrmble));
	r = r << (sizeof(int) / 2);
	g = 255 * mSq(cos(i * M_PI/scrmble));
	g = g << (sizeof(int) / 2);
	b = 255 * mSq(tan(i * M_PI/scrmble));
	b = b << (sizeof(int) / 2);
	SDL_SetRenderDrawColor( rend, r, g, b, 0xFF );
	myDrawRect( x1 + dx * i, y1 + dy * i, x2 - dx * i, y2 - dy * i);
      }
      SDL_SetRenderDrawColor(rend, old.r, old.g, old.b, old.a);
    }
    else {
      fprintf(stderr, "Unable to backup previous drawRender settings: %s  \n", SDL_GetError());
    }
  }
  else {
    fprintf(stderr, "FunRect passed negative or zero layers \n");
  }
}

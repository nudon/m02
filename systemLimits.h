#ifndef FILE_SYSTEMLIMITS_SEEN
#define FILE_SYSTEMLIMITS_SEEN
//idea for this, has 2 parts
//1, define some constants for screensize, filepath limit, and so on
//2, have some functions to do nice things
//have one to write a value to a string field, checks if size is okay
//plan on other things for handling file management
//since I don't like hardcoding paths to things
#define MAXPATHLEN 255
//extern int MAXPATHLEN;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int TILED;
extern char* MEDIA_DIR;

void writeFilePath(char* dest, char* textPath);
int setMediaDir();
void copyWithMediaPrefix(char* dest, char* text);

#endif

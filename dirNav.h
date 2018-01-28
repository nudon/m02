#ifndef FILE_DIRNAV_SEEN
#define FILE_DIRNAV_SEEN

char** imgsInDir(char* path, char opt);

void freeImgsInDir(char** imgArray);

int numOfFiles(char** imgArray);

#endif

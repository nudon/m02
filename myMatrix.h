#ifndef FILE_MYMATRIX_SEEN
#define FILE_MYMATRIX_SEEN

typedef
enum {
  TYPE_NPCLIST
} elementType;

typedef
 struct {
  int cols;
  int rows;
  void*** data;
  elementType type;
} gen_matrix;

//still thinking about whether I should include type information
//Would mean I either have to manually compare the type everytime I access something
//or maintain functions that do the typechecking for me, and just have variations for everytype.
//well, already maintaining a elementType for everything, why not include functions


gen_matrix* newGen_matrix(int cols, int rows);

void freeGen_marix(gen_matrix* rm);

void* getDataAtIndex(gen_matrix* matrix, int col, int row);

void setDataAtIndex(gen_matrix* matrix, int col, int row, void* item);

#endif

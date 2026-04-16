#include "pointer.h"

int main()
{
  Pointer * p = (Pointer *) malloc(sizeof(Pointer));
  if (!init_pointer("lexer-test2.c", p)) {
    printf("FAILED TO INIT POINTER\n");
    return 1;
  }; 

  for (int i=0; i<70; i++) {
    read_next_token(p, 0);
  }
  for (int i=0; i<11; i++) {
    read_next_token(p, 1);
  }
  
  free(p);
  return 0;
}
#include "pointer.h"
#include "parser.h"

int main()
{
  Pointer * p = (Pointer *) malloc(sizeof(Pointer));
  if (!init_pointer("lexer-test3.c", p)) {
    printf("FAILED TO INIT POINTER\n");
    return 1;
  }; 

  Parser * parser = (Parser *) malloc(sizeof(Parser));

  init_parser(parser, p);

  int a = parse_code(parser);
  printf("\n%d\n", a);
  return a;
  int sline = 1;
  int eline = 2;

  while (p->row < sline) {
    Token * tk= read_next_token(p, 0);
    if (p->row == sline) {
      prt_token(tk, 1);
    }
  }

  while (p->row < eline + 1) {
    read_next_token(p, 1);
  }

  free(p);
  return 0;
}
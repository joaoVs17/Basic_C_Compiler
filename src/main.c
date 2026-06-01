#include "pointer.h"
#include "parser.h"

int main()
{
  Pointer * p = (Pointer *) malloc(sizeof(Pointer));
  if (!init_pointer("parser-test1.c", p)) {
    printf("FAILED TO INIT POINTER\n");
    return 1;
  }; 

  Parser * parser = (Parser *) malloc(sizeof(Parser));

  init_parser(parser, p);

  ASTNode *ast = parse_code(parser);
  if (ast) {
    printf("\n\n---------AST---------\n\n");
    prt_ast(ast, 0);
    destroy_ast(ast);
    printf("\n");
    return 0;
  } else {
    prt_parser_errors(parser);
    return 1;
  }
  int sline = 1;
  int eline = 2;

  //Ler tudo
  //Ler tudo

  while (1) {
    Token *tk =read_next_token(p, 0);
    prt_token(tk, 1);
    if (tk->type == TOKEN_EOF) {
      return 0;
    }
  }

  return 0;


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

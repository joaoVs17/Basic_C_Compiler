#include "./lexer/lexer.h"
#include "./parser/parser.h"

int main() {

  char *file_path = "./docs/parser-test4.c";
  Pointer *pointer = (Pointer *)malloc(sizeof(Pointer));
  if (!init_pointer(file_path, pointer)) {
    printf("FAILED TO INIT POINTER\n");
    return 1;
  }; 

  // while (1) {
  //   Token *tk = read_next_token(pointer, 0);
  //   prt_token(tk, 1);
  //   if (tk->type == TOKEN_EOF) {
  //     return 0;
  //   }
  // }

  Parser parser;
  ParseStack stack;
  init_parser(&parser, pointer);
  parse_stack_init(&stack);
  if (parse(&parser, &stack)) {
    printf("ESCREVESTE UM C QUE EU CÁ SEI BEM LER\n");
  } else {
    printf("TENS MUITO A MELHORAR\n");
  };
  printf("Stack count: %d\n", stack.count);
  
  return 0;
}

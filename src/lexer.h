#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
  KEYWORD,
  IDENTIFIER,
  NUMBER,
  OPERATOR,
  SEPARATOR,
  COMMENT,
  LITERAL,
  UNKNOWN,
  NONE,
} TokenType;

typedef struct pointer
{
  int col, row;
  int scol, srow;
  int value;
  TokenType reading_state;
  int long curr_pos;
  int long start_pos;
  FILE *stream;
} Pointer;

typedef struct
{
  TokenType type;
  char *lex;
  int row, col;
} Token;

int advance_pointer(Pointer *p);
int advance_pointer_n(Pointer *p, int n);
int change_pointer_state(Pointer *p, TokenType state);
int read_next_token(Pointer *p);
int is_valid_token_start(int c);
TokenType get_token_start_type(int c);

void update_pointer_position(Pointer *p); // Must be used only inside advance_pointer
void update_pointer_state(Pointer *p);    // Must be used only inside advance_pointer

#endif
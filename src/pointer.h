#ifndef POINTER_H
#define POINTER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


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

typedef enum {
  STATE_INITIAL,

  STATE_SLASH,              // viu '/'
  STATE_COMMENT_LINE,       // //
  STATE_COMMENT_BLOCK,      // /*
  STATE_COMMENT_BLOCK_END,  // viu '*' dentro do bloco

  STATE_LITERAL,
  STATE_OPEN_LITERAL,
  STATE_CLOSED_LITERAL,
  STATE_OPEN_SIMPLE_LITERAL,
  STATE_CLOSED_SIMPLE_LITERAL,


  STATE_IDENTIFIER,
  STATE_SEPARATOR,
  STATE_NUMBER,
  STATE_FLOAT_NUMBER_WITHOUT_DECIMAL,
  STATE_FLOAT_NUMBER,
  STATE_ERROR_NUMBER_MANY_DOTS,
  STATE_OPERATOR,
  STATE_DONE,
  STATE_ERROR
} ReadingState;


typedef struct
{
  TokenType type;
  char *lex;
  int row, col;
} Token;

typedef struct pointer
{
  int col, row;
  int pivot_col, pivot_row;
  int value;
  ReadingState reading_state;
  long curr_pos;
  long pivot_pos;
  FILE *stream;
  Token *token;
} Pointer;


int advance_pointer(Pointer *p);
int advance_pointer_n(Pointer *p, int n);
int change_pointer_state(Pointer *p, ReadingState state);
Token * read_next_token(Pointer *p, int prt);
int is_valid_token_start(int c);
ReadingState detect_start_state(int c);

void update_pointer_position(Pointer *p); // Must be used only inside advance_pointer
void update_pointer_state(Pointer *p);    // Must be used only inside advance_pointer

int init_pointer(char * filename, Pointer *p);
void prt_pointer(Pointer *p);
void prt_token(Token *t, int br);
Token * get_token_from_pointer(Pointer *p);
TokenType classify_token(char * lex, ReadingState state);  

int is_separator_start(int c);

//Char groups identifiers

int is_letter(int c);
int is_digit(int c);
int is_whitespace(int c);

int is_identifier_start(int c);
int is_identifier_part(int c);

int is_operator_char(int c);
int is_separator_char(int c);

int is_quote(int c);
int is_slash(int c);
int is_star(int c);

int is_newline(int c);

int is_token_start(int c);
void update_pivot(Pointer *p);
void set_pointer_state(Pointer *p, ReadingState state);

#endif
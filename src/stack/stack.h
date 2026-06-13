#ifndef STACK_H
#define STACK_H

#define PARSE_STACK_MAX 1024
#define NON_TERMINALS_COUNT 23
#define TERMINALS_COUNT 30

#define FIRST_NON_TERMINAL SYM_CODE
#define LAST_NON_TERMINAL SYM_TYPE

#define FIRST_TERMINAL SYM_IDENTIFIER
#define LAST_TERMINAL SYM_EOF

typedef enum {

  SYM_CODE,
  SYM_BLOCK,
  SYM_PROG,
  SYM_DECL,

  SYM_VAR_DECLARATION,
  SYM_VAR_DECLARATION_TAIL,

  SYM_ASSIGNMENT,

  SYM_CONDITION,
  SYM_CONDITION_TAIL,

  SYM_WHILE_LOOP,

  SYM_EXPR,

  SYM_LOGICAL_OR_EXPR,
  SYM_LOGICAL_OR_EXPR_TAIL,

  SYM_LOGICAL_AND_EXPR,
  SYM_LOGICAL_AND_EXPR_TAIL,

  SYM_EQUALITY_EXPR,
  SYM_EQUALITY_EXPR_TAIL,

  SYM_RELATIONAL_EXPR,
  SYM_RELATIONAL_EXPR_TAIL,

  SYM_ADD_EXPR,
  SYM_ADD_EXPR_TAIL,

  SYM_TERM,
  SYM_TERM_TAIL,

  SYM_FACTOR,

  SYM_TYPE,

  SYM_IDENTIFIER,
  SYM_NUMBER,
  SYM_STRING_LITERAL,
  SYM_CHAR_LITERAL,

  SYM_INT,
  SYM_FLOAT,
  SYM_CHAR,
  SYM_STRING,

  SYM_IF,
  SYM_ELSE,
  SYM_WHILE,

  SYM_ASSIGN,
  SYM_EQUAL,
  SYM_NOT_EQUAL,

  SYM_LESS,
  SYM_LESS_EQUAL,
  SYM_GREATER,
  SYM_GREATER_EQUAL,

  SYM_AND,
  SYM_OR,

  SYM_PLUS,
  SYM_MINUS,
  SYM_MULTIPLY,
  SYM_DIVIDE,

  SYM_LPAREN,
  SYM_RPAREN,

  SYM_LBRACE,
  SYM_RBRACE,

  SYM_SEMICOLON,

  SYM_EOF,

  SYM_EPSILON

} GrammarSymbol;

typedef struct {
  GrammarSymbol items[PARSE_STACK_MAX];
  int count;
} ParseStack;

void parse_stack_init(ParseStack *stack);
void parse_stack_clear(ParseStack *stack);

int parse_stack_push(ParseStack *stack, GrammarSymbol symbol);
int parse_stack_pop(ParseStack *stack, GrammarSymbol *out_symbol);
int parse_stack_peek(const ParseStack *stack, GrammarSymbol *out_symbol);

int parse_stack_is_empty(const ParseStack *stack);
int parse_stack_is_full(const ParseStack *stack);
int parse_stack_size(const ParseStack *stack);
void parse_stack_print(const ParseStack *stack);

char *grammar_symbol_to_string(GrammarSymbol sym);
void print_symbol(GrammarSymbol sym);

#endif

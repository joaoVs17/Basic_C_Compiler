#ifndef PARSE_STACK_H
#define PARSE_STACK_H

#include "../lexer/lexer.h"
#include <stdarg.h>

#define PARSE_STACK_MAX 1024
#define NON_TERMINALS_COUNT 23
#define TERMINALS_COUNT 30

// Pra facilitar saber quais são ou não terminais

#define FIRST_NON_TERMINAL SYM_CODE
#define LAST_NON_TERMINAL SYM_TYPE

#define FIRST_TERMINAL SYM_IDENTIFIER
#define LAST_TERMINAL SYM_EOF

// Macro pra deixar fácil dar push_many
#define SYMBOL(s) ((ParseItem){ .type = PARSE_ITEM_SYMBOL, .symbol = (s) })
#define ACTION(a, t) ((ParseItem){ .type = PARSE_ITEM_ACTION, .action = { (a), (t) } })

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

typedef enum { ITEM_SYMBOL,
               ITEM_ACTION } ItemType;

typedef enum {
  ACT_BUILD_BINARY,       // pop 2 operandos -> BINARY_EXPR (usa o token do operador)
  ACT_BUILD_ASSIGN,       // pop expr, ident -> ASSIGNMENT
  ACT_BUILD_VARDECL,      // pop ident, type -> VAR_DECLARATION (sem init)
  ACT_BUILD_VARDECL_INIT, // pop expr, ident, type -> VAR_DECLARATION (com init)
  ACT_BUILD_TYPE,         // cria nó TYPE a partir do token capturado
  ACT_BUILD_IDENTIFIER,   // cria folha IDENTIFIER a partir do token capturado
  ACT_BUILD_NUMBER,       // cria folha NUMBER a partir do token capturado
  ACT_BUILD_STRING,       // cria folha STRING_LITERAL a partir do token capturado
  ACT_BUILD_CHAR,         // cria folha CHAR_LITERAL a partir do token capturado
  ACT_BUILD_IF,           // pop block, cond -> IF
  ACT_BUILD_IF_ELSE,      // pop else-block, then-block, cond -> IF (com else)
  ACT_BUILD_WHILE,        // pop block, cond -> WHILE
  ACT_OPEN_BLOCK,         // empilha um marcador de início de bloco no NodeStack
  ACT_BUILD_BLOCK,        // colhe tudo até o marcador -> BLOCK
  ACT_BUILD_PROGRAM,      // colhe tudo até o marcador -> PROGRAM/CODE
  ACT_EMIT_PUSH_NUM,
  ACT_EMIT_PUSH_VAR,
} ASTAction;

typedef struct {
  ItemType type;
  GrammarSymbol symbol; // se ITEM_SYMBOL
  ASTAction action;     // se ITEM_ACTION
  Token *token;         // operador/token capturado p/ a ação
} StackItem;

typedef struct {
  StackItem items[PARSE_STACK_MAX];
  int count;
} ParseStack;

typedef enum {
  PARSE_ITEM_SYMBOL,
  PARSE_ITEM_ACTION
} ParseItemType;

typedef struct {
  ParseItemType type;
  union {
    GrammarSymbol symbol;
    struct {
      ASTAction action;
      Token *token;
    } action;
  };
} ParseItem;

void parse_stack_init(ParseStack *stack);
void parse_stack_clear(ParseStack *stack);

int parse_stack_push_symbol(ParseStack *stack, GrammarSymbol symbol);
int parse_stack_push_action(ParseStack *stack, ASTAction action, Token *token);
int parse_stack_push_many(ParseStack *stack, int count, ...);
int parse_stack_pop(ParseStack *stack, StackItem *out_item);
int parse_stack_peek(const ParseStack *stack, StackItem *out_item);

int parse_stack_is_empty(const ParseStack *stack);
int parse_stack_is_full(const ParseStack *stack);
int parse_stack_size(const ParseStack *stack);
void parse_stack_print(const ParseStack *stack);

char *grammar_symbol_to_string(GrammarSymbol sym);
char *ast_action_to_string(ASTAction action);
void print_symbol(GrammarSymbol sym);
void print_item(const StackItem *item);

#endif

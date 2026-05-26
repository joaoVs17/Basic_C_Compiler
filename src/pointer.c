#include "pointer.h"

static const char *reading_state_to_str(ReadingState s) {
  switch (s) {
  case STATE_INITIAL:
    return "STATE_INITIAL";
  case STATE_SLASH:
    return "STATE_SLASH";
  case STATE_COMMENT_LINE:
    return "STATE_COMMENT_LINE";
  case STATE_COMMENT_BLOCK:
    return "STATE_COMMENT_BLOCK";
  case STATE_COMMENT_BLOCK_ASTERISK:
    return "STATE_COMMENT_BLOCK_ASTERISK";
  case STATE_COMMENT_BLOCK_END:
    return "STATE_COMMENT_BLOCK_END";
  case STATE_OPERATOR:
    return "STATE_OPERATOR";
  case STATE_PLUS:
    return "STATE_PLUS";
  case STATE_MINUS:
    return "STATE_MINUS";
  case STATE_TIMES:
    return "STATE_TIMES";
  case STATE_ASSIGN:
    return "STATE_ASSIGN";
  case STATE_EXCLAMATION_MARK:
    return "STATE_EXCLAMATION_MARK";
  case STATE_DONE:
    return "STATE_DONE";
  case STATE_ERROR:
    return "STATE_ERROR";
  case STATE_SEPARATOR:
    return "STATE_SEPARATOR";
  default:
    return "INVALID";
  }
}

void prt_pointer(Pointer *p) {
  if (p == NULL) {
    printf("Pointer: NULL\n");
    return;
  }

  printf("=== POINTER STATE ===\n");
  printf("row: %d\n", p->row);
  printf("col: %d\n", p->col);
  printf("pivot_row: %d\n", p->pivot_row);
  printf("pivot_col: %d\n", p->pivot_col);
  printf("value: %c\n", p->value);
  printf("reading_state: %s\n", reading_state_to_str(p->reading_state));
  printf("curr_pos: %ld\n", p->curr_pos);
  printf("pivot_pos: %ld\n", p->pivot_pos);
  printf("stream: %s\n", p->stream ? "OPEN" : "NULL");
  printf("=====================\n");
}

int init_pointer(char *filename, Pointer *p) {

  p->stream = fopen(filename, "rb");
  if (p->stream == NULL) {
    return 0;
  }
  p->col = 0;
  p->row = 1;
  p->reading_state = STATE_INITIAL;
  p->curr_pos = ftell(p->stream);
  p->pivot_col = 0;
  p->pivot_row = 0;
  p->pivot_pos = ftell(p->stream);
  p->value = -1;
  p->token = NULL;

  return 1;
}

void update_pointer_position(Pointer *p) {
  if (p->value == '\n') {

    p->col = 0;
    p->row += 1;
  } else {
    p->col += 1;
  }
  p->curr_pos = ftell(p->stream);
}

void emit_token(Pointer *p) { p->token = get_token_from_pointer(p); }

void restart_at_current_char(Pointer *p) {
  p->reading_state = detect_start_state(p->value);
  update_pivot(p);
}

void emit_and_restart(Pointer *p) {
  emit_token(p);
  restart_at_current_char(p);
}

void emit_and_go_to(Pointer *p, ReadingState state) {
  emit_token(p);
  p->reading_state = state;
}

// HANDLE TRANSITIONS

void handle_initial_transition(Pointer *p, int c) {
  ReadingState tt = detect_start_state(c);
  set_pointer_state(p, tt);

  // After change from NONE to any reading state, must set the pivot position as
  // current position pointer must update the pos
  p->pivot_col = p->col;
  p->pivot_row = p->row;
  p->pivot_pos = p->curr_pos - 1;
}

void handle_slash_transition(Pointer *p, int c) {
  if (c == '/') {
    p->reading_state = STATE_COMMENT_LINE;
  } else if (c == '*') {
    p->reading_state = STATE_COMMENT_BLOCK;
  } else if (c == '=') {
    p->reading_state = STATE_OPERATOR;
  } else {
    emit_and_restart(p);
  }
}

void handle_comment_line_transition(Pointer *p, int c) {
  if (c == '\n') {
    emit_and_go_to(p, STATE_INITIAL);
  }
}

void handle_comment_block_transition(Pointer *p, int c) {
  if (c == '*') {
    p->reading_state = STATE_COMMENT_BLOCK_ASTERISK;
  }
}

void handle_comment_block_asterisk_transition(Pointer *p, int c) {
  if (c == '*') {
    p->reading_state = STATE_COMMENT_BLOCK_ASTERISK;
  } else if (c == '/') {
    p->reading_state = STATE_COMMENT_BLOCK_END;
  } else {
    p->reading_state = STATE_COMMENT_BLOCK;
  }
}

void handle_comment_block_end_transition(Pointer *p) {
  emit_and_restart(p);
}

void handle_error_transition(Pointer *p, int c) {
  if ((is_separator_start(c))) {
    emit_and_go_to(p, STATE_SEPARATOR);
    update_pivot(p);
  } else if (is_operator_char(c)) {
    emit_and_restart(p);
  } else if (is_whitespace(c)) {
    emit_and_go_to(p, STATE_INITIAL);
  }
}

void handle_identifier_transition(Pointer *p, int c) {
  if ((is_separator_start(c))) {
    emit_and_restart(p);
    // update_pivot(p);
  } else if (is_operator_char(c)) {
    emit_and_restart(p);
  } else if (is_whitespace(c)) {
    emit_and_go_to(p, STATE_INITIAL);
  }
}

void handle_separator_transition(Pointer *p, int c) {
  if (c == ';') {
    printf("DSA\n");
  }
  emit_and_restart(p);
}

void handle_operator_transition(Pointer *p) {
  emit_and_restart(p);
}

void handle_plus_transition(Pointer *p, int c) {
  if (c == '=' || c == '+') {
    p->reading_state = STATE_OPERATOR;
  } else {
    emit_and_restart(p);
  }
}

void handle_minus_transition(Pointer *p, int c) {
  if (c == '=' || c == '-') {
    p->reading_state = STATE_OPERATOR;
  } else {
    emit_and_restart(p);
  }
}

void handle_times_transition(Pointer *p, int c) {
  if (c == '=') {
    p->reading_state = STATE_OPERATOR;
  } else {
    emit_and_restart(p);
  }
}

void handle_assign_transition(Pointer *p, int c) {
  if (c == '=') {
    p->reading_state = STATE_OPERATOR;
  } else {
    emit_and_restart(p);
  }
}

void handle_exclamation_mark_transition(Pointer *p, int c) {
  if (c == '=') {
    p->reading_state = STATE_OPERATOR;
  } else {
    emit_and_restart(p);
  }
}

void handle_number_transition(Pointer *p, int c) {
  if (c == '.') {
    p->reading_state = STATE_FLOAT_NUMBER_WITHOUT_DECIMAL;
  } else if ((is_separator_char(c) && c != '.') || c == ' ' || c == '\t' ||
             c == '\n' || is_operator_char(c)) {
    emit_and_restart(p);
  } else {
    if (!is_digit(c)) {
      // p->token = get_token_from_pointer(p);
      p->reading_state = STATE_ERROR;
      // update_pivot(p);
    }
  }
}

void handle_float_number_without_decimal_transition(Pointer *p, int c) {
  if (is_digit(c)) {
    p->reading_state = STATE_FLOAT_NUMBER;
  } else {
    p->reading_state = STATE_ERROR;
  }
}

void handle_float_number_transition(Pointer *p, int c) {
  if (c == '.') {
    p->reading_state = STATE_ERROR_NUMBER_MANY_DOTS;
  } else if ((is_separator_char(c) && c != '.') || c == ' ' || c == '\t' ||
             c == '\n' || is_operator_char(c)) {
    emit_and_restart(p);
  } else {
    if (!is_digit(c)) {
      // p->token = get_token_from_pointer(p);
      p->reading_state = STATE_ERROR;
      // update_pivot(p);
    }
  }
}

void handle_error_number_many_dots_transition(Pointer *p, int c) {
  if (((is_separator_start(c) && c != '.'))) {
    emit_and_go_to(p, STATE_SEPARATOR);
    update_pivot(p);
  } else if (c == ' ' || c == '\n' || c == '\t') {
    emit_and_go_to(p, STATE_INITIAL);
  }
}

void handle_open_literal_transition(Pointer *p, int c) {
  if (c == '\"') {
    p->reading_state = STATE_CLOSED_LITERAL;
  } else if (c == '\n') {
    p->reading_state = STATE_ERROR;
    emit_and_go_to(p, STATE_INITIAL);
    update_pivot(p);
  }
}

void handle_closed_literal_transition(Pointer *p) {
  emit_and_restart(p);
}

void handle_open_simple_literal_transition(Pointer *p, int c) {
  if (c == '\'') {
    p->reading_state = STATE_CLOSED_SIMPLE_LITERAL;
  } else if (c == '\n') {
    p->reading_state = STATE_ERROR;
    emit_and_go_to(p, STATE_INITIAL);
    update_pivot(p);
  }
}

void handle_closed_simple_literal_transition(Pointer *p) {
  emit_and_restart(p);
}

void update_pointer_state(Pointer *p) {
  if (p == NULL)
    return;

  int c = p->value;

  switch (p->reading_state) {
  case STATE_INITIAL:
    handle_initial_transition(p, c);
    break;
  case STATE_SLASH:
    handle_slash_transition(p, c);
    break;
  case STATE_COMMENT_LINE:
    handle_comment_line_transition(p, c);
    break;
  case STATE_COMMENT_BLOCK:
    handle_comment_block_transition(p, c);
    break;
  case STATE_COMMENT_BLOCK_ASTERISK:
    handle_comment_block_asterisk_transition(p, c);
    break;
  case STATE_COMMENT_BLOCK_END:
    handle_comment_block_end_transition(p);
    break;
  case STATE_ERROR:
    handle_error_transition(p, c);
    break;
  case STATE_IDENTIFIER:
    handle_identifier_transition(p, c);
    break;
  case STATE_SEPARATOR:
    handle_separator_transition(p, c);
    break;
  case STATE_OPERATOR:
    handle_operator_transition(p);
    break;
  case STATE_PLUS:
    handle_plus_transition(p, c);
    break;
  case STATE_MINUS:
    handle_minus_transition(p, c);
    break;
  case STATE_TIMES:
    handle_times_transition(p, c);
    break;
  case STATE_ASSIGN:
    handle_assign_transition(p, c);
    break;
  case STATE_EXCLAMATION_MARK:
    handle_exclamation_mark_transition(p, c);
    break;
  case STATE_NUMBER:
    handle_number_transition(p, c);
    break;
  case STATE_FLOAT_NUMBER_WITHOUT_DECIMAL:
    handle_float_number_without_decimal_transition(p, c);
    break;
  case STATE_FLOAT_NUMBER:
    handle_float_number_transition(p, c);
    break;
  case STATE_ERROR_NUMBER_MANY_DOTS:
    handle_error_number_many_dots_transition(p, c);
    break;
  case STATE_OPEN_LITERAL:
    handle_open_literal_transition(p, c);
    break;
  case STATE_CLOSED_LITERAL:
    handle_closed_literal_transition(p);
    break;
  case STATE_OPEN_SIMPLE_LITERAL:
    handle_open_simple_literal_transition(p, c);
    break;
  case STATE_CLOSED_SIMPLE_LITERAL:
    handle_closed_simple_literal_transition(p);
    break;
  default:
    break;
  }
}

Token *get_token_from_pointer(Pointer *p) {
  if (!p || !p->stream)
    return NULL;

  long start = p->pivot_pos;
  long end = p->curr_pos - 1;
  // printf("POSITION BEFORE READING TOKEN %d\n", ftell(p->stream));

  if (end < start)
    return NULL;
  long size = end - start;

  Token *token = (Token *)malloc(sizeof(Token));
  if (!token)
    return NULL;
  token->lex = (char *)malloc(size + 1);
  if (!token->lex) {
    free(token);
    return NULL;
  }

  fseek(p->stream, start, SEEK_SET);
  fread(token->lex, 1, size, p->stream);
  token->lex[size] = '\0';

  fseek(p->stream, end, SEEK_SET);

  token->row = p->pivot_row;
  token->col = p->pivot_col;
  token->type = classify_token(token->lex, p->reading_state);

  p->pivot_pos = p->curr_pos;
  p->pivot_row = p->row;
  p->pivot_col = p->col;

  // printf("POSITION AFTER READING TOKEN %d\n", ftell(p->stream));
  int c = fgetc(p->stream);
  // printf("TOKEN [%ld,%ld): '%s' // LAST READ VALUE: %c\n", start, end,
  // token->lex, p->value); printf("c: %c val: %c\n", c, p->value);
  return token;
}

int advance_pointer(Pointer *p) {
  int c = fgetc(p->stream);
  p->value = c;
  if (c == EOF)
    return 0;
  update_pointer_position(p);
  update_pointer_state(p);
}

ReadingState detect_start_state(int c) {

  if (c == '/')
    return STATE_SLASH;

  if (c == '+')
    return STATE_PLUS;

  if (c == '-')
    return STATE_MINUS;

  if (c == '*')
    return STATE_TIMES;

  if (c == '=')
    return STATE_ASSIGN;

  if (c == '!')
    return STATE_EXCLAMATION_MARK;

  //Some operators first

  if (c == '\n' || c == ' ' || c == '\t')
    return STATE_INITIAL;

  if (is_separator_char(c))
    return STATE_SEPARATOR;

  if (is_letter(c))
    return STATE_IDENTIFIER;

  if (is_digit(c))
    return STATE_NUMBER;

  if (is_operator_char(c))
    return STATE_OPERATOR;

  if (c == '\'')
    return STATE_OPEN_SIMPLE_LITERAL;

  if (c == '\"')
    return STATE_OPEN_LITERAL;

  return STATE_ERROR;
}

void prt_token(Token *t, int br) {
  if (t == NULL) {
    printf("TOKEN NULO \n");
    return;
  }
  printf("{lex: \x1b[32m %s\x1b[0m, ", t->lex);
  printf("type: \x1b[35m");
  switch (t->type) {
  case KEYWORD:
    printf("KEYWORD");
    break;
  case KEYWORD_TYPE:
    printf("KEYWORD_TYPE");
    break;
  case KEYWORD_RETURN:
    printf("KEYWORD_RETURN");
    break;
  case KEYWORD_IF:
    printf("KEYWORD_IF");
    break;
  case KEYWORD_ELSE:
    printf("KEYWORD_ELSE");
    break;
  case KEYWORD_WHILE:
    printf("KEYWORD_WHILE");
    break;
  case KEYWORD_FOR:
    printf("KEYWORD_FOR");
    break;
  case KEYWORD_BREAK:
    printf("KEYWORD_BREAK");
    break;
  case KEYWORD_CONTINUE:
    printf("KEYWORD_CONTINUE");
    break;
  case KEYWORD_STRUCT:
    printf("KEYWORD_STRUCT");
    break;
  case IDENTIFIER:
    printf("IDENTIFIER");
    break;
  case LITERAL:
    printf("LITERAL");
    break;
  case COMMENT:
    printf("COMMENT");
    break;
  case OPERATOR:
    printf("OPERATOR");
    break;
  case OPERATOR_ASSIGNMENT:
    printf("OPERATOR_ASSIGNMENT");
    break;
  case OPERATOR_ARITMETIC:
    printf("OPERATOR_ARITMETIC");
    break;
  case OPERATOR_LOGIC:
    printf("OPERATOR_LOGIC");
    break;
  case SEPARATOR:
    printf("SEPARATOR");
    break;
  case NUMBER:
    printf("NUMBER");
    break;
  default:
    printf("UNKNOWN");
    break;
  }
  printf("\x1b[0m");
  printf(", row: %d, col: %d", t->row, t->col);
  printf("}");
  if (br)
    printf("\n");
}

TokenType classify_keyword(char *lex) {
  if (strcmp(lex, "int") == 0 || strcmp(lex, "float") == 0 ||
      strcmp(lex, "char") == 0 || strcmp(lex, "double") == 0 ||
      strcmp(lex, "void") == 0) {
    return KEYWORD_TYPE;
  }
  if (strcmp(lex, "return") == 0)
    return KEYWORD_RETURN;
  if (strcmp(lex, "if") == 0)
    return KEYWORD_IF;
  if (strcmp(lex, "else") == 0)
    return KEYWORD_ELSE;
  if (strcmp(lex, "while") == 0)
    return KEYWORD_WHILE;
  if (strcmp(lex, "for") == 0)
    return KEYWORD_FOR;
  if (strcmp(lex, "break") == 0)
    return KEYWORD_BREAK;
  if (strcmp(lex, "continue") == 0)
    return KEYWORD_CONTINUE;
  if (strcmp(lex, "struct") == 0)
    return KEYWORD_STRUCT;
  return IDENTIFIER;
}

TokenType classify_operator(char *lex) {
  if (strcmp(lex, "+=") == 0 || strcmp(lex, "-=") == 0 ||
      strcmp(lex, "*=") == 0 || strcmp(lex, "/=") == 0 ||
      strcmp(lex, "=") == 0) {
    return OPERATOR_ASSIGNMENT;
  }

  if (strcmp(lex, "+") == 0 || strcmp(lex, "-") == 0 ||
      strcmp(lex, "*") == 0 || strcmp(lex, "/") == 0) {
    return OPERATOR_ARITMETIC;
  }

  if (strcmp(lex, "!") == 0 || strcmp(lex, "==") == 0 ||
      strcmp(lex, "!=") == 0) {
    return OPERATOR_LOGIC;
  }

  return OPERATOR;
}

TokenType classify_token(char *lex, ReadingState state) {
  switch (state) {
  case STATE_COMMENT_LINE:
  case STATE_COMMENT_BLOCK:
  case STATE_COMMENT_BLOCK_ASTERISK:
  case STATE_COMMENT_BLOCK_END:
    return COMMENT;

  case STATE_SLASH:
  case STATE_OPERATOR:
  case STATE_PLUS:
  case STATE_MINUS:
  case STATE_TIMES:
  case STATE_ASSIGN:
  case STATE_EXCLAMATION_MARK:
    return classify_operator(lex);
  case STATE_IDENTIFIER:
    return classify_keyword(lex);
  case STATE_SEPARATOR:
    return SEPARATOR;
  case STATE_FLOAT_NUMBER:
  case STATE_NUMBER:
    return NUMBER;
  case STATE_CLOSED_SIMPLE_LITERAL:
  case STATE_CLOSED_LITERAL:
    return LITERAL;
  default:
    break;
  }
  return UNKNOWN;
}

int is_separator_start(int c) {
  switch (c) {
  case '(':
  case ')':
  case '{':
  case '}':
  case '[':
  case ']':
  case ';':
  case ',':
  case '.':
    // case ':': ternary (later)
    // case '?':ternary (later)
    return 1;

  case '-':
    return 1;

  default:
    return 0;
  }
}

// Char groups
int is_letter(int c) { return isalpha(c) || c == '_'; }

int is_digit(int c) { return isdigit(c); }

int is_whitespace(int c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int is_identifier_start(int c) { return is_letter(c); }

int is_identifier_part(int c) { return is_letter(c) || is_digit(c); }

int is_operator_char(int c) { return strchr("+-*/=%!<>|&^~", c) != NULL; }

int is_separator_char(int c) { return strchr("(){}[];,:", c) != NULL; }

int is_slash(int c) { return c == '/'; }

int is_star(int c) { return c == '*'; }

int is_quote(int c) { return c == '"'; }

int is_newline(int c) { return c == '\n'; }

int is_token_start(int c) {
  return is_identifier_start(c) || is_digit(c) || is_quote(c) ||
         is_operator_char(c) || is_separator_char(c);
}

Token *read_next_token(Pointer *p, int prt) {
  p->token = NULL;
  while (p->token == NULL) {
    advance_pointer(p);
    // printf("%d",p->token);
  }
  if (prt) {
    prt_token(p->token, 1);
  }
  Token *tk = p->token;
  p->token = NULL;
  return tk;
}

void update_pivot(Pointer *p) {
  p->pivot_col = p->col;
  p->pivot_row = p->row;
  p->pivot_pos = p->curr_pos - 1;
}

void set_pointer_state(Pointer *p, ReadingState state) {
  p->reading_state = state;
  update_pivot(p);
}

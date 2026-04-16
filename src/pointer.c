#include "pointer.h"

static const char *reading_state_to_str(ReadingState s) {
  switch (s) {
    case STATE_INITIAL: return "STATE_INITIAL";
    case STATE_SLASH: return "STATE_SLASH";
    case STATE_COMMENT_LINE: return "STATE_COMMENT_LINE";
    case STATE_COMMENT_BLOCK: return "STATE_COMMENT_BLOCK";
    case STATE_COMMENT_BLOCK_END: return "STATE_COMMENT_BLOCK_END";
    case STATE_OPERATOR: return "STATE_OPERATOR";
    case STATE_DONE: return "STATE_DONE";
    case STATE_ERROR: return "STATE_ERROR";
    case STATE_SEPARATOR: return "STATE_SEPARATOR";
    default: return "INVALID";
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

int init_pointer(char * filename, Pointer *p) {
  
  p->stream = fopen(filename, "rb");
  if (p->stream == NULL) {
    return 0;
  }
  p->col = 0;
  p->row = 1;
  p->reading_state = STATE_INITIAL;
  p->curr_pos = ftell(p->stream);
  p->pivot_col = 0  ;
  p->pivot_row = 0;
  p->pivot_pos = ftell(p->stream);
  p->value = -1;
  p->token = NULL;
  
  return 1;
}

void update_pointer_position(Pointer *p)
{
  if (p->value == '\n')
  {

    p->col = 0;
    p->row += 1;
  }
  else
  {
    p->col += 1;
  }
  p->curr_pos = ftell(p->stream);
}

void update_pointer_state(Pointer *p)
{
  int c = p->value;
  if (p == NULL)
    return;
  if (p->reading_state == STATE_INITIAL)
  {
    ReadingState tt = detect_start_state(p->value);  
    set_pointer_state(p, tt);

    //After change from NONE to any reading state, must set the pivot position as current position
    //pointer must update the pos
    p->pivot_col = p->col;
    p->pivot_row = p->row;
    p->pivot_pos = p->curr_pos - 1;
  }
  else if (p->reading_state == STATE_SLASH && p->value == '/') {
    p->reading_state = STATE_COMMENT_LINE;
  } 


  //Transition rules
  else if (p->reading_state == STATE_COMMENT_LINE && p->value == '\n') {
    p->token = get_token_from_pointer(p);
    p->reading_state = STATE_INITIAL;
  } else if(p->reading_state == STATE_ERROR) {
    if ((is_separator_start(c))) {
      p->token = get_token_from_pointer(p);
      p->reading_state = STATE_SEPARATOR;
      update_pivot(p);
    } else if (c == ' ' || c == '\n' || c == '\t') {
      p->token = get_token_from_pointer(p);
      p->reading_state = STATE_INITIAL;
    }
  } else if (p->reading_state == STATE_IDENTIFIER ) {
    if ((is_separator_start(c))) {
      p->token = get_token_from_pointer(p);
      p->reading_state = STATE_SEPARATOR;
      update_pivot(p);
    } else if (c == ' ' || c == '\n' || c == '\t') {
      p->token = get_token_from_pointer(p);
      p->reading_state = STATE_INITIAL;
    }
  } else if (p->reading_state == STATE_SEPARATOR) {
    if (c == ';') {
      printf("DSA\n");
    }
    p->token = get_token_from_pointer(p);
    p->reading_state = detect_start_state(p->value);
    update_pivot(p);
  } else if (p->reading_state == STATE_OPERATOR) {
    p->token = get_token_from_pointer(p);
    p->reading_state = detect_start_state(p->value);
    update_pivot(p);   
  } else if (p->reading_state == STATE_NUMBER) {
    if (c == '.') {
      p->reading_state = STATE_FLOAT_NUMBER_WITHOUT_DECIMAL;
    } else if ((is_separator_char(c) && c != '.') || c == ' ' || c == '\t' || c== '\n' || is_operator_char(c)) {
      p->token = get_token_from_pointer(p);
      p->reading_state = detect_start_state(p->value);
      update_pivot(p);
    } else {
      if (!is_digit(c)) {
        // p->token = get_token_from_pointer(p);
        p->reading_state = STATE_ERROR;
        // update_pivot(p);
      }
    }
  } else if(p->reading_state == STATE_FLOAT_NUMBER_WITHOUT_DECIMAL) {
    if (is_digit(c)) {
      p->reading_state = STATE_FLOAT_NUMBER;
    } else {
      p->reading_state = STATE_ERROR;
    }
  } else if (p->reading_state == STATE_FLOAT_NUMBER) {
    if (c == '.') {
      p->reading_state = STATE_ERROR_NUMBER_MANY_DOTS;
    } else if ((is_separator_char(c) && c != '.') || c == ' ' || c == '\t' || c== '\n' || is_operator_char(c)) {
      p->token = get_token_from_pointer(p);
      p->reading_state = detect_start_state(p->value);
      update_pivot(p);
    } else {
      if (!is_digit(c)) {
        // p->token = get_token_from_pointer(p);
        p->reading_state = STATE_ERROR;
        // update_pivot(p);
      }
    } 
  } else if (p->reading_state == STATE_ERROR_NUMBER_MANY_DOTS) {
    if (((is_separator_start(c) && c != '.'))) {
      p->token = get_token_from_pointer(p);
      p->reading_state = STATE_SEPARATOR;
      update_pivot(p);
    } else if (c == ' ' || c == '\n' || c == '\t') {
      p->token = get_token_from_pointer(p);
      p->reading_state = STATE_INITIAL;
    }
  } else if (p->reading_state == STATE_OPEN_LITERAL) {
    if (c == '\"') {
      p->reading_state = STATE_CLOSED_LITERAL;
    } else if (c == '\n') {
      p->reading_state = STATE_ERROR;
      p->token = get_token_from_pointer(p);
      p->reading_state = STATE_INITIAL;
      update_pivot(p);
    }
  } else if(p->reading_state == STATE_CLOSED_LITERAL) {
    p->token = get_token_from_pointer(p);
    p->reading_state = detect_start_state(p->value);
    update_pivot(p);
  } else if (p->reading_state == STATE_OPEN_SIMPLE_LITERAL) {
    if (c == '\'') {
      p->reading_state = STATE_CLOSED_SIMPLE_LITERAL;
    } else if (c == '\n') {
      p->reading_state = STATE_ERROR;
      p->token = get_token_from_pointer(p);
      p->reading_state = STATE_INITIAL;
      update_pivot(p);
    }
  } else if(p->reading_state == STATE_CLOSED_SIMPLE_LITERAL) {
    p->token = get_token_from_pointer(p);
    p->reading_state = detect_start_state(p->value);
    update_pivot(p);
  }


}

Token * get_token_from_pointer(Pointer *p) {
  if (!p || !p->stream) return NULL;
  
  long start = p->pivot_pos;
  long end = p->curr_pos-1;
  // printf("POSITION BEFORE READING TOKEN %d\n", ftell(p->stream));
  
  if (end < start) return NULL;
  long size = end - start;
  
  Token *token = (Token *) malloc(sizeof(Token));
  if (!token) return NULL;
  token->lex = (char *) malloc(size + 1);
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
  int c  = fgetc(p->stream);
  // printf("TOKEN [%ld,%ld): '%s' // LAST READ VALUE: %c\n", start, end, token->lex, p->value);
  // printf("c: %c val: %c\n", c, p->value);
  return token;
}

int advance_pointer(Pointer *p)
{
  int c = fgetc(p->stream);
  p->value = c;
  if (c == EOF)
    return 0;
  update_pointer_position(p);
  update_pointer_state(p);
}


ReadingState detect_start_state(int c) {

  if (c == '/' )
    return STATE_SLASH;

  if (c == '\n' || c == ' ' || c == '\t')   
    return STATE_INITIAL;

  if (is_separator_char(c)) return STATE_SEPARATOR;

  if (is_letter(c)) return STATE_IDENTIFIER;

  if (is_digit(c)) return STATE_NUMBER;

  if (is_operator_char(c)) return STATE_OPERATOR;
  
  if (c == '\'') return STATE_OPEN_SIMPLE_LITERAL;

  if (c == '\"') return STATE_OPEN_LITERAL;

  return STATE_ERROR;
}


void prt_token(Token *t, int br)
{
  printf("{lex: %s, ", t->lex);
  switch (t->type)
  {
  case KEYWORD:
    printf("type: KEYWORD");
    break;
  case IDENTIFIER:
    printf("type: IDENTIFIER");
    break;
  case LITERAL:
    printf("type: LITERAL");
    break;
  case COMMENT:
    printf("type: COMMENT");
    break;
  case OPERATOR:
    printf("type: OPERATOR");
    break;
  case SEPARATOR:
    printf("type: SEPARATOR");
    break;
  case NUMBER:
    printf("type: NUMBER");
    break;
  default:
    printf("type: UNKNOWN");
    break;
  }
  printf(", row: %d, col: %d", t->row, t->col);
  printf("}");
  if (br)
    printf("\n");
}

TokenType classify_token(char * lex, ReadingState state) {
  switch (state) {
    case STATE_COMMENT_LINE:
    case STATE_COMMENT_BLOCK:
      return COMMENT;

    case STATE_SLASH:
    case STATE_OPERATOR:
      return OPERATOR;
    case STATE_IDENTIFIER:
      return IDENTIFIER;
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


int is_separator_start(int c)
{
  switch (c)
  {
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

//Char groups
int is_letter(int c) {
  return isalpha(c) || c == '_';
}

int is_digit(int c) {
  return isdigit(c);
}

int is_whitespace(int c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int is_identifier_start(int c) {
  return is_letter(c);
}

int is_identifier_part(int c) {
  return is_letter(c) || is_digit(c);
}

int is_operator_char(int c) {
  return strchr("+-*/=%!<>|&^~", c) != NULL;
}

int is_separator_char(int c) {
  return strchr("(){}[];,:", c) != NULL;
}

int is_slash(int c) {
  return c == '/';
}

int is_star(int c) {
  return c == '*';
}

int is_quote(int c) {
  return c == '"';
}

int is_newline(int c) {
  return c == '\n';
}

int is_token_start(int c) {
  return is_identifier_start(c) || is_digit(c) || is_quote(c) || is_operator_char(c) || is_separator_char(c);
}

Token * read_next_token(Pointer *p, int prt) {
  p->token = NULL;
  while (p->token == NULL) {
    advance_pointer(p);
    // printf("%d",p->token);
  }
  if (prt) {
    prt_token(p->token, 1);
  }
  Token * tk = p->token;
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
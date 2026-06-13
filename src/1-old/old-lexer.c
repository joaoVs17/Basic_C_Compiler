#include "lexer.h"

int is_valid_token_start(int c)
{
  return c == '_' || c == '+' || c == '-' || c == '/' || c == '%';
}

void update_pointer_position(Pointer *p)
{
  if (p->value == '\n')
  {
    p->col = 1;
    p->row += 1;
  }
  else
  {
    p->col += 1;
  }
}

void update_pointer_state(Pointer *p)
{
  if (p == NULL)
    return;
  if (p->reading_state == NONE)
  {
  }
}

int advance_pointer(Pointer *p)
{
  p->value = fgetc(p->stream);
  if (p->value == EOF)
    return 0;
  update_pointer_position(p);
  update_pointer_state(p);
}

int advance_pointer_n(Pointer *p, int n)
{
  int res = 1;
  for (int i = 0; i < n; i++)
  {
    if (!advance_pointer(p))
    {
      res = 0;
      break;
    }
  }
  return res;
}
int change_pointer_state(Pointer *p, TokenType state)
{
  if (p == NULL)
    return 0;
  p->reading_state = state;
  return 1;
}

int read_next_token(Pointer *p)
{
  if (p == NULL)
    return 0;
  if (p->reading_state != NONE)
    return 0;

  while (!is_valid_token_start(p->value))
  {
    advance_pointer(p);
  }
}

Token *readliteral(FILE *file, int *colc, int *rowc);
Token *readcomment(FILE *file, int *colc, int *rowc);
Token *readkeyword_or_identifier(FILE *file, int *colc, int *rowc);
Token *create_token(TokenType type, char *lex, int row, int col);
Token *readseparator(FILE *file, int *colc, int *rowc);
Token *readoperator(FILE *file, int *colc, int *rowc);
Token *readnumber(FILE *file, int *colc, int *rowc);

void prt_token(Token *t, int br);
int iskeyword(char *str);
char *read_file(FILE *file, int start_pos, int char_no);
int is_operator_start(int cc);
int is_separator_start(char c);
int peek(FILE *file);
void increment_counters(int *colc, int *rowc, int cc);



int is_operator_start(int cc)
{
  switch (cc)
  {
  case '+':
  case '-':
  case '*':
  case '/':
  case '%':
  case '=':
  case '!':
  case '<':
  case '>':
  case '&':
  case '|':
  case '^':
  case '~':
    return 1;

  default:
    return 0;
  }
}

int isliteralend(FILE *file)
{
  long pos = ftell(file);

  int count = 0;
  int cc;
  while (ftell(file) > 0)
  {
    cc = fgetc(file);

    if (cc == '\\')
    {
      count++;
      fseek(file, -2, SEEK_CUR);
    }
    else
    {
      break;
    }
  }
  fseek(file, pos, SEEK_SET);
  return (count % 2 == 0);
}

Token *readliteral(FILE *file, int *colc, int *rowc)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;
  int unclosed = 0;
  int col = *colc;
  int row = *rowc;
  while (1)
  {
    cc = fgetc(file);
    rdcnt += 1;
    if ((cc == pc && isliteralend(file)))
      break;
    if (cc == EOF || cc == '\n')
    {
      increment_counters(colc, rowc, cc);
      unclosed = 1;
      break;
    }
    increment_counters(colc, rowc, cc);
  }
  char *str = read_file(file, spos, rdcnt);
  Token *t;
  if (unclosed)
  {
    t = create_token(UNKNOWN, str, row, col);
  }
  else
  {
    t = create_token(LITERAL, str, row, col);
  }
  free(str);
  return t;
}

Token *readcomment(FILE *file, int *colc, int *rowc)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;
  int col = *colc;
  int row = *rowc;
  cc = fgetc(file);
  increment_counters(colc, rowc, cc);
  if (cc == '/')
  {
    while (1)
    {
      cc = fgetc(file);
      rdcnt += 1;
      if (cc == '\n' || cc == EOF)
      {
        break;
      }
      increment_counters(colc, rowc, cc);
    }
  }
  else if (cc == '*')
  {
    while (1)
    {
      cc = fgetc(file);
      rdcnt += 1;
      if (cc == '*')
      {
        if (peek(file) == '/' || cc == EOF)
        {
          rdcnt += 2;
          break;
        }
      }
    }
    increment_counters(colc, rowc, cc);
  }
  Token *t;
  char *str = read_file(file, spos, rdcnt);
  t = create_token(COMMENT, str, row, col);
  free(str);
  return t;
}

Token *readkeyword_or_identifier(FILE *file, int *colc, int *rowc)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;
  int col = *colc;
  int row = *rowc;
  int has_invalid_char = !((cc >= 'a' && cc <= 'z') || (cc >= 'A' && cc <= 'Z') || cc == '_');
  while (1)
  {
    cc = fgetc(file);
    rdcnt += 1;

    if ((is_separator_start(cc)) || is_operator_start(cc) || cc == ' ' || cc == '\n' || cc == '/')
    {
      increment_counters(colc, rowc, cc);
      break;
    }
    if (!((cc >= 'a' && cc <= 'z') || (cc >= 'A' && cc <= 'Z') || (cc >= '0' && cc <= '9') || cc == '_'))
    {
      has_invalid_char += 1;
    }
    increment_counters(colc, rowc, cc);
  }
  char *str = read_file(file, spos, rdcnt - 1);
  Token *t;
  if (has_invalid_char)
  {
    t = create_token(UNKNOWN, str, row, col);
    free(str);
    return t;
  }
  if (iskeyword(str))
  {
    t = create_token(KEYWORD, str, row, col);
  }
  else
  {
    t = create_token(IDENTIFIER, str, row, col);
  }
  free(str);
  return t;
}

Token *readoperator(FILE *file, int *colc, int *rowc)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;
  int col = *colc;
  int row = *rowc;
  int next = peek(file);
  int c1 = ((cc == '+' || cc == '-') && next == cc);
  int c2 = ((cc == '+' || cc == '-' || cc == '*' || cc == '/' || cc == '=' || cc == '!' || cc == '%') && next == '=');
  int c3 = ((cc == '<' || cc == '>') && (next == '=' || next == cc));
  int c4 = ((cc == '&' || cc == '|') && next == cc);

  increment_counters(colc, rowc, cc);
  if (c1 || c2 || c3 || c4)
  {
    rdcnt = 2;
    increment_counters(colc, rowc, cc);
  }

  Token *t;
  char *str = read_file(file, spos, rdcnt);
  t = create_token(OPERATOR, str, row, col);

  return t;
}

Token *readseparator(FILE *file, int *colc, int *rowc)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;

  Token *t;
  int col = *colc;
  int row = *rowc;

  increment_counters(colc, rowc, cc);
  if (cc == '-')
  {
    rdcnt = 2;
    increment_counters(colc, rowc, cc);
  };
  char *str = read_file(file, spos, rdcnt);
  t = create_token(SEPARATOR, str, row, col);
  free(str);
  return t;
}

Token *readnumber(FILE *file, int *colc, int *rowc)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;
  int dotc = 0;
  int hasinvalidchar = 0;
  int col = *colc;
  int row = *rowc;
  while (1)
  {

    cc = fgetc(file);
    rdcnt += 1;
    if (cc == '.')
      dotc += 1;
    if ((is_separator_start(cc) && cc != '.') || cc == ' ')
    {
      break;
    }
    if (!(cc >= '0' && cc <= '9') && cc != '.')
      hasinvalidchar = 1;
    increment_counters(colc, rowc, cc);
  }

  Token *t;
  char *str = read_file(file, spos, rdcnt - 1);
  if (dotc > 1 || hasinvalidchar || str[rdcnt - 2] == '.')
  {
    t = create_token(UNKNOWN, str, row, col);
  }
  else
  {
    t = create_token(NUMBER, str, row, col);
  }

  free(str);
  return t;
}

int cmpstr(char *s1, char *s2)
{
  return strcmp(s1, s2);
}

int iskeyword(char *str)
{
  const char *keywords[] = {
      "int", "float", "if", "string", "bool", "for", "return", "char"};
  int n = sizeof(keywords) / sizeof(keywords[0]);

  for (int i = 0; i < n; i++)
  {
    if (strcmp(str, keywords[i]) == 0)
    {
      return 1;
    }
  }

  return 0;
}

int is_separator_start(char c)
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
int peek(FILE *file)
{
  char c = fgetc(file);
  fseek(file, -1, SEEK_CUR);
  return c;
}

char *read_file(FILE *file, int start_pos, int char_no)
{
  char *str = (char *)malloc(sizeof(char) * char_no + 1);
  fseek(file, start_pos, SEEK_SET);
  fread(str, sizeof(char), char_no, file);
  str[char_no] = '\0';
  return str;
}

Token *create_token(TokenType type, char *lex, int row, int col)
{
  Token *t = (Token *)malloc(sizeof(Token));
  t->type = type;
  t->row = row;
  t->col = col;
  t->lex = (char *)malloc(strlen(lex) + 1);
  strcpy(t->lex, lex);
  return t;
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

void increment_counters(int *colc, int *rowc, int cc)
{
  if (cc == '\n')
  {
    *rowc += 1;
    *colc = 0;
  }
  else
  {
    *colc += 1;
  }
}

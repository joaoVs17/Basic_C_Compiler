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
} TokenType;

typedef struct
{
  TokenType type;
  char *lex;
} Token;

Token *readliteral(FILE *file, int colc, int rowc);
Token *readcomment(FILE *file);
Token *readkeyword_or_identifier(FILE *file);
Token *create_token(TokenType type, char *lex);
Token *readseparator(FILE *file);
Token *readoperator(FILE *file);
Token *readnumber(FILE *file);

void prt_token(Token *t, int br);
int iskeyword(char *str);
char *read_file(FILE *file, int start_pos, int char_no);
int is_operator_start(int cc);
int is_separator_start(char c);
int peek(FILE *file);

int main()
{
  FILE *file;
  file = fopen("lexer-test2.c", "r");
  int i = 3;
  if (file == NULL)
    return 1;

  int cc;
  int colc = 1, rowc = 1;

  do
  {
    cc = fgetc(file);
    colc += 1;
    if (cc == '\n')
    {
      colc = 1;
      rowc += 1;
    };
    if (cc == ' ' || cc == '\t' || cc == '\n' || cc == EOF)
      continue;
    if (cc == '\"' || cc == '\'')
    {
      prt_token(readliteral(file, colc, rowc), 1);
    }
    else if (cc == '/')
    {
      int temp = peek(file);
      if (temp == '/' || temp == '*')
        prt_token(readcomment(file), 1);
      else
      {
        prt_token(readoperator(file), 1);
      }
    }
    // else if ((cc >= 'a' && cc <= 'z') || (cc >= 'A' && cc <= 'Z') || cc == '_')
    // {
    //   prt_token(readkeyword_or_identifier(file), 1);
    // }
    else if (is_separator_start(cc))
    {
      if (cc == '-' && peek(file) != '>')
      {
        prt_token(readoperator(file), 1);
      }
      else
      {
        prt_token(readseparator(file), 1);
      }
    }
    else if (is_operator_start(cc))
    {
      prt_token(readoperator(file), 1);
    }
    else if (cc >= '0' && cc <= '9')
    {
      prt_token(readnumber(file), 1);
    }
    else
    {
      prt_token(readkeyword_or_identifier(file), 1);
    }

  } while (cc != EOF);

  fclose(file);
  return 0;
}

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

Token *readliteral(FILE *file, int colc, int rowc)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;
  int unclosed = 0;
  while (1)
  {
    cc = fgetc(file);
    rdcnt += 1;
    if ((cc == pc && isliteralend(file)))
      break;
    if (cc == EOF || cc == '\n')
    {
      unclosed = 1;
      break;
    }
  }
  char *str = read_file(file, spos, rdcnt);
  Token *t;
  if (unclosed)
  {
    t = create_token(UNKNOWN, str);
  }
  else
  {
    t = create_token(LITERAL, str);
  }
  free(str);
  return t;
}

Token *readcomment(FILE *file)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;

  cc = fgetc(file);
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
  }
  Token *t;
  char *str = read_file(file, spos, rdcnt);
  t = create_token(COMMENT, str);
  free(str);
  return t;
}

Token *readkeyword_or_identifier(FILE *file)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;
  int has_invalid_char = !((cc >= 'a' && cc <= 'z') || (cc >= 'A' && cc <= 'Z') || cc == '_');
  while (1)
  {
    cc = fgetc(file);
    rdcnt += 1;

    if (is_separator_start(cc) || cc == ' ' || cc == '\n' || cc == '/')
    {
      break;
    }
    if (!((cc >= 'a' && cc <= 'z') || (cc >= 'A' && cc <= 'Z') || (cc >= '0' && cc <= '9') || cc == '_'))
    {
      has_invalid_char += 1;
    }
  }
  char *str = read_file(file, spos, rdcnt - 1);
  Token *t;
  if (has_invalid_char)
  {
    t = create_token(UNKNOWN, str);
    free(str);
    return t;
  }
  if (iskeyword(str))
  {
    t = create_token(KEYWORD, str);
  }
  else
  {
    t = create_token(IDENTIFIER, str);
  }
  free(str);
  return t;
}

Token *readoperator(FILE *file)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;

  int next = peek(file);
  int c1 = ((cc == '+' || cc == '-') && next == cc);
  int c2 = ((cc == '+' || cc == '-' || cc == '*' || cc == '/' || cc == '=' || cc == '!' || cc == '%') && next == '=');
  int c3 = ((cc == '<' || cc == '>') && (next == '=' || next == cc));
  int c4 = ((cc == '&' || cc == '|') && next == cc);

  if (c1 || c2 || c3 || c4)
    rdcnt = 2;

  Token *t;
  char *str = read_file(file, spos, rdcnt);
  t = create_token(OPERATOR, str);

  return t;
}

Token *readseparator(FILE *file)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;

  Token *t;

  if (cc == '-')
  {
    rdcnt = 2;
  };
  char *str = read_file(file, spos, rdcnt);
  t = create_token(SEPARATOR, str);
  free(str);
  return t;
}

Token *readnumber(FILE *file)
{
  fseek(file, -1, SEEK_CUR);
  long spos = ftell(file);
  int cc = fgetc(file);
  int pc = cc;
  int rdcnt = 1;
  int dotc = 0;
  int hasinvalidchar = 0;
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
  }

  Token *t;
  char *str = read_file(file, spos, rdcnt - 1);
  if (dotc > 1 || hasinvalidchar || str[rdcnt - 2] == '.')
  {
    t = create_token(UNKNOWN, str);
  }
  else
  {
    t = create_token(NUMBER, str);
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

Token *create_token(TokenType type, char *lex)
{
  Token *t = (Token *)malloc(sizeof(Token));
  t->type = type;
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
  printf("}");
  if (br)
    printf("\n");
}

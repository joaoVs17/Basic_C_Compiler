#include <stdio.h>

int main()
{
  int x = 10;         // declaração simples
  float y = 3.14;     // número decimal
  string s = "Hello"; // string literal
  char c = 'A';       // char literal

  // Comentário de linha
  /* Comentário
     de múltiplas
     linhas */

  x += 5;   // operador composto
  y--;      // operador de decremento
  z->field; // operador ->

  if (x > 5 && y < 10)
  {
    return x + y;
  }

  // Casos de erro léxico
  int 123abc; // identificador inválido (começa com número)
  string s2 = "Unclosed string;  // string não fechada
      float f1 = 4.5.6; // número inválido (dois pontos)
  $illegal = 10;        // identificador ilegal com caractere $
}
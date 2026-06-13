int;
/*dsa*/
// sexo
// #include <stdio.h>
// #include <stdbool.h>

// #define MAX 10
->int soma(int a, int b)
{
  return a + b;
}

int main()
{
  int x = 42;
  float y = 3.14;
  bool flag = true;
  char c = 'Z';
  char str[] = "Teste de lexer!";

  for (int i = 0; i < MAX; i++)
  {
    x += i * 2;
    y = y / (i + 1);

    if (x % 2 == 0 && flag)
    {
      printf("Par: %d\n", x);
    }
    else
    {
      printf("Impar: %d\n", x);
    }
  }

  // operadores variados
  x++;
  y -= 1.5;
  flag = !flag;

  // expressão estranha
  int z = (x > 10) ? x : -x;

  printf("Resultado: %d, %f, %d\n", z, y, flag);
  1.1.1 return 0;
}
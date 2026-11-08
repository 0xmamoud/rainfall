#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  int id;
  char *name;
} account;

char c[68];

void m(void) {
  printf("%s - %d\n", c, time(NULL));
}

int main(int argc, char **argv) {
  account *a = malloc(8);
  a->id = 1;
  a->name = malloc(8);

  account *b = malloc(8);
  b->id = 2;
  b->name = malloc(8);

  strcpy(a->name, argv[1]);
  strcpy(b->name, argv[2]);

  FILE *f = fopen("/home/user/level8/.pass", "r");
  fgets(c, 68, f);
  puts("~~");
  return (0);
}

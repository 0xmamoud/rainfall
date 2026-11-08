#include <stdio.h>
#include <string.h>
#include <unistd.h>

void p(char *dest, char *sep) {
  char buf[4104];

  puts(sep);
  read(0, buf, 4096);
  *strchr(buf, '\n') = 0;
  strncpy(dest, buf, 20);
}

void pp(char *dest) {
  char a[20];
  char b[20];

  p(a, " - ");
  p(b, " - ");
  strcpy(dest, a);
  strcat(dest, " ");
  strcat(dest, b);
}

int main(void) {
  char dest[54];

  pp(dest);
  puts(dest);
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void n(void) {
  system("/bin/cat /home/user/level7/.pass");
}

void m(void) {
  puts("Nope");
}

int main(int argc, char **argv) {
  char *buffer;
  void (**funcptr)(void);

  buffer = malloc(64);
  funcptr = malloc(4);
  *funcptr = m;
  strcpy(buffer, argv[1]);
  (*funcptr)();
  return (0);
}

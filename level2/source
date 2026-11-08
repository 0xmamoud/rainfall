#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void p(void) {
  char buffer[76];
  void *ret;

  fflush(stdout);
  gets(buffer);
  ret = __builtin_return_address(0);
  if (((unsigned int)ret & 0xb0000000) == 0xb0000000) {
    printf("(%p)\n", ret);
    _exit(1);
  }
  puts(buffer);
  strdup(buffer);
}

int main(void) {
  p();
  return (0);
}

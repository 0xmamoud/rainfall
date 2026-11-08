#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  char buffer[132];
  FILE *f;

  f = fopen("/home/user/end/.pass", "r");
  if (f == NULL || argc != 2)
    return -1;

  fread(buffer, 1, 66, f);
  buffer[65] = 0;
  buffer[atoi(argv[1])] = 0;
  fread(buffer + 66, 1, 65, f);
  fclose(f);

  if (strcmp(buffer, argv[1]) == 0)
    execl("/bin/sh", "sh", NULL);
  else
    puts(buffer + 66);
  return 0;
}

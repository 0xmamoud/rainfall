#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *auth = NULL;
char *service = NULL;

int main(void) {
  char line[128];

  while (1) {
    printf("%p, %p \n", auth, service);
    if (fgets(line, 128, stdin) == NULL)
      break;

    if (strncmp(line, "auth ", 5) == 0) {
      auth = malloc(4);
      auth[0] = 0;
      if (strlen(line + 5) <= 30)
        strcpy(auth, line + 5);
    }
    if (strncmp(line, "reset", 5) == 0)
      free(auth);
    if (strncmp(line, "service", 6) == 0)
      service = strdup(line + 7);
    if (strncmp(line, "login", 5) == 0) {
      if (auth[32] != 0)
        system("/bin/sh");
      else
        fwrite("Password:\n", 1, 10, stdout);
    }
  }
  return (0);
}

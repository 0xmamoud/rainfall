#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int language = 0;

void greetuser(char *name) {
  char greeting[72];

  switch (language) {
  case 1:
    strcpy(greeting, "Hyv\xc3\xa4\xc3\xa4 p\xc3\xa4iv\xc3\xa4\xc3\xa4 ");
    break;
  case 2:
    strcpy(greeting, "Goedemiddag! ");
    break;
  default:
    strcpy(greeting, "Hello ");
    break;
  }
  strcat(greeting, name);
  puts(greeting);
}

int main(int argc, char **argv) {
  char buffer[76];
  char *lang;

  if (argc != 3)
    return 1;

  memset(buffer, 0, sizeof(buffer));
  strncpy(buffer, argv[1], 40);
  strncpy(buffer + 40, argv[2], 32);

  lang = getenv("LANG");
  if (lang != NULL) {
    if (memcmp(lang, "fi", 2) == 0)
      language = 1;
    else if (memcmp(lang, "nl", 2) == 0)
      language = 2;
  }

  greetuser(buffer);
  return 1;
}

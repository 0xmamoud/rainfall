#include <cstring>

class N {
  char annotation[100];
  int number;

public:
  N(int n) { number = n; }

  void setAnnotation(char *s) {
    memcpy(annotation, s, strlen(s));
  }

  virtual int operator+(N &r) { return number + r.number; }
  virtual int operator-(N &r) { return number - r.number; }
};

int main(int argc, char **argv) {
  if (argc <= 1)
    _exit(1);

  N *a = new N(5);
  N *b = new N(6);

  a->setAnnotation(argv[1]);
  return (*b + *a);
}

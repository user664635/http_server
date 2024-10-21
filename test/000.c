#include <assert.h>

extern int add(int, int);
int main(void) {
  for (int i = -100; i < 100; ++i)
    for (int j = -100; j < 100; ++j)
      if (i + j != add(i, j))
        return 1;
}

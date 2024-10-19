#include <assert.h>

extern int add(int, int);
int main(void) { assert(1 + 1 == add(1, 1)); }

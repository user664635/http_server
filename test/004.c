#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 256
static unsigned char random[] = {
#embed "../random" limit(N + N * 4 * 256)
};
int find_median(int *arr, int n);
static int cmp(const void *a, const void *b) {
  int x = *(int *)a, y = *(int *)b;
  if (x == y)
    return 0;
  if (x > y)
    return 1;
  return -1;
}
static inline int _find_median(int *arr, int n) {
  qsort(arr, n, 4, cmp);
  return arr[n >> 1];
}
int main(void) {
  int *arrs = (int *)(random + N);
  int arrcopy[256], arrsrc[256];
  for (int i = 0; i < N; ++i) {
    int *arr = arrs + i * 256;
    int n = random[i] | 1;
    memcpy(arrcopy, arr, n * 4);
    memcpy(arrsrc, arr, n * 4);
    int corr_ans = _find_median(arr, n);
    int ans = find_median(arrcopy, n);
    if (corr_ans != ans) {

      printf("%d/%d\n", i, N);
      puts("src");
      for (int j = 0; j < n; ++j)
        printf("%d\n", arrsrc[j]);
      puts("sorted:");
      for (int j = 0; j < n; ++j)
        printf("%d\n", arr[j]);
      puts("yours:");
      for (int j = 0; j < n; ++j)
        printf("%d\n", arrcopy[j]);

      printf("correct:%d\n", corr_ans);
      printf("yours:%d\n", ans);
      return 1;
    }
  }
}

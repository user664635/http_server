#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 256
static unsigned char random[] = {
#embed "../random" limit(N + N * 4 * 256)
};
typedef struct {
  double avg;
  int median, major;
} ArrayMeta;
ArrayMeta calc_meta(int *arr, int n);

static int cmp(const void *a, const void *b) {
  int x = *(int *)a, y = *(int *)b;
  if (x == y)
    return 0;
  if (x > y)
    return 1;
  return -1;
}
static inline ArrayMeta _calc_meta(int *arr, int n) {
  qsort(arr, n, 4, cmp);
  ArrayMeta meta;
  for (int i = 0; i < n; ++i)
    meta.avg += arr[i];
}
int main(void) {
  int *arrs = (int *)(random + N);
  int arrcopy[256], arrsrc[256];
  for (int i = 0; i < N; ++i) {
    int *arr = arrs + i * 256;
    int n = random[i] | 1;
    memcpy(arrcopy, arr, n * 4);
    memcpy(arrsrc, arr, n * 4);
    ArrayMeta corr_ans = _calc_meta(arr, n);
    ArrayMeta ans = calc_meta(arrcopy, n);
    if (corr_ans.avg != ans.avg) {

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

      printf("correct:%f\n", corr_ans.avg);
      printf("yours:%f\n", ans.avg);
      return 1;
    }
  }
}

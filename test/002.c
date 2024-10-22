#include <stdio.h>

#define N 256
static unsigned char random[] = {
#embed "../random" limit(N + N * 4 * 256)
};

static inline double _avg(int *arr, int n) {
  double sum = 0;
  for (int i = 0; i < n; ++i)
    sum += arr[n];
  return sum / n;
}
double avg(int *arr, int n);
int main(void) {
  int *arrs = (int *)(random + N);
  for (int i = 0; i < N; ++i) {
    int *arr = arrs + i * 256;
    int n = random[i] + 1;
    double corr_ans = _avg(arr + i * 256, n);
    double ans = avg(arr + i * 256, n);
    if (__builtin_elementwise_abs(corr_ans - ans) > 1e-10) {
      printf("%d/%d\n", i, N);
      printf("correct:%.20f\n", corr_ans);
      printf("yours:%.20f\n", ans);
    }
  }
}

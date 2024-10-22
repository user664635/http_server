#include <stdio.h>
#define N 1024
static char random[] = {
#embed "../random" limit(N)
};

static inline void _float2hex(float num, char hex[9]) {
  int x = *(int *)&num;
  sprintf(hex, "%08X", x);
}
void float2hex(float num, char hex[9]);
int main(void) {
  float *num = (float *)random;
  char corr_ans[16], ans[16];
  for (int i = 0; i < N; ++i) {
    _float2hex(num[i], corr_ans);
    float2hex(num[i], ans);
    if (*(long *)corr_ans != *(long *)ans) {
      printf("%d/%d\n", i, N);
      printf("%.20e\n",num[i]);
      printf("correct:%s\n", corr_ans);
      printf("yours:%s\n", ans);
      return 1;
    }
  }
}

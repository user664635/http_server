#include <stdio.h>

static inline double _calc_triangle_area(double vert[6]) {
  long double dx1 = vert[2] - vert[0];
  long double dx2 = vert[4] - vert[0];
  long double dy1 = vert[3] - vert[1];
  long double dy2 = vert[5] - vert[1];
  return .5 * __builtin_elementwise_abs(dx1 * dy2 - dx2 * dy1);
}
double calc_triangle_area(double[6]);
#define N 1024
static char random[] = {
#embed "../random" limit(N * 6 * 8)
};

int main(void) {
  double(*verts)[6] = (double(*)[6])random;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < 6; ++j)
      verts[i][j] = 0x1p-56 * *(long *)(verts[i] + j);
    double corr_ans = _calc_triangle_area(verts[i]);
    double ans = calc_triangle_area(verts[i]);
    if (__builtin_elementwise_abs(ans - corr_ans) > 1e-10) {
      printf("%u/%u\n", i + 1, N);
      for (int j = 0; j < 6; ++j)
        printf("%f\n", verts[i][j]);
      printf("\ncorrect:\n%.20f\n", corr_ans);
      printf("your:\n%.20f\n", ans);
      return 1;
    }
  }
}

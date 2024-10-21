double calc_triangle_area(double vert[6]) {
  long double y0 = vert[3] - vert[5];
  long double y1 = vert[5] - vert[1];
  long double y2 = vert[1] - vert[3];
  return .5 *
         __builtin_elementwise_abs(vert[0] * y0 + vert[2] * y1 + vert[4] * y2);
}

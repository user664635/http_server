double calc_triangle_area(double vert[6]) {
  double dx1 = vert[2] - vert[0];
  double dx2 = vert[4] - vert[0];
  double dy1 = vert[3] - vert[1];
  double dy2 = vert[5] - vert[1];
  return .5 * __builtin_elementwise_abs(dx1 * dy2 - dx2 * dy1);
}

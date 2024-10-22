void float2hex(float num, char hex[9]) {
  unsigned int x = *(int *)&num;
  for (int i = 0; i < 8; ++i) {
    x = x << 4 | x >> 28;
    int y = x & 15;
    hex[i] = y > 9 ? y + '7' : y + '0';
  }
}

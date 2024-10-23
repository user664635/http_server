static inline void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}
static inline int partition(int *arr, int l, int h) {
  int pivot = arr[h];
  for (int j = l; j < h; ++j)
    if (arr[j] < pivot)
      swap(arr + j, arr + l++);
  swap(arr + l, arr + h);
  return l;
}
static inline int select(int *arr, int l, int h, int k) {
  int i = partition(arr, l, h);
  if (i == k)
    return arr[i];
  if (i < k)
    return select(arr, i + 1, h, k);
  return select(arr, l, i - 1, k);
}
int find_median(int *arr, int n) { return select(arr, 0, n - 1, n >> 1); }


typedef float __attribute((ext_vector_type(2))) vec2;
typedef struct {
  vec2 v0, v1, v2;
} Triangle;
typedef struct {
  vec2 cent, diag;
} Rectangle;
typedef struct {
  vec2 cent;
  float r;
} Circle;
typedef struct {
  enum { triangle, rectangle, circle } type;
  union {
    Triangle triangle;
    Rectangle rectangle;
    Circle circle;
  };
} Shape;

#define abs __builtin_elementwise_abs
double area(Shape shape) {
  switch (shape.type) {
  case triangle:
    vec2 v0 = shape.triangle.v0;
    vec2 v1 = shape.triangle.v1;
    vec2 v2 = shape.triangle.v2;
    double dx1 = v1.x - v0.x;
    double dx2 = v2.x - v0.x;
    double dy1 = v1.y - v0.y;
    double dy2 = v2.y - v0.y;
    return .5 * abs(dx1 * dy2 - dx2 * dy1);
  case rectangle:
    vec2 diag = shape.rectangle.diag - shape.rectangle.cent;
    return abs(diag.x * diag.y);
  case circle:
    double r = shape.circle.r;
    return 3.14 * r * r;
  }
}

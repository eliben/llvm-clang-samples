int gint;
float gfloat;

float** gppfloat;

float foo() {
  return gfloat + **gppfloat;
}

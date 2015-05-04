namespace detail {

struct Base {
};

constexpr int foo2(int i) {
  return 2 * i;
}

}



namespace ns {

//constexpr int foo2(int i) {
  //return 2*i;
//}

using detail::foo2;

constexpr float foo2(float f) {
  return 2.2+f;
}

float joe = foo2(45.4f);

namespace Sambusak {

constexpr double foo2(double d) {
  return d * 2.2;
}

}

}

int iii = ns::foo2(10);
float fla = ns::foo2(4.4f);
double dla = ns::Sambusak::foo2(4.4);

void bbbb() {
  auto func = ns::Sambusak::foo2;
  int k = func(10);
}

template <typename T>
class Foo : public detail::Base{
 public:

  typedef typename T::zoo_type arg_type;

  Foo(char c) : iguana(c) { f = 0.9; }

  Foo(arg_type i_, float f_) : kak(i_), iguana(ns::foo2(i_)), f(f_) {}
  Foo(arg_type i_, float f_, int tt) {
    using ns::foo2;
    iguana = foo2(i_, i_);
  }

  Foo() : detail::Base(), iguana(1) {}

 private:
  int kak;
  int iguana;
  float f;

  struct Faborz {
  } faborz;
};


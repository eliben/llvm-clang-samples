namespace Vroom {
  int kwa;

  int foobar(int i) {
    return i * 2;
  }

  constexpr double dodo();

  float bar3();
  double foo2();

  float bolly();

  struct Joe {
    double bar3();
  };
};


int bolly();


namespace Swing {
  constexpr float juju = 2.0;

  float foobar(float i) {
    return i * 2;
  }

  using ::bolly;
};


namespace blagonda {
  using Vroom::bolly;
};

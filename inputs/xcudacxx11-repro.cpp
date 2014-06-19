  template <typename T>
  struct K1 {
    T member;
  };

  template <typename T>
  struct K2 {
    T member;
  };

  template <typename T>
  struct K3 {
    T member;
  };

  K3<K2<K1<int>>> var;



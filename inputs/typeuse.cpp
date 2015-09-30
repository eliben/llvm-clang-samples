namespace Frob {

template <typename T>
struct Homer {
  T a, b;
};

}

using namespace Frob;

Homer<float> h;

void foobar(Frob::Homer<double> hh) {
}

template <typename T>
void templated() {
  T t;
}

void joe() {
  templated<Homer<float>>();

  int i[] = {10, 20};
  Homer<double>* h = reinterpret_cast<Homer<double>*>(i);
}

template<typename Scalar1, typename Scalar2>
struct scalar_multiple2_op {
};

template<typename UnaryOp, typename XprType>
class CwiseUnaryOp
{};

inline const CwiseUnaryOp<scalar_multiple2_op<int,std::complex<int> >, float>
operator*(const Homer<int>& scalar) const
{
}



// template <typename T, typename U>
// class Container {
//   T t;
//   Homer<T> ht;
// };

// template <typename U>
// class Container<Homer<float>, U> {
// };

// template <>
// class Container<Homer<float>, int> {};


// template <>
// class Container<Homer<float>, float> {} ;


// /** Overloaded for efficient real matrix times complex scalar value */
// EIGEN_DEVICE_FUNC
// inline const CwiseUnaryOp<internal::scalar_multiple2_op<Scalar,std::complex<Scalar> >, const Derived>
// operator*(const std::complex<Scalar>& scalar) const
// {
//   return CwiseUnaryOp<internal::scalar_multiple2_op<Scalar,std::complex<Scalar> >, const Derived>
//     (*static_cast<const Derived*>(this), internal::scalar_multiple2_op<Scalar,std::complex<Scalar> >(scalar));
// }

// template<typename Scalar1, typename Scalar2>
// struct scalar_multiple2_op {
//   typedef typename scalar_product_traits<Scalar1,Scalar2>::ReturnType result_type;
//   EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE scalar_multiple2_op(const scalar_multiple2_op& other) : m_other(other.m_other) { }
//   EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE scalar_multiple2_op(const Scalar2& other) : m_other(other) { }
//   EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE result_type operator() (const Scalar1& a) const { return a * m_other; }
//   typename add_const_on_value_type<typename NumTraits<Scalar2>::Nested>::type m_other;
// };

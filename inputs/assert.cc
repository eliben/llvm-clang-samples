  extern "C" void __assertfail(
    const void  *message,
    const void  *file,
    unsigned int line,
    const void  *function,
    unsigned long       charsize);

  void __assert_fail(
    const char  *__assertion,
    const char  *__file,
    unsigned int __line,
    const char  *__function)
  {
    __assertfail(
      (const void *)__assertion,
      (const void *)__file,
                    __line,
      (const void *)__function,
      sizeof(char));
  }

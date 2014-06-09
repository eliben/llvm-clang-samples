#include <string>

int foo(std::string s);

int bar(std::string joe) {
  // This c_str() call should be replaced by remov-cstr-calls
  return foo(joe.c_str());
}

#include <iostream>
#include <exception>
#include <stdexcept>

void foo(int i) {
  if (i > 2) {
    throw std::runtime_error("Expected i <= 2");
  }
}

int main(int argc, char** argv) {
  try {
    foo(argc);
  }
  catch (std::exception& e) {
    std::cerr << "Got exception: " << e.what() << std::endl;
  }

  return 0;
}

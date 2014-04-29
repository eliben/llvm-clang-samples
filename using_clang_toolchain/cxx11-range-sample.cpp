#include <iostream>
#include <vector>

int main(int argc, char** argv) {
  // Container initialization - shiny!
  std::vector<int> nums = {1, 2, 3, 4, 5};

  // auto + for-range loop - shiny^2!
  for (auto& n : nums) {
    std::cout << n << " ";
  }

  std::cout << std::endl;
  return 0;
}

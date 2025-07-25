#include "config.hpp"

int32_t
main()
{
  if (!std::cout)
    return -1;
  else
    std::cout << "Fuck world!\n" << std::endl;
  return 0;
}

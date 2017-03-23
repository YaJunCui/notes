#include "common.h"

int main()
{
  const int answer = 42;
  auto& x = answer;
  auto y = &answer;

  return 0;
}
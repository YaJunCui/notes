#include "common.h"

auto create_init_list(int val)
{
  cout << val << endl;
  return val+88;
}

int main()
{
  // auto x = {27};

  // char* name = abi::__cxa_demangle(typeid(x).name(), nullptr, nullptr, nullptr);
  // cout << __FUNCTION__ << " " << name << endl;
  // free(name);

  // cout << *x.begin() << endl;

  int val = 200;
  cout << create_init_list(val) << endl;

  return 0;
}
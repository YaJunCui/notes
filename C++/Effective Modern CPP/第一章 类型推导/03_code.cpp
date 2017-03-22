#include "common.h"

struct Point
{
  int x, y;
};


template <typename Container, typename Index>
decltype(auto) authAndAccess(Container& c, Index i)
{
  return c[i];
}

int main()
{
  // const int i = 0;
  // decltype(i) y = 100;
  // cout << typeid(y).name() << endl;

  // Point pt;
  // decltype(pt) p1;

  // char* name = abi::__cxa_demangle(typeid(p1).name(), nullptr, nullptr, nullptr);
  // cout << __FUNCTION__ << " " << name << endl;
  // free(name);

  vector<int> ivec{1,2,3,4,5,6};

  cout << authAndAccess(ivec,3) << endl;

  for(auto val : ivec)
  {
    cout << val << "\t";
  }
  cout << endl;

  authAndAccess(ivec,3) = 100;

  for(auto val : ivec)
  {
    cout << val << "\t";
  }
  cout << endl;

  return 0;
}
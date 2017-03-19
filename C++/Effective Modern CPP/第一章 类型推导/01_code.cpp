#include <iostream>
#include <typeinfo>

using namespace std;

template <typename T>
void f(const T& param)
{
  cout << typeid(T).name() << endl;
  cout << param << endl;
}


int main()
{
  f(10);

  int x = 0;

  f(x);

  return 0;
}
#include <iostream>
#include <typeinfo>
#include <cstdlib>

#ifndef _MSC_VER
#include <cxxabi.h>
#endif

using namespace std;

// template <typename T>
// void f(const T& param)
// {
//   cout << typeid(T).name() << endl;
//   cout << param << endl;
// }

template <typename T>
void f0(T& param)
{
   char* name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
   cout << name << endl;
   free(name);
}

template <typename T>
void f1(const T& param)
{
   char* name = abi::__cxa_demangle(typeid(param).name(), nullptr, nullptr, nullptr);
   cout << __FUNCTION__ << " " << name << endl;
   free(name);
}

template<typename T>
void f2(T* param)
{
   char* name = abi::__cxa_demangle(typeid(param).name(), nullptr, nullptr, nullptr);
   cout << __FUNCTION__ << " " << name << endl;
   free(name);
}

template<typename T>
void f3(const T* param)
{
   char* name = abi::__cxa_demangle(typeid(param).name(), nullptr, nullptr, nullptr);
   cout << __FUNCTION__ << " " << name << endl;
   free(name);
}


template<typename T>
void f4(T&& param)
{
  char* name = abi::__cxa_demangle(typeid(param).name(), nullptr, nullptr, nullptr);
  cout << __FUNCTION__ << " " << name << endl;
  free(name);
}

int main()
{
  // int x = 27;
  // f(x);
  // const int cx = x;
  // f(cx);
  // const int& rx = x;
  // f1(rx);

  // int x = 27;
  // const int *px = &x;
  // const int * const px1 = px;

  // f3(&x);  //T是int，        param的类型是int*
  // f3(px);  //T是cosnt int，  param的类型是const int*
  // f3(px1);  //T是cosnt int，  param的类型是const int*

  int x = 27;
  const int cx = x;
  const int& rx = x;

  f4(x);   
  f4(cx);  
  f4(rx);  
  f4(27);  

  return 0;
}
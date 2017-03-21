#include "common.h"

template <typename T>
void f(T param)
{
   char* name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
   cout << __FUNCTION__ << " " << name << endl;
   free(name);
}

template <typename T>
void f0(T& param)
{
   char* name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
   cout << __FUNCTION__ << " " << name << endl;
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

template<typename T>
void f5(T& param)
{
  char* name = abi::__cxa_demangle(typeid(param).name(), nullptr, nullptr, nullptr);
  cout << __FUNCTION__ << " " << name << endl;
  free(name);
}

//在编译期返回数组大小（数组参数没有名字，因为我们仅仅为了知道数组的大小）
template<typename T, std::size_t N>
constexpr std::size_t array_size(T(&)[N]) noexcept
{
  return N;
}

void someFunc(int, double)
{

}

template<typename T>
void f6(T param)
{
  char* name = abi::__cxa_demangle(typeid(param).name(), nullptr, nullptr, nullptr);
  cout << __FUNCTION__ << " " << name << endl;
  free(name);
}

template<typename T>
void f7(T& param)
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

  // const char name[] = "cui ya jun";
  // f(name);
  // f5(name);

  // cout << "array size = " << array_size(name) << endl;

  f6(someFunc);   //T是void (*)(int, double)
  f7(someFunc);   //T是 void ()(int, double)

  return 0;
}
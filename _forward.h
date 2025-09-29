#pragma once
#include <iostream>
#include <string>
using namespace std;

namespace _forward
	// 完美转发: 在转发参数的同时，保留并传递值类别
	// 核心:
	// 1.万能引用和模板类型推导: 保留值类别, 将左值捕获为左值引用, T推导为U&，将右值捕获为右值引用, T推导为值类型U
	// 2.forward 的有条件转发: 传递值类别, 只将右值引用转为右值, 否则转为左值 (move 无条件转发为右值)
	// 解决痛点: 既用引用/移动代替拷贝, 又用一个函数代替多个重载
	// 使用 T 导致不必要的拷贝, 使用 T& 又无法接受右值
	// C++11前, 唯一的方法是为左值右值设计两个函数, 对于右值仍然只能用 const& 来捕获
	// C++11后(不用完美转发的情况下), 可以右值引用捕获右值, 再 move 触发移动, 但是仍然需要管理多个函数
{
	template <class T>
	constexpr T&& __forward(remove_reference_t<T>& arg) noexcept
	{
		return static_cast<T&&>(arg); // 引用折叠
	}

	template <class T>
	constexpr T&& __forward(remove_reference_t<T>&& _Arg) noexcept // forward 对于右值参数的重载
	{
		cout << "r forward" << endl;
		return static_cast<T&&>(_Arg);
	}

	inline void f(const string& s) { cout << "f左值: " << s << endl; }
	inline void f(string&& s) { cout << "f右值: " << s << endl; }
	template<typename T>
	inline void g(T&& arg) // 万能引用, 引用折叠
	{
		f(__forward<T>(arg)); // 将参数转发到正确的那一个 f()
	}

	inline void main()
	{
		string s("Hello");
		g(s);
		g(move(s));

		string&& ss = move(s);
		__forward<string&&>(ss);
	}
}
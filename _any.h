#pragma once
#include <iostream>
#include <any>
using namespace std;

namespace _any
{
	inline void main()
	{
		any a;
		cout << a.type().name() << endl; // 动态类型 void, 可以理解为更安全智能的 void*
		cout << typeid(a).name() << endl; // 静态类型 class std::any

		a = "abc";
		cout << sizeof(a) << endl; // 64
		// 通过预留一块固定大小(如64)的缓冲区来避免每次都进行动态内存分配, 实现小对象优化

		a.reset();
		if (!a.has_value()) cout << "a has no value" << endl;

		a = make_any<string>("abc");
		a.emplace<string>("abc"); // make_any 和 emplace 原地构造

		// 使用 try 访问
		try
		{
			cout << any_cast<string>(a) << endl; // abc
			cout << any_cast<int>(a) << endl; // bad any cast
		}
		catch (const bad_any_cast& e)
		{
			cout << e.what() << endl;
		}

		// 使用指针访问
		auto p = any_cast<int>(&a); // 使用 any_cast<T>(any*) 的重载
		if (p) cout << *p << endl;
		else cout << "nullptr" << endl; // nullptr

		// 使用 typeid 先判断再访问
		if (a.type() == typeid(string))
			cout << any_cast<string>(a) << endl; // abc
	}
}
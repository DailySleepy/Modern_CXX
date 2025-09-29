#pragma once
#include <iostream>
#include <functional>
#include <memory>
#include <string>
using namespace std;

namespace _invoke
{
	inline void _invoke()
	{
		auto print = [](string msg) { cout << msg << endl; };
		struct F
		{
			void operator()(string msg) { cout << msg << endl; }
		};
		struct C
		{
			int v = 6;
			void print(string msg) { cout << msg << endl; }
		};

		// 调用普通函数或lambda
		invoke(print, "simple");

		// 调用函数对象
		F f;
		invoke(f, "factor");

		// 调用成员函数(通过值/引用/指针)
		C o;
		auto p = make_unique<C>();
		invoke(&C::print, o, "member");
		invoke(&C::print, &o, "ref");
		invoke(&C::print, p, "ptr");

		// 调用成员变量
		cout << "v is " << invoke(&C::v, o) << endl;
	}

	inline void compare_with_bind()
	{
		cout << endl << "---compare_with_bind---" << endl;
		struct X
		{
			int i = 0;
			void inc() { i++; }
			void print() { cout << "i: " << i << endl; }
		};

		X x;
		cout << "initial:\n";
		x.print();
		cout << "invoke:\n";
		{
			invoke(&X::inc, x);
			x.print();
			invoke(&X::inc, &x);
			x.print();
			invoke(&X::inc, ref(x));
			x.print();
			// 0 1 2 3
			// invoke 是立即调用, Arg&&通用引用, 所以x, &x, ref(x)的结果一样
		}
		x.i = 0;
		cout << "bind:\n";
		{
			auto b1 = bind(&X::inc, x); b1();
			x.print();
			auto b2 = bind(&X::inc, &x); b2();
			x.print();
			auto b3 = bind(&X::inc, ref(x)); b3();
			x.print();
			// 0 0 1 2
			// bind 是创建可调用对象, 延迟执行
			// 将参数都存起来, 这要求使用decay_t来退化类型(与thread相同的要求), 所以传值x不会对i产生影响
		}
	}

	inline void main()
	{
		_invoke();
		compare_with_bind();
	}
}
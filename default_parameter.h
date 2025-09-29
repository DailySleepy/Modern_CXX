#pragma once
#include <iostream>
#include <memory>
using namespace std;

namespace default_parameter
{
	struct B
	{
		virtual void f(int a = 666) { cout << "B: " << a << endl; }
		// virtual 提供运行时多态, 实现函数覆写
	};
	struct D : B
	{
		void f(int a = 0) override { cout << "D: " << a << endl; }
	};

	struct X
	{
		//const int a = 0; // 不能作为默认参数, 因为在离开类的作用域时 const 变量会失效, 必须要static
		//constexpr int a = 0; // ERROR: 非 static 变量属于类实例, 这与 constexpr 语义矛盾, 必须要static
		//static int a = 0; // ERROR: static 变量必须是 inline 或 const/constexpr
		inline static int a = 0; // inline 解决了之前的版本中必须在类外对 static 变量进行定义的麻烦
		const static int aa = 1;
		void f(int b = sizeof a + aa) { cout << b << endl; } // 默认参数必须是属于静态存储期, 即static
	};

	inline void main()
	{
		unique_ptr<B> p = make_unique<D>();
		p->f();
		// 默认参数: 编译时静态绑定, 由于p是B*类型, 默认参数来自 B:f()
		// 调用函数: 运行时动态绑定, 由于p实际指向的对象是D, 故调用 D:f()

		X x;
		x.f();
	}
}
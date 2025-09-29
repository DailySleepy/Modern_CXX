#pragma once
#include <iostream>
#include <functional>
#include <string>
using namespace std;

namespace _bind
{
	using namespace placeholders;

	inline int add(int a, int b) { return a + b; }

	struct Printer
	{
		void print(const string& s) { cout << s << endl; }
	};

	inline void main()
	{
		// 绑定普通函数
		auto add10 = bind(add, _1, 10); // _1 是占位符, 表示新函数调用时传入的第一个参数
		cout << add10(1) << endl;

		// 绑定成员函数
		Printer obj;
		auto printHello = bind(&Printer::print, &obj, "Hello");
		printHello();

		// 但是 lambda 是实现上述功能更好的选择
		auto add10_lambda = [](int i) { return add(i, 1); };
		auto printHello_lambda = [&obj]() { obj.print("Hello"); };
		// lambda 语法更清晰
		// 此外, bind 的返回值是非常复杂的"未命名函数对象", 难以像 lambda 一样直接内联
	}
}
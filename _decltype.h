#pragma once
#include <string>
#include <array>
#include <iostream>
using namespace std;

namespace _decltype
{
	inline void main()
	{
		int x = 1;
		int a[1] = { 1 };
		// 单纯的标识符:
		using T1 = decltype(x); // T
		// 非单纯的标识符:
		// prv -> T
		using T2 = decltype(1);
		using T8 = decltype(&x);
		using T7 = decltype(string("***"));
		// lv -> T&
		using T5 = decltype(("***"));
		using T3 = decltype(a[0]);
		using T4 = decltype((x));
		// xv -> T&&
		using T6 = decltype(move(x));

		// decltype(auto) 完全保留类型
		auto g = [] -> const int&& { return 0; };
		auto f = [&] -> decltype(auto) { return g(); };
		decltype(auto) v = f();
		auto vv = f();
		decltype(auto) s = "abc";
		auto ss = "abc";

		//decltype(auto) shaders = { "basic.vert", "pbr.vert" };
		// ERROR: 大括号列表本身没有一个可以被 decltype 推导的独立类型
		auto shaders1 = { "basic.vert", "pbr.vert" };
		const char* shaders2[] = { "basic.vert", "pbr.vert" };
		array shaders3 = { "basic.vert", "pbr.vert" };
	}
}
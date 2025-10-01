#pragma once
#include <iostream>
#include <variant>
using namespace std;

namespace _variant
{
	struct Visitor1
	{
		void operator()(int) { cout << "visit int" << endl; }
		void operator()(string) { cout << "visit string" << endl; }
	};

	template<typename... Ts>
	struct Visitor2 : Ts...
	{
		// 调用operator()时, 找到 Int::operator()(int) 和 String::operator()(string)
		// 但是二者位于不同的基类作用域, 无法比较, 报错: "不明确"
		using Ts::operator()...;
		// 将基类的 operator() 都拉入当前类, 这样才可以进行重载决议

		Visitor2() = default;
		Visitor2(Ts... ts) : Ts(ts)... {} // C++17
	};
	// 推导指引
	template<typename... Ts>
	Visitor2(Ts... ts) -> Visitor2<Ts...>; // C++17

	struct Int { void operator()(int) { cout << "visit int" << endl; } };
	struct String { void operator()(string) { cout << "visit string" << endl; } };

	auto lambda_int = [](int) { cout << "visit int" << endl; };
	auto lambda_string = [](string) { cout << "visit string" << endl; };
	auto lambda_int_or_string = [](auto&& arg)
	{
		using T = decay_t<decltype(arg)>;
		if constexpr (is_same_v<T, int>)  cout << "visit int" << endl;
		else if constexpr (is_same_v<T, string>)  cout << "visit string" << endl;
		else cout << "unsupported type" << endl;
	};

	inline void main()
	{
		using VarType = variant<string, int>;

		VarType v = 1;
		v.emplace<string>("get string");

		// 访问的类型不是当前类型时会抛出错误
		try
		{
			cout << get<int>(v) << endl; // bad variant access
			cout << get<string>(v) << endl; // string
			// get<float>(v) ERROR: 无法访问未声明的类型
		}
		catch (const bad_variant_access& e)
		{
			cout << e.what() << endl;
		}

		// 使用 index 和 get<i>
		if (v.index() == 0) cout << get<0>(v) << endl;

		// 使用 holds_alternative 和 get<T>
		if (holds_alternative<string>(v)) cout << get<string>(v) << endl;

		// 使用 visit
		visit(Visitor1{}, v); // 传统函数对象
		visit(Visitor2<Int, String>{}, v); // 继承重载集合 -- 结构体继承
		visit(Visitor2{ lambda_int, lambda_string }, v); // 继承重载集合 -- lambda 继承
		// Visitor2{ lambda_int, lambda_string } 等价于 
		// Visitor2<decltype(lambda_int), decltype(lambda_string)>{ lambda_int, lambda_string }
		// 利用了类模板参数推导（CTAD, Class Template Argument Deduction）
		// 但是如果要在 C++17 使用, 还要依赖于构造函数或推导指引
		// 直到 C++20 引入聚合 CATD 扩展
		visit(lambda_int_or_string, v); // lambda + constexpr
	}
}
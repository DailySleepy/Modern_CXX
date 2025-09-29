#pragma once
#include <iostream>
#include <type_traits>
#include <cmath>
using namespace std;

namespace type_traits
{
	// 1.如何判断类型特征
	template<typename T>
	struct is_int_strict : false_type {};
	template<>
	struct is_int_strict<int> : true_type {};
	template<typename T>
	struct is_int : is_int_strict<remove_cv_t<remove_reference_t<T>>> {};
	// 先移除引用, 再移除 cv: 是 & 修饰 const, 而不是 const 修饰 &
	// 也可以直接用 remove_cvref_t
	template<typename T>
	constexpr bool my_is_int_v{ is_int<T>::value };

	// 2.使用类型特征来选择函数模板
	class IMeasurable
	{
	public:
		virtual double length() const = 0;
	};
	class Vec : public IMeasurable
	{
	public:
		int x, y;
		Vec(int x, int y) : x(x), y(y) {}
		double length() const { return sqrt(x * x + y * y); }
	};
	template<typename T>
	inline double length(T&& t)
	{
		if constexpr (is_arithmetic_v<T>)
			return abs(t);
		else if constexpr (is_base_of_v<IMeasurable, remove_reference_t<T>>)
			// 引用类型不参与 is_base_of 判断，所以先移除引用
			return t.length();
		return 0;
	}

	inline void main()
	{
		cout << boolalpha << my_is_int_v<const int&> << endl;

		cout << length(2) << endl;
		cout << length(Vec(3, 4)) << endl;
		const auto v = Vec(3, 4);
		cout << length(v) << endl;
	}
}
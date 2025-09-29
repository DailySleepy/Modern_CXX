#pragma once
#include <concepts>
#include <type_traits>
using namespace std;

namespace _concept
{
	template<typename T>
	concept AddAble =
		requires(T a)
	{
		{ a + a } -> same_as<T>;
		requires is_arithmetic_v<T>;
	};

	template<typename T>
	concept Divisible =
		is_arithmetic_v<T>;

	template<typename T>
	auto add1(T a, T b) requires AddAble<T>;

	template<AddAble T>
	auto add2(T a, T b);

	template<typename T>
		requires requires(T a, T b) { { a + b } -> same_as<T>; }
	auto add3(T a, T b);

	auto add4(AddAble auto a, AddAble auto b);

	template<typename T>
	auto mean(T a, T b) requires AddAble<T>&& Divisible<T>;

	template<typename T>
		requires integral<T>&& AddAble<T>
	struct Pair
	{
		T a, b;
		Pair(T a, T b) : a(a), b(b) {};
	};

	template<integral auto N>
	struct Array
	{
		int arr[N];
	};

	inline void main()
	{
		Pair p(1, 2);
		Array<3> a;
	}
}
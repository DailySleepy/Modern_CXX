#pragma once
#include <iostream>
#include <string_view>
using namespace std;

namespace user_defined_literals
{
	// operator""_后缀名
	inline auto operator""_km(long double d)
		// 浮点数只接受 long double, 防止重载 (避免复杂的重载决议)
	{
		return d * 1000;
	}

	inline void main()
	{
		auto f = 1.f;

		auto dis = 1.5_km; cout << dis << endl;

		auto str = "sss"sv;

		// auto time = 50h; // 需要 <chrono>，如需用可自行解注
	}
}
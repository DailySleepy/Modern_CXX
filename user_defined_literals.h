#pragma once
#include <iostream>
#include <string_view>
using namespace std;

namespace user_defined_literals
{
	// operator""_��׺��
	inline auto operator""_km(long double d)
		// ������ֻ���� long double, ��ֹ���� (���⸴�ӵ����ؾ���)
	{
		return d * 1000;
	}

	inline void main()
	{
		auto f = 1.f;

		auto dis = 1.5_km; cout << dis << endl;

		auto str = "sss"sv;

		// auto time = 50h; // ��Ҫ <chrono>�������ÿ����н�ע
	}
}
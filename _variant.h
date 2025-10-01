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
		// ����operator()ʱ, �ҵ� Int::operator()(int) �� String::operator()(string)
		// ���Ƕ���λ�ڲ�ͬ�Ļ���������, �޷��Ƚ�, ����: "����ȷ"
		using Ts::operator()...;
		// ������� operator() �����뵱ǰ��, �����ſ��Խ������ؾ���

		Visitor2() = default;
		Visitor2(Ts... ts) : Ts(ts)... {} // C++17
	};
	// �Ƶ�ָ��
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

		// ���ʵ����Ͳ��ǵ�ǰ����ʱ���׳�����
		try
		{
			cout << get<int>(v) << endl; // bad variant access
			cout << get<string>(v) << endl; // string
			// get<float>(v) ERROR: �޷�����δ����������
		}
		catch (const bad_variant_access& e)
		{
			cout << e.what() << endl;
		}

		// ʹ�� index �� get<i>
		if (v.index() == 0) cout << get<0>(v) << endl;

		// ʹ�� holds_alternative �� get<T>
		if (holds_alternative<string>(v)) cout << get<string>(v) << endl;

		// ʹ�� visit
		visit(Visitor1{}, v); // ��ͳ��������
		visit(Visitor2<Int, String>{}, v); // �̳����ؼ��� -- �ṹ��̳�
		visit(Visitor2{ lambda_int, lambda_string }, v); // �̳����ؼ��� -- lambda �̳�
		// Visitor2{ lambda_int, lambda_string } �ȼ��� 
		// Visitor2<decltype(lambda_int), decltype(lambda_string)>{ lambda_int, lambda_string }
		// ��������ģ������Ƶ���CTAD, Class Template Argument Deduction��
		// �������Ҫ�� C++17 ʹ��, ��Ҫ�����ڹ��캯�����Ƶ�ָ��
		// ֱ�� C++20 ����ۺ� CATD ��չ
		visit(lambda_int_or_string, v); // lambda + constexpr
	}
}
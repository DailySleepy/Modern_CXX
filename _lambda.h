#pragma once
#include <iostream>
using namespace std;

namespace _lambda
{
	inline int g = 1;
	inline void main()
	{
		struct X : decltype([] { cout << "666" << endl; }) { }; // lambda ��һ���� (��������)
		X x;
		x();

		//auto l1 = [num = 1]() { num++; }; // ERROR: lambda::operator() Ĭ���� const ����
		auto l1 = [num = 1] mutable { num++; };

		int val = 0;
		auto l2 = [&val]() { val++; }; // const �����п����޸����õĲ���

		void (*p1)() = []() {}; // �޲���ʱ, lambda ���Ա���Ϊ��������
		auto p4 = +[] {}; // '+' ������ lambda ת��Ϊ����ָ��, ������Ϊ�� (�����СΪ1)
		cout << sizeof(p1) << " " << sizeof(p4) << endl;

		auto p2 = [](auto n) { return n + 1; };
		auto p3 = []<typename T>(T n) { return n + 1; };
		cout << p2(1) << " " << p3("abc") << endl;
		// auto g1 = [](auto a) { &y; };
		// ERROR: ��Ȼ������δ�����ODR-use, 
		// ��������ģ������׶μ�鵼���ڴ�ʱ���������һ����, ֱ��ģ�屻ʵ����

		const int a = 1;
		constexpr int b = 1;
		static int c = 1;
		int d = 1;
		//const float f = 1; // ERROR: const float ���ܱ���Ϊ constexpr
		constexpr float f = 1;
		auto p5 = []()
		{
			a; b; f; // ��������֪, ����ֱ���滻, ���� ODR-use
			//d; &a; &b; // ERROR: ODR-use �������뱻�����Ա��� lambda �ıհ���
			c; &c; g; &g; // lambda ����ֱ�ӷ���"ȫ�ֱ���"��"����������еľֲ���̬����"
		};

		const int y = 1;
		auto g1 = [] { y; };
		auto g2 = [=] { y; }; // ��ֵ���ʽ, ������ODR-use, ������ʾָ������, �ʲ���
		auto g3 = [=] { sizeof y; }; // ����ֵ���ʽ, ��������ODR-use, �ʲ�����
		cout << sizeof g1 << " " << sizeof g2 << " " << sizeof g3 << '\n'; // 1, 4, 1
	}
}
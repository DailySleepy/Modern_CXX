#pragma once
#include <iostream>
#include <memory>
using namespace std;

namespace default_parameter
{
	struct B
	{
		virtual void f(int a = 666) { cout << "B: " << a << endl; }
		// virtual �ṩ����ʱ��̬, ʵ�ֺ�����д
	};
	struct D : B
	{
		void f(int a = 0) override { cout << "D: " << a << endl; }
	};

	struct X
	{
		//const int a = 0; // ������ΪĬ�ϲ���, ��Ϊ���뿪���������ʱ const ������ʧЧ, ����Ҫstatic
		//constexpr int a = 0; // ERROR: �� static ����������ʵ��, ���� constexpr ����ì��, ����Ҫstatic
		//static int a = 0; // ERROR: static ���������� inline �� const/constexpr
		inline static int a = 0; // inline �����֮ǰ�İ汾�б���������� static �������ж�����鷳
		const static int aa = 1;
		void f(int b = sizeof a + aa) { cout << b << endl; } // Ĭ�ϲ������������ھ�̬�洢��, ��static
	};

	inline void main()
	{
		unique_ptr<B> p = make_unique<D>();
		p->f();
		// Ĭ�ϲ���: ����ʱ��̬��, ����p��B*����, Ĭ�ϲ������� B:f()
		// ���ú���: ����ʱ��̬��, ����pʵ��ָ��Ķ�����D, �ʵ��� D:f()

		X x;
		x.f();
	}
}
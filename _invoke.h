#pragma once
#include <iostream>
#include <functional>
#include <memory>
#include <string>
using namespace std;

namespace _invoke
{
	inline void _invoke()
	{
		auto print = [](string msg) { cout << msg << endl; };
		struct F
		{
			void operator()(string msg) { cout << msg << endl; }
		};
		struct C
		{
			int v = 6;
			void print(string msg) { cout << msg << endl; }
		};

		// ������ͨ������lambda
		invoke(print, "simple");

		// ���ú�������
		F f;
		invoke(f, "factor");

		// ���ó�Ա����(ͨ��ֵ/����/ָ��)
		C o;
		auto p = make_unique<C>();
		invoke(&C::print, o, "member");
		invoke(&C::print, &o, "ref");
		invoke(&C::print, p, "ptr");

		// ���ó�Ա����
		cout << "v is " << invoke(&C::v, o) << endl;
	}

	inline void compare_with_bind()
	{
		cout << endl << "---compare_with_bind---" << endl;
		struct X
		{
			int i = 0;
			void inc() { i++; }
			void print() { cout << "i: " << i << endl; }
		};

		X x;
		cout << "initial:\n";
		x.print();
		cout << "invoke:\n";
		{
			invoke(&X::inc, x);
			x.print();
			invoke(&X::inc, &x);
			x.print();
			invoke(&X::inc, ref(x));
			x.print();
			// 0 1 2 3
			// invoke ����������, Arg&&ͨ������, ����x, &x, ref(x)�Ľ��һ��
		}
		x.i = 0;
		cout << "bind:\n";
		{
			auto b1 = bind(&X::inc, x); b1();
			x.print();
			auto b2 = bind(&X::inc, &x); b2();
			x.print();
			auto b3 = bind(&X::inc, ref(x)); b3();
			x.print();
			// 0 0 1 2
			// bind �Ǵ����ɵ��ö���, �ӳ�ִ��
			// ��������������, ��Ҫ��ʹ��decay_t���˻�����(��thread��ͬ��Ҫ��), ���Դ�ֵx�����i����Ӱ��
		}
	}

	inline void main()
	{
		_invoke();
		compare_with_bind();
	}
}
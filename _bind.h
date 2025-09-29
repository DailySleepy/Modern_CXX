#pragma once
#include <iostream>
#include <functional>
#include <string>
using namespace std;

namespace _bind
{
	using namespace placeholders;

	inline int add(int a, int b) { return a + b; }

	struct Printer
	{
		void print(const string& s) { cout << s << endl; }
	};

	inline void main()
	{
		// ����ͨ����
		auto add10 = bind(add, _1, 10); // _1 ��ռλ��, ��ʾ�º�������ʱ����ĵ�һ������
		cout << add10(1) << endl;

		// �󶨳�Ա����
		Printer obj;
		auto printHello = bind(&Printer::print, &obj, "Hello");
		printHello();

		// ���� lambda ��ʵ���������ܸ��õ�ѡ��
		auto add10_lambda = [](int i) { return add(i, 1); };
		auto printHello_lambda = [&obj]() { obj.print("Hello"); };
		// lambda �﷨������
		// ����, bind �ķ���ֵ�Ƿǳ����ӵ�"δ������������", ������ lambda һ��ֱ������
	}
}
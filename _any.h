#pragma once
#include <iostream>
#include <any>
using namespace std;

namespace _any
{
	inline void main()
	{
		any a;
		cout << a.type().name() << endl; // ��̬���� void, �������Ϊ����ȫ���ܵ� void*
		cout << typeid(a).name() << endl; // ��̬���� class std::any

		a = "abc";
		cout << sizeof(a) << endl; // 64
		// ͨ��Ԥ��һ��̶���С(��64)�Ļ�����������ÿ�ζ����ж�̬�ڴ����, ʵ��С�����Ż�

		a.reset();
		if (!a.has_value()) cout << "a has no value" << endl;

		a = make_any<string>("abc");
		a.emplace<string>("abc"); // make_any �� emplace ԭ�ع���

		// ʹ�� try ����
		try
		{
			cout << any_cast<string>(a) << endl; // abc
			cout << any_cast<int>(a) << endl; // bad any cast
		}
		catch (const bad_any_cast& e)
		{
			cout << e.what() << endl;
		}

		// ʹ��ָ�����
		auto p = any_cast<int>(&a); // ʹ�� any_cast<T>(any*) ������
		if (p) cout << *p << endl;
		else cout << "nullptr" << endl; // nullptr

		// ʹ�� typeid ���ж��ٷ���
		if (a.type() == typeid(string))
			cout << any_cast<string>(a) << endl; // abc
	}
}
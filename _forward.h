#pragma once
#include <iostream>
#include <string>
using namespace std;

namespace _forward
	// ����ת��: ��ת��������ͬʱ������������ֵ���
	// ����:
	// 1.�������ú�ģ�������Ƶ�: ����ֵ���, ����ֵ����Ϊ��ֵ����, T�Ƶ�ΪU&������ֵ����Ϊ��ֵ����, T�Ƶ�Ϊֵ����U
	// 2.forward ��������ת��: ����ֵ���, ֻ����ֵ����תΪ��ֵ, ����תΪ��ֵ (move ������ת��Ϊ��ֵ)
	// ���ʹ��: ��������/�ƶ����濽��, ����һ����������������
	// ʹ�� T ���²���Ҫ�Ŀ���, ʹ�� T& ���޷�������ֵ
	// C++11ǰ, Ψһ�ķ�����Ϊ��ֵ��ֵ�����������, ������ֵ��Ȼֻ���� const& ������
	// C++11��(��������ת���������), ������ֵ���ò�����ֵ, �� move �����ƶ�, ������Ȼ��Ҫ����������
{
	template <class T>
	constexpr T&& __forward(remove_reference_t<T>& arg) noexcept
	{
		return static_cast<T&&>(arg); // �����۵�
	}

	template <class T>
	constexpr T&& __forward(remove_reference_t<T>&& _Arg) noexcept // forward ������ֵ����������
	{
		cout << "r forward" << endl;
		return static_cast<T&&>(_Arg);
	}

	inline void f(const string& s) { cout << "f��ֵ: " << s << endl; }
	inline void f(string&& s) { cout << "f��ֵ: " << s << endl; }
	template<typename T>
	inline void g(T&& arg) // ��������, �����۵�
	{
		f(__forward<T>(arg)); // ������ת������ȷ����һ�� f()
	}

	inline void main()
	{
		string s("Hello");
		g(s);
		g(move(s));

		string&& ss = move(s);
		__forward<string&&>(ss);
	}
}
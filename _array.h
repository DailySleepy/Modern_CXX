#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <cstring>
using namespace std;

namespace _array
{
	struct FArray
	{
		~FArray() = default;
		int size;
		char data[];
		// ��������, ���ֶ����ݺ� struct ͷ���������ڴ��Ͻ�������
		// �������� vector һ���������������ڴ����
	};
	inline void main()
	{
		int a[5]{ 1 }; // ֻ��ʼ����һ��Ԫ��Ϊ1, ����Ϊ0
		cout << a[1] << endl;

		cout << boolalpha << is_same_v<int[1], int[2]> << endl; // ����������, ������С
		// ֻ�ڴ���, ���ʽ(����sizeof, &)���˻�Ϊָ��

		// vector<int[2]> v;
		// v.push_back({ 1, 2 }); 
		// ERROR: ����Ԫ����Ҫ���ƶ��ɿ���, ��ԭʼ���鲻����

		vector<array<int, 2>> v;
		v.push_back({ 1, 2 });
		// v.emplace_back(1, 2);
		// ERROR: emplace �����ڹ��캯��, ������������ʵ�Ǿۺϳ�ʼ��{}, �ⲻ�����ڹ��캯��

		// ָ�����������ָ�루����Ϊ int (*)[5]����������ָ����Ԫ�ص�ָ�� int*
		auto p1 = &a;
		// int*
		auto p0 = a;
		auto p2 = &a[0];
		auto p3 = a + 1;

		int n = 10;
		{
			cout << "malloc:" << endl;
			auto p = (FArray*)malloc(sizeof(FArray) + n * sizeof(char));
			p->size = n;
			strcpy_s(p->data, 10, "123456789");
			cout << p->data << endl;
			free(p);
			// ��Ϊ FArray �� Plain Old Data(POD) ����, ���Կ���ʹ�ü򵥵� malloc / free
		}
		{
			cout << "new:" << endl;
			void* mem = ::operator new(sizeof(FArray) + n * sizeof(char)); // ����ԭʼ�ڴ棬�����ù��캯����
			auto* p = new (mem) FArray; // ʹ�ö�λ new ���ѷ�����ڴ� mem �Ϲ��� FArray ����, �������ڴ����
			p->size = n;
			strcpy_s(p->data, 10, "123456789");
			cout << p->data << endl;
			p->~FArray(); // ��� FArray ������Ҫ�������Դ, ������ֶ�������������
			::operator delete(mem); // �ͷ�ԭʼ�ڴ�
		}
	}
}
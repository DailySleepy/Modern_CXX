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
		// 柔性数组, 该字段数据和 struct 头部数据在内存上紧密排列
		// 而不是像 vector 一样数据与对象本身的内存分离
	};
	inline void main()
	{
		int a[5]{ 1 }; // 只初始化第一个元素为1, 其他为0
		cout << a[1] << endl;

		cout << boolalpha << is_same_v<int[1], int[2]> << endl; // 数组是类型, 包含大小
		// 只在传参, 表达式(除了sizeof, &)中退化为指针

		// vector<int[2]> v;
		// v.push_back({ 1, 2 }); 
		// ERROR: 容器元素需要可移动可拷贝, 而原始数组不可以

		vector<array<int, 2>> v;
		v.push_back({ 1, 2 });
		// v.emplace_back(1, 2);
		// ERROR: emplace 依赖于构造函数, 我们期望的其实是聚合初始化{}, 这不依赖于构造函数

		// 指向整个数组的指针（类型为 int (*)[5]），而不是指向首元素的指针 int*
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
			// 因为 FArray 是 Plain Old Data(POD) 类型, 所以可以使用简单的 malloc / free
		}
		{
			cout << "new:" << endl;
			void* mem = ::operator new(sizeof(FArray) + n * sizeof(char)); // 分配原始内存，不调用构造函数。
			auto* p = new (mem) FArray; // 使用定位 new 在已分配的内存 mem 上构造 FArray 对象, 不进行内存分配
			p->size = n;
			strcpy_s(p->data, 10, "123456789");
			cout << p->data << endl;
			p->~FArray(); // 如果 FArray 包含需要清理的资源, 则必须手动调用析构函数
			::operator delete(mem); // 释放原始内存
		}
	}
}
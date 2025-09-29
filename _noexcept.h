#pragma once
#include <iostream>
using namespace std;

namespace _noexcept
	// 不抛异常保证 
	// 如果一个 noexcept 函数抛出异常，程序会立即调用 std::terminate() 并强制终止
	// 解决了 throw(e...) 的运行时开销(检查异常是否匹配e...)和未定义行为(不匹配时调用 unexpected())
	// 其他异常保证等级:
	// 强保证: 成功或完全无影响(回滚), 如 vector.push_back() 失败, 原有的数据不变, 包括内存地址和大小; 方法如拷贝交换
	// 基本保证: 对象仍然是合法有效状态, 仍然可以访问 vector 的元素和方法, 但不保证元素顺序, size(), 内存; 方法如RAII
	// 无保证: 没有考虑异常安全
{
	inline void f() noexcept
	// 普通函数默认是 except 的, noexcept 显式指明不会出错
	// noexcept(false) 显式指明可能出错, 可以防止编译器自动推导为 noexcept(true)
	{
		cout << "print is called" << endl;
	}

	struct C
	{
		int* p = nullptr;
		C() : p(new int(1)) {}
		C(C& o)
		//拷贝构造函数是可能出错的, 涉及内存动态分配
		{
			if (o.p) p = new int(*o.p);
		}
		C(C&& o) noexcept
		// 移动构造函数通常是不会出现错误的, 不涉及内存动态分配(只有指针交换)
		// 如果不是 noexcept, 在强异常保证下, STL在需要移动元素时会回退到效率低下的拷贝操作
		{
			p = o.p;
			o.p = nullptr;
		}
		~C()
		{
			if (p) delete p;
		}
		// 析构函数默认是 noexcept 的
		// 这意味着析构函数不应该抛出异常, 而是当遇到异常时在自己的作用域中处理掉异常
		// 如果析构函数是 noexcept(false), 可能抛出异常, 这可能导致双重异常:
		// 当一个其他异常的抛出触发栈展开, 调用析构函数释放路径上的临时对象, 如果此时析构函数又抛出异常, 就出现了双重异常
		// 此时程序会直接 terminate() 强制终止, 导致临时对象没有正常被析构函数清理, 出现资源泄露

		void swap(C& other) noexcept
		{
			std::swap(p, other.p); // 也要是 noexcept
		}
	};

	inline void main()
	{
		// noexcept 编译期不求值表达式, 不会调用函数
		cout << boolalpha << noexcept(f()) << endl; // true
		C c; cout << boolalpha << noexcept(c.~C()) << endl; // true
	}
}
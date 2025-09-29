#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <functional>
using namespace std;

namespace member_pointer
	// 类的非静态成员在类定义中的偏移量, 依赖于具体的类对象
{
	struct X
	{
		int i = 0;
		void f(int a) { cout << "f is called by " << a << endl; }
		void print() { cout << "i: " << i << endl; }
	};

	template<typename T, typename U>
	inline void printMember(vector<T>& objs, U T::* member_var_ptr)
	{
		for (auto& obj : objs)
			cout << obj.*member_var_ptr << " ";
		cout << endl;
	}
	template<typename T, typename U>
	inline void excuteAction(vector<T>& objs, U(T::* member_fun_ptr)(void))
	{
		for (auto& obj : objs)
			(obj.*member_fun_ptr)();
	}

	inline void main()
	{
		X x;
		// 1.成员变量指针
		int X::* vp = &X::i;
		x.*vp = 66;
		x.print();
		// 作为 invoke 参数调用
		invoke(vp, x) = 6666; // invoke(vp, x) 内联为 x.*vp
		x.print();

		// 2.成员函数指针
		void (X:: * fp)(int) = &X::f;
		(x.*fp)(1);

		// 和普通函数指针一样, 成员函数指针可以作为函数参数
		// lambda 接受成员函数指针, 类实例, 成员函数参数, 这里类似 invoke
		auto lambda = [](void(X::* fp)(int), X& x, int a) { (x.*fp)(a); };
		lambda(fp, x, 2);
		invoke(fp, x, 2);
		cout << endl;

		// 作用: 对一组对象的某个变量做批量操作
		struct Person
		{
			string name;
			int age;
			Person(const string& n, int a) : name(n), age(a) {}
			void jump() { cout << name << " is jumping\n"; }
			void sing() { cout << name << " is singing\n"; }
		};
		vector<Person> people = {
			{"Alice", 30},
			{"Bob", 25},
			{"Charlie", 35}
		};
		// 打印所有人的 name
		printMember(people, &Person::name);
		// 打印所有人的 age
		printMember(people, &Person::age);
		// 让所有人 jump
		excuteAction(people, &Person::jump);
		// 让所有人 sing
		excuteAction(people, &Person::sing);
	}
}
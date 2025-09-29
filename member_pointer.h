#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <functional>
using namespace std;

namespace member_pointer
	// ��ķǾ�̬��Ա���ඨ���е�ƫ����, �����ھ���������
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
		// 1.��Ա����ָ��
		int X::* vp = &X::i;
		x.*vp = 66;
		x.print();
		// ��Ϊ invoke ��������
		invoke(vp, x) = 6666; // invoke(vp, x) ����Ϊ x.*vp
		x.print();

		// 2.��Ա����ָ��
		void (X:: * fp)(int) = &X::f;
		(x.*fp)(1);

		// ����ͨ����ָ��һ��, ��Ա����ָ�������Ϊ��������
		// lambda ���ܳ�Ա����ָ��, ��ʵ��, ��Ա��������, �������� invoke
		auto lambda = [](void(X::* fp)(int), X& x, int a) { (x.*fp)(a); };
		lambda(fp, x, 2);
		invoke(fp, x, 2);
		cout << endl;

		// ����: ��һ������ĳ����������������
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
		// ��ӡ�����˵� name
		printMember(people, &Person::name);
		// ��ӡ�����˵� age
		printMember(people, &Person::age);
		// �������� jump
		excuteAction(people, &Person::jump);
		// �������� sing
		excuteAction(people, &Person::sing);
	}
}
#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>

using namespace std;

namespace _lambda
{
	int g = 1;
	void main()
	{
		struct X : decltype([] { cout << "666" << endl; }) { }; // lambda 是一个类 (函数对象)
		X x;
		x();

		//auto l1 = [num = 1]() { num++; }; // ERROR: lambda::operator() 默认是 const 函数
		auto l1 = [num = 1] mutable { num++; };

		int val = 0;
		auto l2 = [&val]() { val++; }; // const 函数中可以修改引用的参数

		void (*p1)() = []() {}; // 无捕获时, lambda 可以被视为匿名函数
		auto p4 = +[] {}; // '+' 帮助将 lambda 转换为函数指针, 否则视为类 (输出大小为1)
		cout << sizeof(p1) << " " << sizeof(p4) << endl;

		auto p2 = [](auto n) { return n + 1; };
		auto p3 = []<typename T>(T n) { return n + 1; };
		cout << p2(1) << " " << p3("abc") << endl;
		// auto g1 = [](auto a) { &y; };
		// ERROR: 虽然出现了未捕获的ODR-use, 
		// 但是由于模板的两阶段检查导致在此时不会检查出这一错误, 直到模板被实例化

		const int a = 1;
		constexpr int b = 1;
		static int c = 1;
		int d = 1;
		//const float f = 1; // ERROR: const float 不能被视为 constexpr
		constexpr float f = 1;
		auto p5 = []()
		{
			a; b; f; // 编译期已知, 可以直接替换, 不是 ODR-use
			//d; &a; &b; // ERROR: ODR-use 参数必须被捕获以保持 lambda 的闭包性
			c; &c; g; &g; // lambda 可以直接访问"全局变量"和"封闭作用域中的局部静态变量"
		};

		const int y = 1;
		auto g1 = [] { y; };
		auto g2 = [=] { y; }; // 求值表达式, 可能是ODR-use, 加上显示指明捕获, 故捕获
		auto g3 = [=] { sizeof y; }; // 不求值表达式, 不可能是ODR-use, 故不捕获
		cout << sizeof g1 << " " << sizeof g2 << " " << sizeof g3 << '\n'; // 1, 4, 1
	}
}

namespace _noexcept
{
	void f() noexcept
	{
		cout << "f is called" << endl;
	}

	void main()
	{
		cout << boolalpha << noexcept(f()) << endl;
	}
}

namespace user_defined_literals
{
	auto operator""_km(long double d)
	{
		using namespace std::chrono_literals;
		return d * 1000;
	}

	void main()
	{
		auto f = 1.f;

		auto dis = 1.5_km; cout << dis << endl;

		auto str = "sss"sv;

		auto time = 50h;
	}
}

namespace member_pointer
{
	struct X
	{
		int i = 0;
		void f() { cout << "no para" << endl; }
		void f(int a) { i = a; cout << "int para: " << a << endl; }
		void print() { cout << "i: " << i << endl; }
	};

	void main()
	{
		X x;
		void (X:: * fp)(int) = &X::f;
		auto lambda = [](void(X::* fp)(int), X& x, int a) { (x.*fp)(a); };

		(x.*fp)(1);
		lambda(fp, x, 2);

		auto b0 = bind(lambda, fp, ref(x), 3); b0();
		auto b1 = bind(fp, ref(x), 4); b1();

		invoke(lambda, fp, ref(x), 5);
		invoke(fp, ref(x), 6);

		x.print();
		invoke(&X::i, x) = 7;
		x.print();
	}

	void test()
	{
		#define flag true

		struct X
		{
			int i = 0;
			void inc() { i++; }
			void print() { cout << "i: " << i << endl; }
		};

		X x;
		x.print();
		if (!flag)
		{
			std::invoke(&X::inc, x);
			x.print();
			std::invoke(&X::inc, &x);
			x.print();
			std::invoke(&X::inc, std::ref(x));
			x.print();
			// 0 1 2 3
			//invoke立即调用, Arg&&通用引用, 所以x, &x, ref(x)的结果一样(对于数据成员指针和成员函数指针)
		}
		else
		{
			auto b1 = bind(&X::inc, x); b1();
			x.print();
			auto b2 = bind(&X::inc, &x); b2();
			x.print();
			auto b3 = bind(&X::inc, ref(x)); b3();
			x.print();
			// 0 0 1 2
			//bind将参数都存起来, 这要求使用decay_t来退化类型(与thread相同的要求), 所以传值x不会对i产生影响
		}
	}
};

namespace default_parameter
{
	struct B
	{
		virtual void f(int a = 666) { cout << "B: " << a << endl; }
	};
	struct D : B
	{
		void f(int a = 0) override { cout << "D: " << a << endl; }
	};

	struct X
	{
		//const int a = 0; // 不能作为默认参数, 因为在离开类的作用域时 const 变量会失效, 必须要static
		//constexpr int a = 0; // ERROR: 非 static 变量属于类实例, 这与 constexpr 语义矛盾, 必须要static
		//static int a = 0; // ERROR: static 变量必须是 inline 或 const/constexpr
		inline static int a = 0; // inline 解决了之前的版本中必须在类外对 static 变量进行定义的麻烦
		const static int aa = 1;
		void f(int b = sizeof a + aa) { cout << b << endl; } // 默认参数必须是属于静态存储期, 即static
	};

	void main()
	{
		/*unique_ptr<B> p = make_unique<D>();
		p->f();*/

		X x;
		x.f();
	}

};

int main()
{
	_lambda::main();
}
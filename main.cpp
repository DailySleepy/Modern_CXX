#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>

using namespace std;

namespace _array
{
	void main()
	{
		cout << boolalpha << is_same_v<int[1], int[2]> << endl; // 数组是类型, 包含大小
	}
}

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
	void test();
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

		cout << "test: " << endl;
		test();
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
		// virtual 提供运行时多态, 实现函数覆写
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
		unique_ptr<B> p = make_unique<D>();
		p->f();
		// 默认参数: 编译时静态绑定, 由于p是B*类型, 默认参数来自 B:f()
		// 调用函数: 运行时动态绑定, 由于p实际指向的对象是D, 故调用 D:f()

		X x;
		x.f();
	}

};

namespace _forward
{
	template <class _Ty>
	constexpr _Ty&& _forward_(remove_reference_t<_Ty>& _Arg) noexcept
		// 首先必须是 & _Arg, 防止拷贝
		// 其次用 remove_reference_t 人为的实现引用折叠, 因为引用折叠只在类型推导时触发 (如万能引用, using)
		// _Arg 的类型 remove_reference_t<_Ty>& 最终确定的过程不是类型推导
		// 类型推导发生在模板参数 _Ty 上，_Arg 的类型是依赖于 _Ty 推导结果的产物
	{
		return static_cast<_Ty&&>(_Arg); // 引用折叠
	}

	void f(const string& s) { cout << "f左值: " << s << endl; }
	void f(string&& s) { cout << "f右值: " << s << endl; }
	template<typename T>
	void g(T&& arg) // 万能引用, 引用折叠
	{
		//f(arg); // 永远是 f左值
		f(forward<T>(arg));
		// 完美转发: 在转发参数的同时，保留其原始的左值/右值这样的值类别(value category)属性
		// 没有完美转发:
		// 使用 T 导致不必要的拷贝, 使用 T& 又无法接受右值
		// 唯一的方法是为左值右值设计两个函数, 对于右值仍然只能用 const& 来捕获
	}
	void main(void)
	{
		string&& s("Hello");
		f(s); // 调用 f左值, 具名的右值引用本身是一个左值
		f(dynamic_cast<string&&>(s)); // 调用 f右值  ps: dynamic_cast 除了多态类, 还可以从左值到右值引用
		
		// 用 g() 来将参数转发到正确的那一个 f()
		g(s);
		g(move(s));
	}
}

namespace _auto
{
	void main()
	{
		int a = 1;
		auto&& b = ref(a).get();
		b++;
		cout << a << endl;
		cout << b << endl;

		int x = 0;
		auto&& v = cref(x).get();
	}
}

int main()
{
	_forward::main();
}
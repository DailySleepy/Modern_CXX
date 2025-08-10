#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <array>

using namespace std;

namespace _decltype
{
	const int&& g() { return 0; }
	decltype(auto) f() { return g(); }

	void main()
	{
		int x = 1;
		int a[1] = { 1 };
		// 单纯的标识符:
		using T1 = decltype(x); // T
		// 非单纯的标识符:
		// prv -> T
		using T2 = decltype(1);
		using T8 = decltype(&x);
		using T7 = decltype(string("***"));
		// lv -> T&
		using T5 = decltype(("***"));
		using T3 = decltype(a[0]);
		using T4 = decltype((x));
		// xv -> T&&
		using T6 = decltype(move(x));

		// decltype(auto) 完全保留类型
		decltype(auto) v = f();
		auto vv = f();
		decltype(auto) a = "abc";
		auto aa = "abc";

		//decltype(auto) shaders = { "basic.vert", "pbr.vert" };
		// ERROR: 大括号列表本身没有一个可以被 decltype 推导的独立类型
		auto shaders = { "basic.vert", "pbr.vert" };
		const char* shaders[] = { "basic.vert", "pbr.vert" };
		array shaders = { "basic.vert", "pbr.vert" };
	}
}

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
	void main()
	{
		int a[5]{ 1 }; // 只初始化第一个元素为1, 其他为0
		cout << a[1] << endl;

		cout << boolalpha << is_same_v<int[1], int[2]> << endl; // 数组是类型, 包含大小
		// 只在传参, 表达式(除了sizeof, &)中退化为指针

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
	// 完美转发: 在转发参数的同时，保留并传递值类别
	// 核心:
	// 1.万能引用和模板类型推导: 保留值类别, 将左值捕获为左值引用, T推导为U&，将右值捕获为右值引用, T推导为值类型U
	// 2.forward 的有条件转发: 传递值类别, 只将右值引用转为右值, 否则转为左值 (move 无条件转发为右值)
	// 解决痛点: 既用引用/移动代替拷贝, 又用一个函数代替多个重载
	// 使用 T 导致不必要的拷贝, 使用 T& 又无法接受右值
	// C++11前, 唯一的方法是为左值右值设计两个函数, 对于右值仍然只能用 const& 来捕获
	// C++11后(不用完美转发的情况下), 可以右值引用捕获右值, 再move触发移动, 但是仍然需要管理多个函数
{
	template <class T>
	constexpr T&& __forward(remove_reference_t<T>& arg) noexcept
		// 首先必须是 & arg, 防止拷贝
		// 其次用 remove_reference_t 来制造一个非推导上下文
		// 防止编译器通过参数来自动推导T, 这在完美转发中是不应该被允许的, 因为此时参数已经丢失了值类别
		// 于是强制用户在调用 forward 时通过 <> 来指明模板参数 T, 以顺利传递值类别这一信息
		// 虽然去掉 remove_reference 也可以手动使用 <>, 但是这不是强制的, 容易忘记
		// 且当期望转发的类型与参数的类型发生冲突时 (如参数为右值引用, 但是T为左值引用) 会报错
	{
		return static_cast<T&&>(arg); // 引用折叠
		// 根据最初的模板参数 T，而不是 arg 本身的值类别，来决定转换的类型
	}

	template <class T>
	constexpr T&& __forward(remove_reference_t<T>&& _Arg) noexcept // forward 对于右值参数的重载
	{
		cout << "r forward" << endl;
		return static_cast<T&&>(_Arg);
	}

	void f(const string& s) { cout << "f左值: " << s << endl; }
	void f(string&& s) { cout << "f右值: " << s << endl; }
	template<typename T>
	void g(T&& arg) // 万能引用, 引用折叠
	{
		//f(arg); 
		// ERROR: 永远是 f左值, 因为具名的右值引用本身是一个左值
		// 重载决议同时考虑了参数的类型(右值引用)和值类别(左值), 值类别优先级更高
		f(__forward<T>(arg)); // 将参数转发到正确的那一个 f()
	}

	void main()
	{
		string s("Hello");
		g(s);
		g(move(s));

		string&& ss = move(s);
		__forward<string&&>(ss);
	}
}

namespace test
{
	void main()
	{

	}
}

int main()
{
	_array::main();
}
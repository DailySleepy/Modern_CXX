#include <iostream>
#include <vector>
#include <functional>
#include <numeric>
#include <chrono>
#include <array>
#include <ranges>
#include <algorithm>
#include <string>
//import std;

using namespace std;

namespace _decltype
{
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
		auto g = [] -> const int&& { return 0; };
		auto f = [&] -> decltype(auto) { return g(); };
		decltype(auto) v = f();
		auto vv = f();
		decltype(auto) s = "abc";
		auto ss = "abc";

		//decltype(auto) shaders = { "basic.vert", "pbr.vert" };
		// ERROR: 大括号列表本身没有一个可以被 decltype 推导的独立类型
		auto shaders1 = { "basic.vert", "pbr.vert" };
		const char* shaders2[] = { "basic.vert", "pbr.vert" };
		array shaders3 = { "basic.vert", "pbr.vert" };
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

namespace _noexcept
	// 不抛异常保证 
	// 如果一个 noexcept 函数抛出异常，程序会立即调用 std::terminate() 并强制终止
	// 解决了 throw(e...) 的运行时开销(检查异常是否匹配e...)和未定义行为(不匹配时调用 unexpected())
	// 其他异常保证等级:
	// 强保证: 成功或完全无影响(回滚), 如 vector.push_back() 失败, 原有的数据不变, 包括内存地址和大小; 方法如拷贝交换
	// 基本保证: 对象仍然是合法有效状态, 仍然可以访问 vector 的元素和方法, 但不保证元素顺序, size(), 内存; 方法如RAII
	// 无保证: 没有考虑异常安全
{
	void f() noexcept
	// 普通函数默认是 except 的, noexcept 显式指明不会出错
	// noexcept(false) 显式指明可能出错, 可以防止编译器自动推导为 noexcept(true)
	{
		cout << "f is called" << endl;
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

	void main()
	{
		// noexcept 编译期不求值表达式, 不会调用函数
		cout << boolalpha << noexcept(f()) << endl; // true
		C c; cout << boolalpha << noexcept(c.~C()) << endl; // true
	}
}

namespace user_defined_literals
{
	// operator""_后缀名
	auto operator""_km(long double d)
		// 浮点数只接受 long double, 防止重载 (避免复杂的重载决议)
	{
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

namespace _bind
{
	using namespace placeholders;

	int add(int a, int b) { return a + b; }

	struct Printer
	{
		void print(const string& s) { cout << s << endl; }
	};

	void main()
	{
		// 绑定普通函数
		auto add10 = bind(add, _1, 10); // _1 是占位符, 表示新函数调用时传入的第一个参数
		cout << add10(1) << endl;

		// 绑定成员函数
		Printer obj;
		auto printHello = bind(&Printer::print, &obj, "Hello");
		printHello();

		// 但是 lambda 是实现上述功能更好的选择
		auto add10_lambda = [](int i) { return add(i, 1); };
		auto printHello_lambda = [&obj]() { obj.print("Hello"); };
		// lambda 语法更清晰
		// 此外, bind 的返回值是非常复杂的"未命名函数对象", 难以像 lambda 一样直接内联
	}
}

namespace member_pointer
	// 类的非静态成员在类定义中的偏移量, 依赖于具体的类对象
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
};

namespace _invoke
{
	void _invoke()
	{

	}
	void compare_with_bind()
	{
		struct X
		{
			int i = 0;
			void inc() { i++; }
			void print() { cout << "i: " << i << endl; }
		};

		X x;
		x.print();
		cout << "invoke:\n";
		{
			invoke(&X::inc, x);
			x.print();
			invoke(&X::inc, &x);
			x.print();
			invoke(&X::inc, ref(x));
			x.print();
			// 0 1 2 3
			// invoke 是立即调用, Arg&&通用引用, 所以x, &x, ref(x)的结果一样
		}
		x.i = 0;
		cout << "bind:\n";
		{
			auto b1 = bind(&X::inc, x); b1();
			x.print();
			auto b2 = bind(&X::inc, &x); b2();
			x.print();
			auto b3 = bind(&X::inc, ref(x)); b3();
			x.print();
			// 0 0 1 2
			// bind 是创建可调用对象, 延迟执行
			// 将参数都存起来, 这要求使用decay_t来退化类型(与thread相同的要求), 所以传值x不会对i产生影响
		}
	}
	void main()
	{
		_invoke();
		compare_with_bind();
	}
}

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

namespace _move
{
	struct X
	{
		X() {}
		X(const X& o) {};
		X(X&& o) {};
		// move的本质不是移动, 而是无条件转为右值引用, 真正实现移动的是移动构造函数
	};
	X f()
	{
		X x;
		return x; // NRVO 或 移动语义
		// 虽然x是左值, 但是编译器知道x即将销毁, 所以还是将x认为是隐式可移动对象, 自动触发移动语义
		// 不应该使用 move(x), 这会破坏 NRVO
	}
	struct Y
	{
		X x;
		X get() { return move(x); }
		// 这里的x是一个左值, 且不会立即销毁, 所以不是一个隐式可移动对象, 需要手动 move 来触发移动语义
	};

}

namespace _forward
	// 完美转发: 在转发参数的同时，保留并传递值类别
	// 核心:
	// 1.万能引用和模板类型推导: 保留值类别, 将左值捕获为左值引用, T推导为U&，将右值捕获为右值引用, T推导为值类型U
	// 2.forward 的有条件转发: 传递值类别, 只将右值引用转为右值, 否则转为左值 (move 无条件转发为右值)
	// 解决痛点: 既用引用/移动代替拷贝, 又用一个函数代替多个重载
	// 使用 T 导致不必要的拷贝, 使用 T& 又无法接受右值
	// C++11前, 唯一的方法是为左值右值设计两个函数, 对于右值仍然只能用 const& 来捕获
	// C++11后(不用完美转发的情况下), 可以右值引用捕获右值, 再 move 触发移动, 但是仍然需要管理多个函数
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

namespace type_traits
{
	// 1.如何判断类型特征
	template<typename T>
	struct is_int_strict : false_type {};
	template<>
	struct is_int_strict<int> : true_type {};
	template<typename T>
	struct is_int : is_int_strict<remove_cv_t<remove_reference_t<T>>> {};
	// 先移除引用, 再移除 cv: 是 & 修饰 const, 而不是 const 修饰 &
	// 也可以直接用 remove_cvref_t
	template<typename T>
	constexpr bool my_is_int_v{ is_int<T>::value };

	// 2.使用类型特征来选择函数模板
	class IMeasurable
	{
	public:
		virtual double length() const = 0;
	};
	class Vec : public IMeasurable
	{
	public:
		int x, y;
		Vec(int x, int y) : x(x), y(y) {}
		double length() const { return sqrt(x * x + y * y); }
	};
	template<typename T>
	double length(T&& t)
	{
		if constexpr (is_arithmetic_v<T>)
			return abs(t);
		else if constexpr (is_base_of_v<IMeasurable, remove_reference_t<T>>)
			// 引用类型不参与 is_base_of 判断，所以先移除引用
			return t.length();
		return 0;
	}

	void main()
	{
		cout << boolalpha << my_is_int_v<const int&> << endl;

		cout << length(2) << endl;
		cout << length(Vec(3, 4)) << endl;
		const auto v = Vec(3, 4);
		cout << length(v) << endl;
	}
}

namespace SFINAE
{
	namespace test1
	{
		class SomeObj1
		{
		public:
			void Dump2File() const
			{
				std::cout << "dump this object to file" << std::endl;
			}
		};
		class SomeObj2 {};

		template<class T>
		auto DumpObj(const T& t) -> decltype(t.Dump2File())
			// 使用 decltype 来触发 SFINAE
			// 如果 T 没有该成员，则表达式无效，decltype 无法推导类型, 忽略该模板
		{
			t.Dump2File();
		}
		void DumpObj(...)
		{
			std::cout << "the object must have a member function Dump2File" << std::endl;
		}
		void main()
		{
			SomeObj1 obj1;
			DumpObj(obj1);
			SomeObj2 obj2;
			DumpObj(obj2);
		}
	}

	namespace test2
	{
		template<typename, typename = void>
		struct has_type_member : std::false_type {};

		template<typename T>
		struct has_type_member<T, std::void_t<typename T::type>> : std::true_type {};
		// 用 void_t 来将任意类型实例化为 void, 如果实例化失败, 触发 SFINAE

		struct A { using type = int; };
		struct B {};

		void main()
		{
			std::cout << boolalpha << has_type_member<A>::value << std::endl;
			std::cout << boolalpha << has_type_member<B>::value << std::endl;
		}
	}

	void main()
	{
		cout << "test1:" << endl;
		test1::main();
		cout << "test2:" << endl;
		test2::main();
	}
}

namespace CRTP
{
	template<typename T>
	struct Logger
	{
		void printLog(const std::string& message)
		{
			std::cout << "[ID:" << static_cast<T*>(this)->getID() << "] ";
			static_cast<T*>(this)->logMessage(message);
		}
	};

	// 继承以自己为模板参数的基类, 获得通用的方法 printLog
	struct Player : Logger<Player>
	{
		int getID() const { return 101; }
		void logMessage(const std::string& message)
		{
			std::cout << "Player: " << message << std::endl;
		}
	};
	struct Enemy : Logger<Enemy>
	{
		int getID() const { return 202; }
		void logMessage(const std::string& message)
		{
			std::cout << "Enemy: " << message << std::endl;
		}
	};

	void main()
	{
		Player player;
		Enemy enemy;

		player.printLog("spawned");
		enemy.printLog("defeated");

		// vector<unique_ptr<Logger<Player>>> entities;
		// vector<unique_ptr<Logger<Enemy>>>  entities;
		// 代价是无法将二者放在同一个容器中, 因为 Logger<Player> 和 Logger<Enemy> 是不同类型
		// 解决方法是再让 Logger 继承自 ILoggerBase, 虽然又引入了虚函数 virtual printLog, 但是减少了虚函数的调用次数
		// 传统虚函数接口: getID + logMessage 两次虚函数 --> CRTP: printLog 一次虚函数 + (getID + logMessage) 两次静态调用
	}
}

namespace _concept
{
	template<typename T>
	concept AddAble = // 用 requires 来声明 Concept
		requires(T a) // requires 就是返回值为 bool 的常量表达式
	{
		{ a + a } -> same_as<T>;
		requires is_arithmetic_v<T>; // 嵌套使用
	};

	template<typename T>
	concept Divisible = // 直接用布尔常量表达式来声明 Concept
		is_arithmetic_v<T>;

	template<typename T>
	auto add1(T a, T b) requires AddAble<T>;

	template<AddAble T> // 适用于一个 Concept 的简单约束
	auto add2(T a, T b);

	template<typename T>
		requires requires(T a, T b) { { a + b } -> same_as<T>; } // 使用匿名 requires
	auto add3(T a, T b);

	// 省略 template<typename T, typename U> 也可以使用不同类型的参数
	auto add4(AddAble auto a, AddAble auto b); // 当 Concept 只需要一个参数时, 可以直接限制 auto

	template<typename T>
	auto mean(T a, T b) requires AddAble<T>&& Divisible<T>; // 使用多个 Concept

	template<typename T> // Concept 用于类模板
		requires integral<T>&& AddAble<T>
	struct Pair
	{
		T a, b;
		Pair(T a, T b) : a(a), b(b) {};
	};

	template<integral auto N> // Concept 用于 NTTP（非类型模板参数）
	struct Array
	{
		int arr[N];
	};

	void main()
	{
		Pair p(1, 2);
		Array<3> a;
	}
}

import MyModule;
namespace _module
{
	void main()
	{
		cout << moduleVariable << endl;
		ModuleClass mc;
		ModuleFunc();
		SubModuleFunc();
		OtherModuleFunc();
	}
}

namespace _range
{
	struct Enemy
	{
		int health;
		float speed;
		float distanceToBase;
	};
	vector<Enemy> enemies = {
		{100, 15.0f, 50.0f},
		{0, 20.0f, 10.0f},
		{80, 5.0f, 30.0f},
		{100, 12.0f, 80.0f},
		{100, 18.0f, 60.0f},
		{100, 22.0f, 20.0f},
		{100, 11.0f, 45.0f},
		{100, 25.0f, 75.0f}
	};

	void main()
	{
		auto distances = enemies
			| views::filter([](const Enemy& e) { return e.health > 0 && e.speed > 10.0f; })
			// 将元素传入lambda, 保留返回真值的元素
			| views::take(5)
			// 取前5个元素
			| views::transform([](const Enemy& e) { return e.distanceToBase; })
			// 对每一个元素进行操作, 生成一个新的元素序列, 可以转换为其他的类型, 这里是将Enemy序列转为float序列
			| ranges::to<vector>();
			// 从 view 类型转为 vector

		ranges::sort(distances, greater<float>());

		ranges::for_each(distances, [](float dist)
		{
			cout << "Distance: " << dist << endl;
		});
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
	_invoke::main();
}
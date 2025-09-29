#pragma once
#include <iostream>
#include <type_traits>
using namespace std;

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
		inline auto DumpObj(const T& t) -> decltype(t.Dump2File())
		{
			t.Dump2File();
		}
		inline void DumpObj(...)
		{
			std::cout << "the object must have a member function Dump2File" << std::endl;
		}
		inline void main()
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

		struct A { using type = int; };
		struct B {};

		inline void main()
		{
			std::cout << boolalpha << has_type_member<A>::value << std::endl;
			std::cout << boolalpha << has_type_member<B>::value << std::endl;
		}
	}

	inline void main()
	{
		cout << "test1:" << endl;
		test1::main();
		cout << "test2:" << endl;
		test2::main();
	}
}
#pragma once
#include <iostream>
using namespace std;

namespace _noexcept
	// �����쳣��֤ 
	// ���һ�� noexcept �����׳��쳣��������������� std::terminate() ��ǿ����ֹ
	// ����� throw(e...) ������ʱ����(����쳣�Ƿ�ƥ��e...)��δ������Ϊ(��ƥ��ʱ���� unexpected())
	// �����쳣��֤�ȼ�:
	// ǿ��֤: �ɹ�����ȫ��Ӱ��(�ع�), �� vector.push_back() ʧ��, ԭ�е����ݲ���, �����ڴ��ַ�ʹ�С; �����翽������
	// ������֤: ������Ȼ�ǺϷ���Ч״̬, ��Ȼ���Է��� vector ��Ԫ�غͷ���, ������֤Ԫ��˳��, size(), �ڴ�; ������RAII
	// �ޱ�֤: û�п����쳣��ȫ
{
	inline void f() noexcept
	// ��ͨ����Ĭ���� except ��, noexcept ��ʽָ���������
	// noexcept(false) ��ʽָ�����ܳ���, ���Է�ֹ�������Զ��Ƶ�Ϊ noexcept(true)
	{
		cout << "print is called" << endl;
	}

	struct C
	{
		int* p = nullptr;
		C() : p(new int(1)) {}
		C(C& o)
		//�������캯���ǿ��ܳ����, �漰�ڴ涯̬����
		{
			if (o.p) p = new int(*o.p);
		}
		C(C&& o) noexcept
		// �ƶ����캯��ͨ���ǲ�����ִ����, ���漰�ڴ涯̬����(ֻ��ָ�뽻��)
		// ������� noexcept, ��ǿ�쳣��֤��, STL����Ҫ�ƶ�Ԫ��ʱ����˵�Ч�ʵ��µĿ�������
		{
			p = o.p;
			o.p = nullptr;
		}
		~C()
		{
			if (p) delete p;
		}
		// ��������Ĭ���� noexcept ��
		// ����ζ������������Ӧ���׳��쳣, ���ǵ������쳣ʱ���Լ����������д�����쳣
		// ������������� noexcept(false), �����׳��쳣, ����ܵ���˫���쳣:
		// ��һ�������쳣���׳�����ջչ��, �������������ͷ�·���ϵ���ʱ����, �����ʱ�����������׳��쳣, �ͳ�����˫���쳣
		// ��ʱ�����ֱ�� terminate() ǿ����ֹ, ������ʱ����û��������������������, ������Դй¶

		void swap(C& other) noexcept
		{
			std::swap(p, other.p); // ҲҪ�� noexcept
		}
	};

	inline void main()
	{
		// noexcept �����ڲ���ֵ���ʽ, ������ú���
		cout << boolalpha << noexcept(f()) << endl; // true
		C c; cout << boolalpha << noexcept(c.~C()) << endl; // true
	}
}
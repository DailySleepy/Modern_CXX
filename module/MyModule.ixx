module;

#include <iostream>
#define PI 1234
using namespace std;

export module MyModule; // 以上的内容属于全局模块片段，也就是不属于任何模块
// 以下的内容使用 export 标记可以被外部使用

export import :SubModule; // 先导入再导出子模块

export import OtherModule; // 也可以导出其他模块

export string moduleVariable = "this is a module variable";

export struct ModuleClass
{
	ModuleClass()
	{
		cout << "this is a module class" << endl;
	}
};

export void ModuleFunc()
{
	cout << "this is a module func" << endl;
	cout << "my PI is " << PI << endl;
}

void PrivateFunc()
{
	cout << "this is a private func" << endl;
}
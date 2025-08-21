module;

#include <iostream>
#define PI 1234

export module myModule; // 以上的内容都是仅在该文件可见

using namespace std;

export string moduleVariable = "this is a module variable";
export void moduleFunc() // 使用 export 标记可以被外部使用的函数和变量
{
	cout << "this is a module" << endl;
	cout << "my PI is " << PI << endl;
}

void privateFunc()
{
	cout << "this is a private func" << endl;
}
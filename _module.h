#pragma once
#include <iostream>
using namespace std;

import MyModule;
namespace _module
{
	inline void main()
	{
		cout << moduleVariable << endl;
		ModuleClass mc;
		ModuleFunc();
		SubModuleFunc();
		OtherModuleFunc();
	}
}
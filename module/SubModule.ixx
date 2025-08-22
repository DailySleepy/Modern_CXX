module;
#include <iostream>
using namespace std;

export module MyModule:SubModule;

export void SubModuleFunc()
{
	cout << "this is a sub module func" << endl;
}
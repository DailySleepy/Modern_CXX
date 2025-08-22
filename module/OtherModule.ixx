module;
#include <iostream>
using namespace std;

export module OtherModule;

export void OtherModuleFunc()
{
	cout << "this is an other module func" << endl;
}
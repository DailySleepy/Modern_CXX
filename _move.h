#pragma once

namespace _move
{
	struct X
	{
		X() {}
		X(const X& o) {};
		X(X&& o) {};
		// move的本质不是移动, 而是无条件转为右值引用, 真正实现移动的是移动构造函数
	};
	inline X f()
	{
		X x;
		return x; // NRVO 或 移动语义
		// 虽然x是左值, 但是编译器知道x即将销毁, 所以还是将x认为是隐式可移动对象, 自动触发移动语义
		// 不应该使用 move(x), 这会破坏 NRVO
	}
	struct Y
	{
		X x;
		inline X get() { return move(x); }
		// 这里的x是一个左值, 且不会立即销毁, 所以不是一个隐式可移动对象, 需要手动 move 来触发移动语义
	};
}
#pragma once
#include <string>
#include <stdexcept>

class MyClass
{
public:
	MyClass(const int& a = 0) :num(a)
	{
		if (a < 0 || a>255)
			throw std::out_of_range("³¬³ö·¶Î§À²£¡");
	}
	operator int() { return num; }
private:
	std::size_t num;
};

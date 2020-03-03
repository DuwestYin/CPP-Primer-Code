#include "StrBlob.h"
#include <stdexcept>

//列表初始化共享指针，为空。
StrBlob::StrBlob() : data(std::make_shared<std::vector<std::string>>()) {}
//列表初始化共享指针为，初始化列表il（长度不定）
StrBlob::StrBlob(std::initializer_list<std::string> il) :
	data(std::make_shared<std::vector<std::string>>(il)) {}
void StrBlob::check(size_type i, const std::string& msg) const
{
	if (i >= data->size())
		throw std::out_of_range(msg);  //下标越界丢出一个异常
}
std::string& StrBlob::front() const
{
	//先检查向量是不是为空
	check(0, "front on enpty StrBlob");
	return data->front();
}
std::string& StrBlob::back() const
{
	//先检查向量是不是为空
	check(0, "back on enpty StrBlob");
	return data->back();
}
void StrBlob::pop_back()
{
	//先检查向量是不是为空
	check(0, "pop_back on enpty StrBlob");
	data->pop_back();
}


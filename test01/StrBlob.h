#pragma once
#include <vector>
#include <string>
#include <memory>

class StrBlob
{
public:
	using size_type = std::vector<std::string>::size_type; //定义一个新类型，就是字符串数组的长度
	StrBlob();   //默认构造函数
	StrBlob(std::initializer_list<std::string> il);  //输入参数可变化的构造函数
	size_type size() const { return data->size(); }  //返回字符串数组长度，内联函数
	bool empty() const { return data->empty(); }
	void push_back(const std::string& t) { data->push_back(t); }
	void pop_back();
	std::string& front() const;   //返回头个字符串元素的引用
	std::string& back() const;    //
private:
	std::shared_ptr<std::vector<std::string>> data;  //字符串向量的共享指针
	//检查i是否在向量的长度内，msg为引发异常时的输出字符串
	void check(size_type i, const std::string& msg) const;


};

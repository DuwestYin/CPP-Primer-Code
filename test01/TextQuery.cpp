#include "TextQuery.h"
#include <sstream>
#include <algorithm>

TextQuery::TextQuery(std::ifstream& is) :file(new std::vector<std::string>) //初始化shared指针
{
	std::string text;
	while (std::getline(is, text))  //读一行文本
	{
		file->push_back(text);
		int n = file->size() - 1; //当前行号
		std::istringstream line(text);  //对每一行处理
		std::string word; //每一个字
		while (line >> word)
		{
			auto& lines = wm[word];     //每一个字的行号set，为一个shared指针，刚开始lines为空
			if (!lines) //当word第一次遇到，lines为空
				lines.reset(new std::set<line_no>);  //开辟一段内存
			lines->insert(n); //插入当前行号
		}
	}
}

QueryResult TextQuery::query(const std::string& sought) const
{
	//当查询的字不存在时，返回一个shared空指针，在多个类对象之间通用
	static std::shared_ptr<std::set<line_no>> nodata(new std::set<line_no>);
	auto loc = wm.find(sought);
	if (loc == wm.end()) //查询的字不存在
		return QueryResult(sought, nodata, file);
	else
		return QueryResult(sought, loc->second, file);  //返回单词，及对应行号set构建的结果类
}

//或类的eval方法定义
QueryResult OrQuery::eval(const TextQuery& text) const
{
	//得到左右操作数的结果
	auto right = rhs.eval(text), left = lhs.eval(text);
	//新建一个or以后的set，用左操作数的值初始化
	auto ret_lines = std::make_shared<std::set<line_no>>(left.begin(), left.end());
	//插入右操作数的值
	ret_lines->insert(right.begin(), right.end());
	//返回一个新的查询结果对象
	return QueryResult(rep(), ret_lines, left.get_file());
}

//与类的eval方法定义
QueryResult AndQuery::eval(const TextQuery& text) const
{
	//得到左右操作数的结果
	auto right = rhs.eval(text), left = lhs.eval(text);
	//构建一个空的set来存储与后的结果
	auto ret_lines = std::make_shared<std::set<line_no>>();
	//调用库的algorithm头文件中的关于set的内置函数，来执行与操作
	std::set_intersection(left.begin(), left.end(), right.begin(), right.end(),
		std::inserter(*ret_lines, ret_lines->begin()));
	//返回一个新的查询结果对象
	return QueryResult(rep(), ret_lines, left.get_file());
}

//取反类的eval方法定义
QueryResult NotQuery::eval(const TextQuery& text) const
{
	//得到需要取反的查询结果
	auto result = query.eval(text);
	//构建一个空的set来存储取反后的结果
	auto ret_lines = std::make_shared<std::set<line_no>>();
	//得到原来结果行号set的迭代器
	auto beg = result.begin(), end = result.end();
	//得到整个文本的行号数量
	auto sz = result.get_file()->size();
	//从0开始遍历所有行号
	for (rsize_t n = 0; n != sz; ++n)
	{
		//当没有原来结果的编号时
		if (beg == end || *beg != n)
			ret_lines->insert(n);
		else if (beg != end)
			++beg;
	}
	//返回一个新的查询结果对象
	return QueryResult(rep(), ret_lines, result.get_file());
}



//打印出结果对象
std::ostream& print(std::ostream& os, const QueryResult& qr)
{
	os << qr.sought << " 一共出现了 " << qr.lines->size() << " 行" << std::endl;
	for (auto num : *(qr.lines))  //因为lines是指针
		os << "\t(行 " << num + 1 << ")" << *(qr.file->begin() + num) << std::endl;
	return os;
}
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <fstream>
#include <iostream>
#include <iterator>


class QueryResult;  //必须要先做声明，因为后面的类要用
class TextQuery
{
public:
	using line_no = std::vector<std::string>::size_type;    //每个字的行号类型
	TextQuery(std::ifstream&);  //构造函数，输入一个输入文件流
	QueryResult query(const std::string&) const;  //输入一个字来查询，返回结果类型
private:
	//将整个文件保存为一个向量，每个string为一行单词
	//因为需要再结果类中共享整个文件数据，所以用shared指针
	std::shared_ptr<std::vector<std::string>> file;  
	//构造从单词到行的映射，每个单词可以有多个行，用了set
	//set需要在结果中共享，所以用了shares指针
	std::map<std::string, std::shared_ptr<std::set<line_no>>> wm;
};

class QueryResult
{
	//声明一个友元函数来打印结果，因为print要用类里面的数据，所以要为友元
	friend std::ostream& print(std::ostream&, const QueryResult&);
public:
	QueryResult(std::string s, std::shared_ptr<std::set<TextQuery::line_no>> p,
		std::shared_ptr<std::vector<std::string>> f) :sought(s), lines(p), file(f) {}
	auto begin() { return lines->begin(); }
	auto end() { return lines->end(); }
	auto get_file() { return file; }
private:
	std::string sought; //需要查询的字
	std::shared_ptr<std::set<TextQuery::line_no>> lines; //字对应的行号set
	std::shared_ptr<std::vector<std::string>> file;      //整个文本信息
};

std::ostream& print(std::ostream& os, const QueryResult& qr);   //打印查询结果

class Query_base
{
	//抽象基类，没有公有内容，为整个上层类定义了接口框架
	friend class Query;       //Query需要使用该类的私有方法
protected:
	using line_no = TextQuery::line_no;   //行号数据类型
	virtual ~Query_base() = default;
private:
	//返回查询结果，纯虚函数，被子类重载，相当于TextQuery的query()方法
	virtual QueryResult eval(const TextQuery&) const = 0;
	//返回查询结果的字符串描述，纯虚函数，被子类重载，相当于返回QueryResult的sought字符串
	virtual std::string rep() const = 0;
};

class Query
{
	//这个类是整个查询的统一数据类型，任何查询，包括复合查询都用这个类来表示。即：多个Query类复合的结果也是Query类
	//这个类是基本框架抽象基类Query_base的友元类，不是继承。
	//这是该类定义的运算符，定义在外部，不是成员函数。运算的结果也是该类
	friend Query operator~(const Query&);
	friend Query operator|(const Query&, const Query&);
	friend Query operator&(const Query&, const Query&);
public:
	Query(const std::string&);   //使用一个单词来new一个最基本的查询WordQuery，相当于TextQuery。
	//和Query_base的方法同名，但不是继承。没有相关性。
	//其实也是直接调用的Query_base的方法
	QueryResult eval(const TextQuery& t) const{ return q->eval(t); }
	std::string rep() const{ return q->rep(); }
private:
	//私有构造函数，用已有的查询对象构造。其实也就是它自己类型对象的q成员，q指向的对象类型是动态绑定的
	Query(std::shared_ptr<Query_base> query) :q(query) {}
	std::shared_ptr<Query_base> q;    //指向基类的共享指针，但实际的动态类型可能是基类的继承子类
};

/*
//给Query对象定义输出运算符
std::ostream& operator<<(std::ostream& output, const Query& query)
{
	//输出查询的字符串信息。因为rep是公有的，所以没有必要定义为友元
	return output << query.rep();        //不知道为什么有错
}*/

//简单的单个单词查询
class WordQuery :public Query_base
{
	//全部类容都是是私有的，通过Query的接口来使用
	friend class Query;   //Query的公有构造函数，需要使用该类的私有构造函数。
	//私有构造函数，维持一个要查询的单词。
	WordQuery(const std::string& s) :query_word(s) {}
	//对基类Query_base的纯虚函数的重载
	QueryResult eval(const TextQuery& t) const   
	{
		return t.query(query_word);                  //因为该类和TextQuery的功能差不多，直接返回TextQuery的结果即可
	}
	std::string rep() const { return query_word; }
	std::string query_word;    //被查询的单词
};

//Query的公有构造函数，用一个单词来new一个WordQuery对象
//并用Query_base基类的指针q指向。
//构造为内联函数，相当于宏定义，是直接替换代码，不是真正的函数调用。
inline Query::Query(const std::string& s) :q(new WordQuery(s)) {}

//对已有查询结果的取反
class NotQuery :public Query_base
{
	//该类所有元素都是私有的
	//因为Query类中定义的取反操作需要调用该类的私有构造函数
	friend Query operator~(const Query&);
	//私有构造函数，用一个需要查询结果取反的Query对象初始化
	NotQuery(const Query& q) :query(q) {};
	//对基类纯虚函数重载
	QueryResult eval(const TextQuery&) const;    //改变返回的结果
	std::string rep() const { return "~(" + query.rep() + ")"; }  //输出字符串
	Query query;              //维持一个需要取反的已有查询结果
};

//Query的去反运算符定义，输入一个Query对象返回一个取反对象
inline Query operator~(const Query& operand)
{
	//隐式的返回一个用指针构造的Query对象
	return std::shared_ptr<Query_base>(new NotQuery(operand));
}

class BinaryQuery :public Query_base
{
	//为了二元操作（&和|）方便定义抽象类
	//因为，没有重载基类的纯虚函数eval，所以这个类也继承了抽象
protected:
	//用左操作数、右操作数和操作符来构建BinaryQuery对象
	BinaryQuery(const Query& left, const Query& right, std::string s) :
		lhs(left), rhs(right), opSym(s) {}
	//对基类纯虚函数重载
	std::string rep() const
	{
		//返回两个操作数结合的字符串
		return "(" + lhs.rep() + " " + opSym + " " + rhs.rep() + ")"; 
	}
	Query lhs, rhs;     //做操作数和右操作数，都是Query对象
	std::string opSym;  //操作符
};

class AndQuery :public BinaryQuery
{
	//该类所有元素都是私有的
	//因为Query类中定义的与操作需要调用该类的私有构造函数
	friend Query operator&(const Query&, const Query&);
	//构造函数只是调用了基类的构造函数，本类并不新增数据
	AndQuery(const Query& left, const Query& right) :
		BinaryQuery(left, right, "&") {}
	//直接继承基类的rep方法，并重载基类的虚函数eval
	QueryResult eval(const TextQuery&) const;  //改变返回的结果
};

//定义与操作运算符
inline Query operator&(const Query& lhs, const Query& rhs)
{
	return std::shared_ptr<Query_base>(new AndQuery(lhs, rhs));
}

class OrQuery :public BinaryQuery
{
	//该类所有元素都是私有的
	//因为Query类中定义的或操作需要调用该类的私有构造函数
	friend Query operator|(const Query&, const Query&);
	//构造函数只是调用了基类的构造函数，本类并不新增数据
	OrQuery(const Query& left, const Query& right) :
		BinaryQuery(left, right, "|") {}
	//直接继承基类的rep方法，并重载基类的虚函数eval
	QueryResult eval(const TextQuery&) const;   //改变查询的结果
};

//定义或操作运算符
inline Query operator|(const Query& lhs, const Query& rhs)
{
	return std::shared_ptr<Query_base>(new OrQuery(lhs, rhs));
}



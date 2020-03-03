#pragma once
#include <string>
#include <iostream>

class Quote 
{
public:
	Quote() = default;
	Quote(const std::string& book, double sales_price) :
		bookNo(book), price(sales_price) {}
	std::string isbn() const { return bookNo; }      //返回书的ISBN号
	virtual ~Quote() = default;  
private:
	std::string bookNo;   //书的ISBN号
protected:
	double price = 0.0;   //不打折的单价
};

class Disc_quote : public Quote
{
public:
	Disc_quote() = default;
	Disc_quote(const std::string& book, double price, std::size_t qty,
		double disc) :Quote(book, price), quantity(qty), discount(disc) {}
	//纯虚函数，该类为抽象类，只能被继承，不能定义类对象
	//函数功能：输入购买数量得到总价格
	virtual double net_price(std::size_t) const = 0;
protected:
		std::size_t quantity = 0;     //数量阈值
		double discount = 0.0;        //打折率
};

class Bulk_quote : public Disc_quote
{
public:
	Bulk_quote() = default;
	Bulk_quote(const std::string& book, double price, std::size_t qty,
		double disc) :
		Disc_quote(book, price, qty, disc) {}
	//根据购买数量返回总价，包含打折计算
	double net_price(std::size_t n) const override;
};

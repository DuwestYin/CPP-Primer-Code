#pragma once
#include <string>
#include <iostream>

class Quote 
{
public:
	Quote() = default;
	Quote(const std::string& book, double sales_price) :
		bookNo(book), price(sales_price) {}
	std::string isbn() const { return bookNo; }      //�������ISBN��
	virtual ~Quote() = default;  
private:
	std::string bookNo;   //���ISBN��
protected:
	double price = 0.0;   //�����۵ĵ���
};

class Disc_quote : public Quote
{
public:
	Disc_quote() = default;
	Disc_quote(const std::string& book, double price, std::size_t qty,
		double disc) :Quote(book, price), quantity(qty), discount(disc) {}
	//���麯��������Ϊ�����ֻ࣬�ܱ��̳У����ܶ��������
	//�������ܣ����빺�������õ��ܼ۸�
	virtual double net_price(std::size_t) const = 0;
protected:
		std::size_t quantity = 0;     //������ֵ
		double discount = 0.0;        //������
};

class Bulk_quote : public Disc_quote
{
public:
	Bulk_quote() = default;
	Bulk_quote(const std::string& book, double price, std::size_t qty,
		double disc) :
		Disc_quote(book, price, qty, disc) {}
	//���ݹ������������ܼۣ��������ۼ���
	double net_price(std::size_t n) const override;
};

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <fstream>
#include <iostream>
#include <iterator>


class QueryResult;  //����Ҫ������������Ϊ�������Ҫ��
class TextQuery
{
public:
	using line_no = std::vector<std::string>::size_type;    //ÿ���ֵ��к�����
	TextQuery(std::ifstream&);  //���캯��������һ�������ļ���
	QueryResult query(const std::string&) const;  //����һ��������ѯ�����ؽ������
private:
	//�������ļ�����Ϊһ��������ÿ��stringΪһ�е���
	//��Ϊ��Ҫ�ٽ�����й��������ļ����ݣ�������sharedָ��
	std::shared_ptr<std::vector<std::string>> file;  
	//����ӵ��ʵ��е�ӳ�䣬ÿ�����ʿ����ж���У�����set
	//set��Ҫ�ڽ���й�����������sharesָ��
	std::map<std::string, std::shared_ptr<std::set<line_no>>> wm;
};

class QueryResult
{
	//����һ����Ԫ��������ӡ�������ΪprintҪ������������ݣ�����ҪΪ��Ԫ
	friend std::ostream& print(std::ostream&, const QueryResult&);
public:
	QueryResult(std::string s, std::shared_ptr<std::set<TextQuery::line_no>> p,
		std::shared_ptr<std::vector<std::string>> f) :sought(s), lines(p), file(f) {}
	auto begin() { return lines->begin(); }
	auto end() { return lines->end(); }
	auto get_file() { return file; }
private:
	std::string sought; //��Ҫ��ѯ����
	std::shared_ptr<std::set<TextQuery::line_no>> lines; //�ֶ�Ӧ���к�set
	std::shared_ptr<std::vector<std::string>> file;      //�����ı���Ϣ
};

std::ostream& print(std::ostream& os, const QueryResult& qr);   //��ӡ��ѯ���

class Query_base
{
	//������࣬û�й������ݣ�Ϊ�����ϲ��ඨ���˽ӿڿ��
	friend class Query;       //Query��Ҫʹ�ø����˽�з���
protected:
	using line_no = TextQuery::line_no;   //�к���������
	virtual ~Query_base() = default;
private:
	//���ز�ѯ��������麯�������������أ��൱��TextQuery��query()����
	virtual QueryResult eval(const TextQuery&) const = 0;
	//���ز�ѯ������ַ������������麯�������������أ��൱�ڷ���QueryResult��sought�ַ���
	virtual std::string rep() const = 0;
};

class Query
{
	//�������������ѯ��ͳһ�������ͣ��κβ�ѯ���������ϲ�ѯ�������������ʾ���������Query�ิ�ϵĽ��Ҳ��Query��
	//������ǻ�����ܳ������Query_base����Ԫ�࣬���Ǽ̳С�
	//���Ǹ��ඨ�����������������ⲿ�����ǳ�Ա����������Ľ��Ҳ�Ǹ���
	friend Query operator~(const Query&);
	friend Query operator|(const Query&, const Query&);
	friend Query operator&(const Query&, const Query&);
public:
	Query(const std::string&);   //ʹ��һ��������newһ��������Ĳ�ѯWordQuery���൱��TextQuery��
	//��Query_base�ķ���ͬ���������Ǽ̳С�û������ԡ�
	//��ʵҲ��ֱ�ӵ��õ�Query_base�ķ���
	QueryResult eval(const TextQuery& t) const{ return q->eval(t); }
	std::string rep() const{ return q->rep(); }
private:
	//˽�й��캯���������еĲ�ѯ�����졣��ʵҲ�������Լ����Ͷ����q��Ա��qָ��Ķ��������Ƕ�̬�󶨵�
	Query(std::shared_ptr<Query_base> query) :q(query) {}
	std::shared_ptr<Query_base> q;    //ָ�����Ĺ���ָ�룬��ʵ�ʵĶ�̬���Ϳ����ǻ���ļ̳�����
};

/*
//��Query��������������
std::ostream& operator<<(std::ostream& output, const Query& query)
{
	//�����ѯ���ַ�����Ϣ����Ϊrep�ǹ��еģ�����û�б�Ҫ����Ϊ��Ԫ
	return output << query.rep();        //��֪��Ϊʲô�д�
}*/

//�򵥵ĵ������ʲ�ѯ
class WordQuery :public Query_base
{
	//ȫ�����ݶ�����˽�еģ�ͨ��Query�Ľӿ���ʹ��
	friend class Query;   //Query�Ĺ��й��캯������Ҫʹ�ø����˽�й��캯����
	//˽�й��캯����ά��һ��Ҫ��ѯ�ĵ��ʡ�
	WordQuery(const std::string& s) :query_word(s) {}
	//�Ի���Query_base�Ĵ��麯��������
	QueryResult eval(const TextQuery& t) const   
	{
		return t.query(query_word);                  //��Ϊ�����TextQuery�Ĺ��ܲ�ֱ࣬�ӷ���TextQuery�Ľ������
	}
	std::string rep() const { return query_word; }
	std::string query_word;    //����ѯ�ĵ���
};

//Query�Ĺ��й��캯������һ��������newһ��WordQuery����
//����Query_base�����ָ��qָ��
//����Ϊ�����������൱�ں궨�壬��ֱ���滻���룬���������ĺ������á�
inline Query::Query(const std::string& s) :q(new WordQuery(s)) {}

//�����в�ѯ�����ȡ��
class NotQuery :public Query_base
{
	//��������Ԫ�ض���˽�е�
	//��ΪQuery���ж����ȡ��������Ҫ���ø����˽�й��캯��
	friend Query operator~(const Query&);
	//˽�й��캯������һ����Ҫ��ѯ���ȡ����Query�����ʼ��
	NotQuery(const Query& q) :query(q) {};
	//�Ի��ി�麯������
	QueryResult eval(const TextQuery&) const;    //�ı䷵�صĽ��
	std::string rep() const { return "~(" + query.rep() + ")"; }  //����ַ���
	Query query;              //ά��һ����Ҫȡ�������в�ѯ���
};

//Query��ȥ����������壬����һ��Query���󷵻�һ��ȡ������
inline Query operator~(const Query& operand)
{
	//��ʽ�ķ���һ����ָ�빹���Query����
	return std::shared_ptr<Query_base>(new NotQuery(operand));
}

class BinaryQuery :public Query_base
{
	//Ϊ�˶�Ԫ������&��|�����㶨�������
	//��Ϊ��û�����ػ���Ĵ��麯��eval�����������Ҳ�̳��˳���
protected:
	//������������Ҳ������Ͳ�����������BinaryQuery����
	BinaryQuery(const Query& left, const Query& right, std::string s) :
		lhs(left), rhs(right), opSym(s) {}
	//�Ի��ി�麯������
	std::string rep() const
	{
		//����������������ϵ��ַ���
		return "(" + lhs.rep() + " " + opSym + " " + rhs.rep() + ")"; 
	}
	Query lhs, rhs;     //�����������Ҳ�����������Query����
	std::string opSym;  //������
};

class AndQuery :public BinaryQuery
{
	//��������Ԫ�ض���˽�е�
	//��ΪQuery���ж�����������Ҫ���ø����˽�й��캯��
	friend Query operator&(const Query&, const Query&);
	//���캯��ֻ�ǵ����˻���Ĺ��캯�������ಢ����������
	AndQuery(const Query& left, const Query& right) :
		BinaryQuery(left, right, "&") {}
	//ֱ�Ӽ̳л����rep�����������ػ�����麯��eval
	QueryResult eval(const TextQuery&) const;  //�ı䷵�صĽ��
};

//��������������
inline Query operator&(const Query& lhs, const Query& rhs)
{
	return std::shared_ptr<Query_base>(new AndQuery(lhs, rhs));
}

class OrQuery :public BinaryQuery
{
	//��������Ԫ�ض���˽�е�
	//��ΪQuery���ж���Ļ������Ҫ���ø����˽�й��캯��
	friend Query operator|(const Query&, const Query&);
	//���캯��ֻ�ǵ����˻���Ĺ��캯�������ಢ����������
	OrQuery(const Query& left, const Query& right) :
		BinaryQuery(left, right, "|") {}
	//ֱ�Ӽ̳л����rep�����������ػ�����麯��eval
	QueryResult eval(const TextQuery&) const;   //�ı��ѯ�Ľ��
};

//�������������
inline Query operator|(const Query& lhs, const Query& rhs)
{
	return std::shared_ptr<Query_base>(new OrQuery(lhs, rhs));
}



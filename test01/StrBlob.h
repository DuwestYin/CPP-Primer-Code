#pragma once
#include <vector>
#include <string>
#include <memory>

class StrBlob
{
public:
	using size_type = std::vector<std::string>::size_type; //����һ�������ͣ������ַ�������ĳ���
	StrBlob();   //Ĭ�Ϲ��캯��
	StrBlob(std::initializer_list<std::string> il);  //��������ɱ仯�Ĺ��캯��
	size_type size() const { return data->size(); }  //�����ַ������鳤�ȣ���������
	bool empty() const { return data->empty(); }
	void push_back(const std::string& t) { data->push_back(t); }
	void pop_back();
	std::string& front() const;   //����ͷ���ַ���Ԫ�ص�����
	std::string& back() const;    //
private:
	std::shared_ptr<std::vector<std::string>> data;  //�ַ��������Ĺ���ָ��
	//���i�Ƿ��������ĳ����ڣ�msgΪ�����쳣ʱ������ַ���
	void check(size_type i, const std::string& msg) const;


};

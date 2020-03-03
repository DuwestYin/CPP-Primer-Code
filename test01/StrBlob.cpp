#include "StrBlob.h"
#include <stdexcept>

//�б��ʼ������ָ�룬Ϊ�ա�
StrBlob::StrBlob() : data(std::make_shared<std::vector<std::string>>()) {}
//�б��ʼ������ָ��Ϊ����ʼ���б�il�����Ȳ�����
StrBlob::StrBlob(std::initializer_list<std::string> il) :
	data(std::make_shared<std::vector<std::string>>(il)) {}
void StrBlob::check(size_type i, const std::string& msg) const
{
	if (i >= data->size())
		throw std::out_of_range(msg);  //�±�Խ�綪��һ���쳣
}
std::string& StrBlob::front() const
{
	//�ȼ�������ǲ���Ϊ��
	check(0, "front on enpty StrBlob");
	return data->front();
}
std::string& StrBlob::back() const
{
	//�ȼ�������ǲ���Ϊ��
	check(0, "back on enpty StrBlob");
	return data->back();
}
void StrBlob::pop_back()
{
	//�ȼ�������ǲ���Ϊ��
	check(0, "pop_back on enpty StrBlob");
	data->pop_back();
}


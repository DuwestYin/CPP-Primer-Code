#include "TextQuery.h"
#include <sstream>
#include <algorithm>

TextQuery::TextQuery(std::ifstream& is) :file(new std::vector<std::string>) //��ʼ��sharedָ��
{
	std::string text;
	while (std::getline(is, text))  //��һ���ı�
	{
		file->push_back(text);
		int n = file->size() - 1; //��ǰ�к�
		std::istringstream line(text);  //��ÿһ�д���
		std::string word; //ÿһ����
		while (line >> word)
		{
			auto& lines = wm[word];     //ÿһ���ֵ��к�set��Ϊһ��sharedָ�룬�տ�ʼlinesΪ��
			if (!lines) //��word��һ��������linesΪ��
				lines.reset(new std::set<line_no>);  //����һ���ڴ�
			lines->insert(n); //���뵱ǰ�к�
		}
	}
}

QueryResult TextQuery::query(const std::string& sought) const
{
	//����ѯ���ֲ�����ʱ������һ��shared��ָ�룬�ڶ�������֮��ͨ��
	static std::shared_ptr<std::set<line_no>> nodata(new std::set<line_no>);
	auto loc = wm.find(sought);
	if (loc == wm.end()) //��ѯ���ֲ�����
		return QueryResult(sought, nodata, file);
	else
		return QueryResult(sought, loc->second, file);  //���ص��ʣ�����Ӧ�к�set�����Ľ����
}

//�����eval��������
QueryResult OrQuery::eval(const TextQuery& text) const
{
	//�õ����Ҳ������Ľ��
	auto right = rhs.eval(text), left = lhs.eval(text);
	//�½�һ��or�Ժ��set�������������ֵ��ʼ��
	auto ret_lines = std::make_shared<std::set<line_no>>(left.begin(), left.end());
	//�����Ҳ�������ֵ
	ret_lines->insert(right.begin(), right.end());
	//����һ���µĲ�ѯ�������
	return QueryResult(rep(), ret_lines, left.get_file());
}

//�����eval��������
QueryResult AndQuery::eval(const TextQuery& text) const
{
	//�õ����Ҳ������Ľ��
	auto right = rhs.eval(text), left = lhs.eval(text);
	//����һ���յ�set���洢���Ľ��
	auto ret_lines = std::make_shared<std::set<line_no>>();
	//���ÿ��algorithmͷ�ļ��еĹ���set�����ú�������ִ�������
	std::set_intersection(left.begin(), left.end(), right.begin(), right.end(),
		std::inserter(*ret_lines, ret_lines->begin()));
	//����һ���µĲ�ѯ�������
	return QueryResult(rep(), ret_lines, left.get_file());
}

//ȡ�����eval��������
QueryResult NotQuery::eval(const TextQuery& text) const
{
	//�õ���Ҫȡ���Ĳ�ѯ���
	auto result = query.eval(text);
	//����һ���յ�set���洢ȡ����Ľ��
	auto ret_lines = std::make_shared<std::set<line_no>>();
	//�õ�ԭ������к�set�ĵ�����
	auto beg = result.begin(), end = result.end();
	//�õ������ı����к�����
	auto sz = result.get_file()->size();
	//��0��ʼ���������к�
	for (rsize_t n = 0; n != sz; ++n)
	{
		//��û��ԭ������ı��ʱ
		if (beg == end || *beg != n)
			ret_lines->insert(n);
		else if (beg != end)
			++beg;
	}
	//����һ���µĲ�ѯ�������
	return QueryResult(rep(), ret_lines, result.get_file());
}



//��ӡ���������
std::ostream& print(std::ostream& os, const QueryResult& qr)
{
	os << qr.sought << " һ�������� " << qr.lines->size() << " ��" << std::endl;
	for (auto num : *(qr.lines))  //��Ϊlines��ָ��
		os << "\t(�� " << num + 1 << ")" << *(qr.file->begin() + num) << std::endl;
	return os;
}
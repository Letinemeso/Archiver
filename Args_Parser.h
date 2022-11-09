#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include <string>
#include <map>
#include <list>


class Args_Parser final
{
private:
	enum class Mode
	{
		none,
		arg,
		value
	};

public:
	typedef std::list<std::string> Values;
	typedef std::map<std::string, Values> Args;

private:
	Args m_args;

public:
	Args_Parser();
	~Args_Parser();

public:
	void parse(const std::string& _raw_data);

public:
	const Args& args() const;
	const Values* values(const std::string& _key) const;

};


#endif // ARGS_PARSER_H

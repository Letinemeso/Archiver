#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include <string>
#include <string.h>
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

private:
	void parse_usual_args(const std::string& _raw_data);

public:
	Args_Parser();
	~Args_Parser();

public:
	void parse(char** _args, unsigned int _count);

public:
	const Args& args() const;
	const Values* values(const std::string& _key) const;
	const std::string* value(const std::string& _key, unsigned int _index) const;

};


#endif // ARGS_PARSER_H

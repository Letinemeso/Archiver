#include "Args_Parser.h"


void Args_Parser::parse_usual_args(const std::string& _raw_data)
{
	std::string current_arg, current_value;
	std::list<std::string> current_parsed_values;

	Mode mode = Mode::none;

	for(unsigned int i=0; i<_raw_data.size(); ++i)
	{
		char c = _raw_data[i];

		if(c == ' ')
		{
			if(mode == Mode::value)
				current_parsed_values.push_back((std::string&&)current_value);

			mode = Mode::none;
			continue;
		}

		if(mode == Mode::arg)
		{
			current_arg += c;
			continue;
		}

		if(mode == Mode::value)
		{
			current_value += c;
			continue;
		}

		if(c == '-')
		{
			if(current_arg.size() > 0)
				m_args.emplace((std::string&&)current_arg, (Values&&)current_parsed_values);

			current_arg.clear();
			current_value.clear();
			current_parsed_values.clear();

			current_arg += c;
			mode = Mode::arg;
			continue;
		}

		//	start of a value
		if(mode == Mode::none && current_arg.size() > 0)
		{
			current_value += c;
			mode = Mode::value;
			continue;
		}
	}

	if(current_arg.size() > 0)
	{
		if(current_value.size() > 0)
			current_parsed_values.push_back((std::string&&)current_value);
		m_args.emplace((std::string&&)current_arg, (Values&&)current_parsed_values);
	}
}



Args_Parser::Args_Parser()
{

}

Args_Parser::~Args_Parser()
{

}



void Args_Parser::parse(char **_args, unsigned int _count)
{
	m_args.clear();

	std::string usual_args_str;

	for(unsigned int i=0; i<_count; ++i)
	{
		const char* curr_str = _args[i];
		unsigned int length = strlen(curr_str);
		unsigned int equals_pos = 0;
		for(; equals_pos < length; ++equals_pos)
			if(curr_str[equals_pos] == '=')
				break;

		if(equals_pos == length)
		{
			usual_args_str += curr_str;
			usual_args_str += ' ';
			continue;
		}

		std::string arg, value;
		for(unsigned int i=0; i<equals_pos; ++i)
			arg += curr_str[i];
		for(unsigned int i=equals_pos + 1; i < length; ++i)
			value += curr_str[i];

		Args::iterator it = m_args.emplace(arg, Values()).first;
		it->second.push_back(value);
	}

	parse_usual_args(usual_args_str);
}



const Args_Parser::Args& Args_Parser::args() const
{
	return m_args;
}

const Args_Parser::Values* Args_Parser::values(const std::string& _key) const
{
	Args::const_iterator it = m_args.find(_key);
	if(it == m_args.cend())
		return nullptr;

	return &it->second;
}

const std::string* Args_Parser::value(const std::string &_key, unsigned int _index) const
{
	const Values* list = values(_key);
	if(list == nullptr)
		return nullptr;
	if(list->size() <= _index)
		return nullptr;

	Values::const_iterator it = list->cbegin();
	for(unsigned int i=0; i<_index; ++i)
		++it;

	return &(*it);
}



































































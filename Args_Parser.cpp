#include "Args_Parser.h"


Args_Parser::Args_Parser()
{

}

Args_Parser::~Args_Parser()
{

}



void Args_Parser::parse(const std::string& _raw_data)
{
	m_args.clear();

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

	if(current_arg.size() > 0 && (current_value.size() > 0 || current_parsed_values.size() > 0))
	{
		if(current_value.size() > 0)
			current_parsed_values.push_back((std::string&&)current_value);
		m_args.emplace((std::string&&)current_arg, (Values&&)current_parsed_values);
	}
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



































































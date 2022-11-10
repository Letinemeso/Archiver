#include "Archive.h"


void Archive::set_error_message(const std::string &_message)
{
	m_error_message = _message;
}



Archive::Archive()
{

}

Archive::Archive(const Archive& _other)
{
	m_unpacked_data = _other.m_unpacked_data;
	m_packed_data = _other.m_packed_data;
}

Archive::Archive(Archive&& _other)
{
	m_unpacked_data = (Files_Data&&)(_other.m_unpacked_data);
	m_packed_data = (std::string&&)(_other.m_packed_data);
}

Archive::~Archive()
{

}



void Archive::reset()
{
	m_unpacked_data.clear();
	m_packed_data.clear();
	m_error_message.clear();
}


void Archive::add_raw_file_data(const std::string& _file_name, const std::string &_data)
{
	Files_Data::iterator it = m_unpacked_data.find(_file_name);
	if(it != m_unpacked_data.end())
		it->second = _data;
	else
		m_unpacked_data.emplace(_file_name, _data);
}

void Archive::add_raw_file_data(const std::string& _file_name, std::string&& _data)
{
	Files_Data::iterator it = m_unpacked_data.find(_file_name);
	if(it != m_unpacked_data.end())
		it->second = (std::string&&)_data;
	else
		m_unpacked_data.emplace(_file_name, (std::string&&)_data);
}

void Archive::remove_raw_file_data(const std::string &_file_name)
{
	Files_Data::iterator it = m_unpacked_data.find(_file_name);
	if(it == m_unpacked_data.end())
		return;
	m_unpacked_data.erase(it);
}


void Archive::pack()
{
	if(m_unpacked_data.size() == 0)
	{
		set_error_message("No unpacked data specified");
		return;
	}

	std::string data_to_pack;

	//	header
	Files_Data::const_iterator it = m_unpacked_data.cbegin();
	while(it != m_unpacked_data.cend())
	{
		data_to_pack += it->first + '\n';
		data_to_pack += std::to_string(it->second.size()) + '\n';
		++it;
	}
	data_to_pack += '\n';

	//	files_data
	it = m_unpacked_data.cbegin();
	while(it != m_unpacked_data.cend())
	{
		data_to_pack += it->second;
		++it;
	}

	HCoder coder;
	coder.encode(data_to_pack);
	m_packed_data = (std::string&&)(coder.encoded_data());
}


void Archive::unpack(const std::string &_raw_data)
{
	m_unpacked_data.clear();

	HCoder coder;
	coder.decode(_raw_data);

	if(!coder.is_ok())
	{
		set_error_message("Archive data was corrupted beyond restorability");
		return;
	}

	m_packed_data = _raw_data;

	std::string unpacked_raw_data = (std::string&&)(coder.decoded_data());

	using StrPair_List = std::list<std::pair<std::string, std::string>>;
	StrPair_List files_in_order;

	//	parse file names, sizes and files' data offset
	unsigned int offset = 0;
	while(true)
	{
		if(unpacked_raw_data[offset] == '\n')
		{
			++offset;
			break;
		}

		std::string file_name, file_length_as_string;

		//	parse file name
		for(; offset < unpacked_raw_data.size(); ++offset)
		{
			if(unpacked_raw_data[offset] == '\n')
			{
				++offset;
				break;
			}
			file_name += unpacked_raw_data[offset];
		}

		//	parse file size as string
		for(; offset < unpacked_raw_data.size(); ++offset)
		{
			if(unpacked_raw_data[offset] == '\n')
			{
				++offset;
				break;
			}
			file_length_as_string += unpacked_raw_data[offset];
		}

		if(offset == unpacked_raw_data.size())
		{
			m_unpacked_data.clear();
			set_error_message("Format error: incorrect header or no files' data");
			return;
		}

		files_in_order.push_back({});
		StrPair_List::iterator it = files_in_order.end();
		--it;

		it->first = (std::string&&)file_name;
		it->second.resize(std::stoi(file_length_as_string));
	}

	StrPair_List::iterator it = files_in_order.begin();
	while(it != files_in_order.end())
	{
		unsigned int file_end = offset + it->second.size();
		if(file_end > unpacked_raw_data.size())
		{
			m_unpacked_data.clear();
			set_error_message("Format error: length of file \"" + it->first + "\" specified in header exceeds archive's size");
			return;
		}

		for(; offset < file_end; ++offset)
			it->second[it->second.size() - (file_end - offset)] = unpacked_raw_data[offset];

		++it;
	}

	it = files_in_order.begin();
	while(it != files_in_order.end())
	{
		m_unpacked_data.emplace((std::string&&)it->first, (std::string&&)it->second);
		++it;
	}
}

void Archive::unpack(std::string&& _raw_data)
{
	HCoder coder;
	coder.decode(_raw_data);

	if(!coder.is_ok())
	{
		set_error_message("Archive data was corrupted beyond restorability");
		return;
	}

	m_packed_data = (std::string&&)(_raw_data);

	std::string unpacked_raw_data = (std::string&&)(coder.decoded_data());

	using StrPair_List = std::list<std::pair<std::string, std::string>>;
	StrPair_List files_in_order;

	//	parse file names, sizes and files' data offset
	unsigned int offset = 0;
	while(true)
	{
		if(unpacked_raw_data[offset] == '\n')
		{
			++offset;
			break;
		}

		std::string file_name, file_length_as_string;

		//	parse file name
		for(; offset < unpacked_raw_data.size(); ++offset)
		{
			if(unpacked_raw_data[offset] == '\n')
			{
				++offset;
				break;
			}
			file_name += unpacked_raw_data[offset];
		}

		//	parse file size as string
		for(; offset < unpacked_raw_data.size(); ++offset)
		{
			if(unpacked_raw_data[offset] == '\n')
			{
				++offset;
				break;
			}
			file_length_as_string += unpacked_raw_data[offset];
		}

		if(offset == unpacked_raw_data.size())
		{
			m_unpacked_data.clear();
			set_error_message("Format error: incorrect header or no files' data");
			return;
		}

		files_in_order.push_back({});
		StrPair_List::iterator it = files_in_order.end();
		--it;

		it->first = (std::string&&)file_name;
		it->second.resize(std::stoi(file_length_as_string));
	}

	StrPair_List::iterator it = files_in_order.begin();
	while(it != files_in_order.end())
	{
		unsigned int file_end = offset + it->second.size();
		if(file_end > unpacked_raw_data.size())
		{
			m_unpacked_data.clear();
			set_error_message("Format error: length of file \"" + it->first + "\" specified in header exceeds archive's size");
			return;
		}

		for(; offset < file_end; ++offset)
			it->second[it->second.size() - (file_end - offset)] = unpacked_raw_data[offset];

		++it;
	}

	it = files_in_order.begin();
	while(it != files_in_order.end())
	{
		m_unpacked_data.emplace((std::string&&)it->first, (std::string&&)it->second);
		++it;
	}
}



std::string& Archive::packed_data()
{
	return m_packed_data;
}

Archive::Files_Data& Archive::unpacked_data()
{
	return m_unpacked_data;
}

const std::string& Archive::packed_data() const
{
	return m_packed_data;
}

const Archive::Files_Data& Archive::unpacked_data() const
{
	return m_unpacked_data;
}

const std::string* Archive::unpacked_data(const std::string& _file_name) const
{
	Files_Data::const_iterator it = m_unpacked_data.find(_file_name);
	if(it == m_unpacked_data.cend())
		return nullptr;

	return &it->second;
}


bool Archive::is_ok() const
{
	return m_error_message.size() == 0;
}

const std::string& Archive::error() const
{
	return m_error_message;
}






































































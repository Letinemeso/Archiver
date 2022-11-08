#include "Archive.h"


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
	m_unpacked_data = (std::list<File_Data>&&)(_other.m_unpacked_data);
	m_packed_data = (std::string&&)(_other.m_packed_data);
}

Archive::~Archive()
{

}



void Archive::reset()
{
	m_unpacked_data.clear();
	m_packed_data.clear();
	m_error_log.clear();
	m_errors_count = 0;
}


void Archive::add_raw_file_data(const std::string& _file_name, const std::string &_data)
{
	m_unpacked_data.push_back({_file_name, _data});
}

void Archive::add_raw_file_data(const std::string& _file_name, std::string&& _data)
{
	m_unpacked_data.push_back({_file_name, (std::string&&)_data});
}


void Archive::pack()
{
	if(m_unpacked_data.size() == 0)
	{
		append_error_message("No unpacked data specified");
		return;
	}

	std::string data_to_pack;

	//	header
	std::list<File_Data>::const_iterator it = m_unpacked_data.cbegin();
	while(it != m_unpacked_data.cend())
	{
		data_to_pack += it->file_name + '\n';
		data_to_pack += std::to_string(it->content.size()) + '\n';
		++it;
	}
	data_to_pack += '\n';

	//	files_data
	it = m_unpacked_data.cbegin();
	while(it != m_unpacked_data.cend())
	{
		data_to_pack += it->content;
		++it;
	}

	HCoder coder;
	coder.encode(data_to_pack);
	m_packed_data = (std::string&&)(coder.encoded_data());
}


void Archive::unpack(const std::string &_raw_data)
{
	m_packed_data = _raw_data;

	HCoder coder;
	coder.decode(m_packed_data);
	std::string unpacked_raw_data = (std::string&&)(coder.decoded_data());

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
			append_error_message("Format error: incorrect header or no files' data");
			return;
		}

		File_Data fd(file_name, "");
		fd.content.resize(std::stoi(file_length_as_string));

		m_unpacked_data.push_back((File_Data&&)fd);
	}

	std::list<File_Data>::iterator it = m_unpacked_data.begin();
	while(it != m_unpacked_data.end())
	{
		unsigned int file_end = offset + it->content.size();
		if(file_end > unpacked_raw_data.size())
		{
			m_unpacked_data.clear();
			append_error_message("Format error: length of file \"" + it->file_name + "\" specified in header exceeds archive's size");
			return;
		}

		for(; offset < file_end; ++offset)
			it->content[file_end - offset] = unpacked_raw_data[offset];

		++it;
	}
}

void Archive::unpack(std::string&& _raw_data)
{
	m_packed_data = (std::string&&)(_raw_data);

	HCoder coder;
	coder.decode(m_packed_data);
	std::string unpacked_raw_data = (std::string&&)(coder.decoded_data());

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
			append_error_message("Format error: incorrect header or no files' data");
			return;
		}

		File_Data fd(file_name, "");
		fd.content.resize(std::stoi(file_length_as_string));

		m_unpacked_data.push_back((File_Data&&)fd);
	}

	std::list<File_Data>::iterator it = m_unpacked_data.begin();
	while(it != m_unpacked_data.end())
	{
		unsigned int file_end = offset + it->content.size();
		if(file_end > unpacked_raw_data.size())
		{
			m_unpacked_data.clear();
			append_error_message("Format error: length of file \"" + it->file_name + "\" specified in header exceeds archive's size");
			return;
		}

		for(; offset < file_end; ++offset)
			it->content[file_end - offset] = unpacked_raw_data[offset];

		++it;
	}
}


void Archive::append_error_message(const std::string &_message)
{
	m_error_log += std::to_string(m_errors_count + 1) + _message + '\n';
	++m_errors_count;
}



const std::string& Archive::packed_data() const
{
	return m_packed_data;
}

const std::list<Archive::File_Data>& Archive::unpacked_data() const
{
	return m_unpacked_data;
}


bool Archive::is_ok() const
{
	return m_errors_count == 0;
}

const std::string& Archive::error_log() const
{
	return m_error_log;
}






































































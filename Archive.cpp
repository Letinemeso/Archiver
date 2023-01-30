#include "Archive.h"


void Archive::set_error_message(const std::string &_message)
{
	m_error_message = _message;
}


std::string Archive::M_parse_value_before_slash(const std::string &_str, unsigned int _offset)
{
	unsigned int i = _offset;
	for(; i < _str.size(); ++i)
		if(_str[i] == '/')
			break;
	return _str.substr(_offset, i - _offset);
}


void Archive::M_unpack_to_temporary_file()
{
	if(!m_packed_file.exists())
		return;

	m_temporary_unpacked_file.set_path(m_packed_file.get_path() + ".temp");
	m_temporary_unpacked_file.clear();

	HCoder coder;

	unsigned int raw_offset = 0;
	std::string chunk = m_packed_file.extract_block(raw_offset, m_read_chunk_size);
	while(chunk.size() > 0)
	{
		coder.decode(chunk);
		if(!coder.is_ok())
		{
			set_error_message("Archive data was corrupted beyond restorability");
			std::remove(m_temporary_unpacked_file.get_path().c_str());
			return;
		}
		m_temporary_unpacked_file.append_block(coder.decoded_data());
		raw_offset += m_read_chunk_size;
		chunk = m_packed_file.extract_block(raw_offset, m_read_chunk_size);
	}
}

void Archive::M_parse_content()
{
	std::string header_length_str = m_temporary_unpacked_file.extract_block(0, 10);	//	10 - max length of unsigned int number as string
	for(unsigned int i=0; i<10; ++i)
	{
		if(header_length_str[i] == '\n')
		{
			header_length_str = header_length_str.substr(0, i);
			break;
		}
	}
	unsigned int header_length = std::stoi(header_length_str);

	std::string header_str = m_temporary_unpacked_file.extract_block(header_length_str.size() + 1, header_length);

	m_content.clear();

	std::string chunk;
	unsigned int offset = 0;
	unsigned int file_offset = header_length_str.size() + 1 + header_length;
	while(offset < header_length)
	{
		std::string name, size_str;

		name = M_parse_value_before_slash(header_str, offset);
		offset += name.size() + 1;
		size_str = M_parse_value_before_slash(header_str, offset);
		offset += size_str.size() + 1;
		const unsigned int size = std::stoi(size_str);

		File_Data data;
		data.file = m_temporary_unpacked_file;
		data.offset = file_offset;
		data.size = size;
		data.file_name = name;

		file_offset += size;

		m_content.push_back(data);
	}
}



Archive::Archive(Archive&& _other)
{
	m_packed_file = (File&&)_other.m_packed_file;
	m_temporary_unpacked_file = (File&&)_other.m_temporary_unpacked_file;
	m_content = (Content_t&&)_other.m_content;
}

Archive::Archive(const std::string& _path)
{
	m_packed_file.set_path(_path);

	if(!m_packed_file.exists())
		m_packed_file.clear();

	m_temporary_unpacked_file.set_path(m_packed_file.get_path() + ".temp");
	m_temporary_unpacked_file.clear();

	if(m_packed_file.length() == 0)
		return;

	M_unpack_to_temporary_file();
	M_parse_content();
}

Archive::~Archive()
{
	if(m_temporary_unpacked_file.exists())
		std::remove(m_temporary_unpacked_file.get_path().c_str());
}



void Archive::append_files(const Content_t& _what)
{
	if(_what.size() == 0)
	{
		set_error_message("No data specified");
		return;
	}

	m_packed_file.clear();

	std::rename(m_temporary_unpacked_file.get_path().c_str(), (m_temporary_unpacked_file.get_path() + ".old").c_str());
	File temp_new(m_temporary_unpacked_file.get_path());
	m_temporary_unpacked_file.set_path(m_temporary_unpacked_file.get_path() + ".old");
	temp_new.clear();

	std::string header_str;
	for(auto it = m_content.cbegin(); it != m_content.cend(); ++it)
	{
		header_str += it->file_name;
		header_str += '/';
		header_str += std::to_string(it->size);
		header_str += '/';
	}
	for(auto it = _what.cbegin(); it != _what.cend(); ++it)
	{
		header_str += it->file_name;
		header_str += '/';
		header_str += std::to_string(it->size);
		header_str += '/';
	}

	temp_new.append_block(std::to_string(header_str.size()) + "\n");
	temp_new.append_block(header_str);

	for(auto it = m_content.cbegin(); it != m_content.cend(); ++it)
		temp_new.append_block_from_other(m_temporary_unpacked_file, it->offset, it->size);

	for(auto it = _what.cbegin(); it != _what.cend(); ++it)
		temp_new.append_block_from_other(it->file, it->offset, it->size);

	std::remove(m_temporary_unpacked_file.get_path().c_str());
	m_temporary_unpacked_file = temp_new;

	HCoder coder;

	unsigned int raw_offset = 0;
	std::string chunk = m_temporary_unpacked_file.extract_block(raw_offset, m_write_chunk_size);
	while(chunk.size() > 0)
	{
		coder.encode(chunk);
		m_packed_file.append_block(coder.encoded_data());
		raw_offset += m_write_chunk_size;
		chunk = m_temporary_unpacked_file.extract_block(raw_offset, m_write_chunk_size);
	}

	M_parse_content();
}


void Archive::unpack(const std::list<std::string>& _files_to_unpack)
{
	auto contains_file = [](const std::list<std::string>& _list, const std::string& _file_name)->bool
	{
		for(const auto& file : _list)
			if(file == _file_name)
				return true;
		return false;
	};

	for(auto it = m_content.cbegin(); it != m_content.cend(); ++it)
	{
		if(_files_to_unpack.size() > 0)
		{
			if(!contains_file(_files_to_unpack, it->file_name))
				continue;
		}

		File file(it->file_name);
		file.clear();

		file.append_block_from_other(it->file, it->offset, it->size);
	}
}

void Archive::exclude_files(const std::list<std::string>& _files_to_exclude)
{
	if(_files_to_exclude.size() == 0)
	{
		set_error_message("No data specified");
		return;
	}

	if(_files_to_exclude.size() == m_content.size())
	{
		std::remove(m_packed_file.get_path().c_str());
		std::remove(m_temporary_unpacked_file.get_path().c_str());

		m_packed_file.set_path("");
		m_temporary_unpacked_file.set_path("");

		m_content.clear();

		return;
	}

	auto contains_file = [](const std::list<std::string>& _list, const std::string& _file_name)->bool
	{
		for(const auto& file : _list)
			if(file == _file_name)
				return true;
		return false;
	};

	m_packed_file.clear();

	std::rename(m_temporary_unpacked_file.get_path().c_str(), (m_temporary_unpacked_file.get_path() + ".old").c_str());
	File temp_new(m_temporary_unpacked_file.get_path());
	m_temporary_unpacked_file.set_path(m_temporary_unpacked_file.get_path() + ".old");
	temp_new.clear();

	std::string header_str;
	for(auto it = m_content.cbegin(); it != m_content.cend(); ++it)
	{
		if(contains_file(_files_to_exclude, it->file_name))
			continue;

		header_str += it->file_name;
		header_str += '/';
		header_str += std::to_string(it->size);
		header_str += '/';
	}

	temp_new.append_block(std::to_string(header_str.size()) + "\n");
	temp_new.append_block(header_str);

	for(auto it = m_content.cbegin(); it != m_content.cend(); ++it)
	{
		if(contains_file(_files_to_exclude, it->file_name))
			continue;
		temp_new.append_block_from_other(m_temporary_unpacked_file, it->offset, it->size);
	}

	std::remove(m_temporary_unpacked_file.get_path().c_str());
	m_temporary_unpacked_file = temp_new;

	HCoder coder;

	unsigned int raw_offset = 0;
	std::string chunk = m_temporary_unpacked_file.extract_block(raw_offset, m_write_chunk_size);
	while(chunk.size() > 0)
	{
		coder.encode(chunk);
		m_packed_file.append_block(coder.encoded_data());
		raw_offset += m_write_chunk_size;
		chunk = m_temporary_unpacked_file.extract_block(raw_offset, m_write_chunk_size);
	}

	M_parse_content();
}


void Archive::clear()
{
	m_temporary_unpacked_file.clear();
	m_packed_file.clear();
	m_content.clear();
}



bool Archive::contains(const std::string &_file_name) const
{
	for(const auto& file : m_content)
		if(file.file_name == _file_name)
			return true;
	return false;
}






































































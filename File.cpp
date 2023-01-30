#include "File.h"


File::File()
{

}

File::File(const File& _other)
	: m_path(_other.m_path)
{

}

File::File(File&& _other)
	: m_path((std::string&&)_other.m_path)
{

}

void File::operator=(const File& _other)
{
	m_path = _other.m_path;
}

void File::operator=(File&& _other)
{
	m_path = (std::string&&)_other.m_path;
}

File::File(const std::string& _path)
	: m_path(_path)
{

}



void File::set_path(const std::string &_path)
{
	m_path = _path;
}

const std::string& File::get_path() const
{
	return m_path;
}



std::string File::extract_block(unsigned int _offset, unsigned int _size) const
{
	std::ifstream file(m_path, std::ifstream::binary);

	if(length() <= _offset)
		return "";

	char* buffer = new char[_size];
	file.seekg(_offset, std::ifstream::beg);
	unsigned int read_amount = file.readsome(buffer, _size);

	std::string result;
	result.resize(read_amount);

	for(unsigned int i=0; i<read_amount; ++i)
		result[i] = buffer[i];

	delete[] buffer;
	file.close();

	return result;
}


void File::clear()
{
	std::ofstream file(m_path, std::ofstream::trunc);
	if(file.is_open())
		file.close();
}

void File::append_block(const std::string& _block)
{
	std::ofstream file(m_path, std::ofstream::binary | std::ofstream::app);
	if(!file.is_open())
		return;

	file << _block;

	file.close();
}


void File::append_block_from_other(const File &_other, unsigned int _offset, unsigned int _size)
{
	if(_size == 0)
		_size = _other.length();

	unsigned int offset = _offset;
	while(offset < _size + _offset)
	{
		unsigned int already_read = offset - _offset;
		unsigned int left_to_read = _size - already_read;
		unsigned int bytes_to_read = 1024 < left_to_read ? 1024 : left_to_read;

		append_block(_other.extract_block(offset, bytes_to_read));

		offset += bytes_to_read;
	}
}



bool File::exists() const
{
	std::ifstream file(m_path, std::ifstream::binary);
	if(!file.is_open())
		return false;
	file.close();
	return true;
}

unsigned int File::length() const
{
	std::ifstream file(m_path, std::ifstream::binary);
	if(!file.is_open())
		return 0;

	file.seekg(0, std::ifstream::end);
	unsigned int length = file.tellg();
	file.close();

	return length;
}

#include "File.h"


File::File(const std::string& _path)
	: m_path(_path)
{

}



std::string File::extract_block(unsigned int _offset, unsigned int _size) const
{
	std::ifstream file(m_path, std::ifstream::binary);

	char* buffer = new char[_size];
	file.seekg(_offset, std::ifstream::beg);
	unsigned int read_amount = file.readsome(buffer, _size);

	std::string result = buffer;
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

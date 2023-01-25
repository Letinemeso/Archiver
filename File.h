#ifndef FILE_H
#define FILE_H

#include <fstream>
#include <string>

class File
{
private:
	std::string m_path;

public:
	File(const std::string& _path);

public:
	std::string extract_block(unsigned int _offset, unsigned int _size) const;

	void clear();
	void append_block(const std::string& _block);

public:
	bool exists() const;
	unsigned int length() const;

};

#endif // FILE_H

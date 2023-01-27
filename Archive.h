#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <map>
#include <list>
#include <string>

#include "HCoder.h"
#include "File.h"


class Archive final
{
public:
	struct File_Data
	{
		File file;
		unsigned int offset = 0;
		unsigned int size = 0;
		std::string file_name;
	};

	typedef std::list<File_Data> Content_t;

private:
	File m_packed_file;
	File m_temporary_unpacked_file;

	Content_t m_content;

	static constexpr unsigned int m_write_chunk_size = 11 * 1;	//	hamming encoding uses additional 5 bits for every 11 bits of real data
	static constexpr unsigned int m_read_chunk_size = 16 * 1;

private:
	std::string m_error_message;

private:
	void set_error_message(const std::string& _message);

private:
	std::string M_parse_value_before_slash(const std::string& _str, unsigned int _offset);

	void M_unpack_to_temporary_file();
	void M_parse_content();

	void M_copy_chunk_to_file(const File& _from, unsigned int _offset, unsigned int _chink_size, File& _to);

public:
	Archive(Archive&& _other);
	Archive(const std::string& _path);
	~Archive();

public:
	void append_files(const Content_t& _what);
	void unpack();

public:
	inline const Content_t& content() const { return m_content; };

	inline bool is_ok() const { return m_error_message.size() == 0; }
	inline const std::string& error() const { return m_error_message; }

};


#endif // ARCHIVE_H

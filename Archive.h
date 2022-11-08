#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <list>
#include <string>

#include "HCoder.h"


class Archive final
{
public:
//	struct Stub final
//	{
//		std::list<std::string> files_paths;
//		operator std::list<std::string>&() { return files_paths; }
//	};

	typedef std::list<std::string> Files_Paths;

public:
	struct File_Data final
	{
		std::string file_name;
		std::string content;

		File_Data(const std::string& _file_name, const std::string& _content) : file_name(_file_name), content(_content) { }
		File_Data(const std::string& _file_name, std::string&& _content) : file_name(_file_name), content((std::string&&)_content) { }
	};

private:
	std::list<File_Data> m_unpacked_data;
	std::string m_packed_data;

	std::string m_error_message;

private:
	void set_error_message(const std::string& _message);

public:
	Archive();
	Archive(const Archive& _other);
	Archive(Archive&& _other);
	~Archive();

public:
	void reset();

	void add_raw_file_data(const std::string& _file_name, const std::string& _data);
	void add_raw_file_data(const std::string& _file_name, std::string&& _data);
	void pack();

	void unpack(const std::string& _raw_data);
	void unpack(std::string&& _raw_data);

public:
	const std::string& packed_data() const;
	const std::list<File_Data>& unpacked_data() const;

	bool is_ok() const;
	const std::string& error() const;

};


#endif // ARCHIVE_H

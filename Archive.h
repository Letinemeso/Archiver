#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <map>
#include <list>
#include <string>

#include "HCoder.h"


class Archive final
{
public:
	typedef std::map<std::string, std::string> Files_Data;

private:
	Files_Data m_unpacked_data;
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
	void remove_raw_file_data(const std::string& _file_name);
	void pack();

	void unpack(const std::string& _raw_data);
	void unpack(std::string&& _raw_data);

public:
	std::string& packed_data();
	Files_Data& unpacked_data();
	const std::string& packed_data() const;
	const Files_Data& unpacked_data() const;
	const std::string* unpacked_data(const std::string& _file_name) const;

	bool is_ok() const;
	const std::string& error() const;

};


#endif // ARCHIVE_H

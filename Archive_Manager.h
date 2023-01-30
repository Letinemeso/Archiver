#ifndef ARCHIVE_MANAGER_H
#define ARCHIVE_MANAGER_H

#include <fstream>
#include <list>

#include "Archive.h"


class Archive_Manager final
{
public:
	typedef std::list<std::string> Files_Paths;

private:
	std::string m_error_log;

private:
	void clear_error_log();
	void append_error_message(const std::string& _message);

private:
	std::string parse_file_name(const std::string& _name_w_path) const;

public:
	Archive_Manager() { }
	~Archive_Manager() { }

public:
	bool pack_files(const Files_Paths& _files_to_pack, const std::string& _archive_name);
	bool append_files(const std::string& _append_to, const Files_Paths& _files_to_append);
	bool exclude_files(const std::string& _exclude_from, const Files_Paths& _files_to_exclude);
	bool merge_archives(const std::string& _save_to, const Files_Paths& _archives);
	bool unpack_files(const std::string& _archive, const Files_Paths& _files_to_unpack);

	std::list<std::string> get_archive_content(const std::string& _archive);

	const std::string& error_log() const;

};


#endif // ARCHIVE_MANAGER_H

#ifndef ARCHIVE_MANAGER_H
#define ARCHIVE_MANAGER_H

#include <fstream>

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
	Archive* pack_files(const Files_Paths& _files_to_pack);
	bool append_files(Archive& _append_to, const Files_Paths& _files_to_append);
	bool exclude_files(Archive& _exclude_from, const Files_Paths& _files_to_exclude);
	Archive* merge_archives(const Files_Paths& _archives);
	bool unpack_files(const Archive& _archive, const std::string& _where);
	bool unpack_files(const Archive& _archive, const Files_Paths& _files_to_unpack);
	Archive* load(const std::string& _file_name);
	bool save(const Archive& _archive, const std::string& _file_name);

	const std::string& error_log() const;

};


#endif // ARCHIVE_MANAGER_H

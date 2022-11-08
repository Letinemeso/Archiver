#ifndef ARCHIVE_MANAGER_H
#define ARCHIVE_MANAGER_H

#include <fstream>

#include "Archive.h"


class Archive_Manager final
{
private:
	std::string m_error_log;
	unsigned int m_errors_count = 0;

private:
	void clear_error_log();
	void append_error_message(const std::string& _message);

public:
	Archive_Manager() { }
	~Archive_Manager() { }

public:
	Archive* pack_files(const Archive::Files_Paths& _files_to_pack);
	bool unpack_files(const Archive& _archive, const std::string& _where);
	Archive* load(const std::string& _file_name);
	bool save(const Archive& _archive, const std::string& _file_name);

	const std::string& error_log() const;

};


#endif // ARCHIVE_MANAGER_H

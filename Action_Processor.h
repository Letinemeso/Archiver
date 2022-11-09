#ifndef ACTION_PROCESSOR_H
#define ACTION_PROCESSOR_H

#include "Args_Parser.h"
#include "Archive_Manager.h"


class Action_Processor final
{
private:
	void create(const std::string& _archive_name, const Archive::Files_Paths& _paths) const;
	void print_files_in_archive(const std::string& _archive_name) const;
	void extract_files(const std::string& _archive_name) const;
	void extract_files(const std::string& _archive_name, const Archive::Files_Paths& _paths) const;
	void append(const std::string& _archive_name, const Archive::Files_Paths& _paths) const;
	void exclude(const std::string& _archive_name, const Archive::Files_Paths& _paths) const;
	void concatenate(const std::string& _archive_name_1, const std::string& _archive_name_2) const;

public:
	void process(const Args_Parser& _args) const;

};


#endif // ACTION_PROCESSOR_H

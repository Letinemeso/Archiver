#ifndef ACTION_PROCESSOR_H
#define ACTION_PROCESSOR_H

#include <iostream>
#include <vector>

#include "Args_Parser.h"
#include "Archive_Manager.h"


class Action_Processor final
{
private:
	std::vector<std::pair<std::string, std::string>> m_possible_commands;

public:
	Action_Processor();
	~Action_Processor();

public:
	void process(const Args_Parser& _args) const;

private:
	void create(const std::string& _archive_name, const Archive_Manager::Files_Paths& _paths) const;
	void print_files_in_archive(const std::string& _archive_name) const;
	void extract_files(const std::string& _archive_name, const Archive_Manager::Files_Paths& _paths) const;
	void append(const std::string& _archive_name, const Archive_Manager::Files_Paths& _paths) const;
	void exclude(const std::string& _archive_name, const Archive_Manager::Files_Paths& _paths) const;
	void merge(const std::string& _result_archive_name, const Archive_Manager::Files_Paths& _paths) const;
	void print_help_message() const;

};


#endif // ACTION_PROCESSOR_H

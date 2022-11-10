#include "Action_Processor.h"


Action_Processor::Action_Processor()
{
	m_possible_commands.resize(7);
	m_possible_commands[0] = {"-c", "--create"};
	m_possible_commands[1] = {"-l", "--list"};
	m_possible_commands[2] = {"-x", "--extract"};
	m_possible_commands[3] = {"-a", "--append"};
	m_possible_commands[4] = {"-d", "--delete"};
	m_possible_commands[5] = {"-A", "--concatenate"};
	m_possible_commands[6] = {"-h", "--help"};
}

Action_Processor::~Action_Processor()
{

}



void Action_Processor::process(const Args_Parser &_args) const
{
	const Args_Parser::Values* values = nullptr;

	int command = -1;
	unsigned int commands_found__check = 0;

	for(int i=0; i<m_possible_commands.size(); ++i)
	{
		const Args_Parser::Values* maybe_values = _args.values(m_possible_commands[i].first);
		if(!maybe_values)
			maybe_values = _args.values(m_possible_commands[i].second);
		if(maybe_values)
		{
			++commands_found__check;
			command = i;
			values = maybe_values;
		}
	}

	if(commands_found__check == 0)
	{
		std::cout << "Error: no existing command is found. Use -h/--help to see list of availible commands\n\n";
		return;
	}

	if(commands_found__check > 1)
	{
		std::cout << "Error: several commands found. Please choose only one command\n\n";
		return;
	}

	if(command == 6)
	{
		print_help_message();
		return;
	}

	const std::string* archive_name = _args.value("-f", 0);
	if(!archive_name)
		archive_name = _args.value("--file", 0);
	if(!archive_name)
	{
		std::cout << "Error: Not found archive file's path/name\n";
		return;
	}

	switch (command) {
	case(0):
		create(*archive_name, *values);
		return;
	case(1):
		print_files_in_archive(*archive_name);
		return;
	case(2):
		extract_files(*archive_name, *values);
		return;
	case(3):
		append(*archive_name, *values);
		return;
	case(4):
		exclude(*archive_name, *values);
		return;
	case(5):
		merge(*archive_name, *values);
		return;
	default:
		return;
	}
}



void Action_Processor::create(const std::string &_archive_name, const Archive_Manager::Files_Paths &_paths) const
{
	Archive_Manager am;

	Archive* archive = am.pack_files(_paths);
	if(archive == nullptr)
	{
		std::cout << "Some errors occured:\n" << am.error_log() << "\n\nArchive not created\n\n";
		return;
	}

	if(!am.save(*archive, _archive_name))
		std::cout << "Some errors occured:\n" << am.error_log() << "\n\nArchive not created\n\n";

	delete archive;
}

void Action_Processor::print_files_in_archive(const std::string &_archive_name) const
{
	Archive_Manager am;

	Archive* archive = am.load(_archive_name);
	if(archive == nullptr)
	{
		std::cout << "Some errors occured:\n" << am.error_log() << "\n\n";
		return;
	}

	const Archive::Files_Data& files_data = archive->unpacked_data();
	Archive::Files_Data::const_iterator it = files_data.cbegin();
	std::cout << "Files in archive \"" << _archive_name << "\":\n";
	while(it != files_data.cend())
	{
		std::cout << "- " << it->first << "\n";
		++it;
	}
	std::cout << "\n";

	delete archive;
}

void Action_Processor::extract_files(const std::string &_archive_name, const Archive_Manager::Files_Paths &_paths) const
{
	Archive_Manager am;

	Archive* archive = am.load(_archive_name);
	if(archive == nullptr)
	{
		std::cout << "Some errors occured:\n" << am.error_log() << "\n\n";
		return;
	}

	if(_paths.size() == 0)
	{
		if(!am.unpack_files(*archive, ""))
			std::cout << "Some errors occured:\n" << am.error_log() << "\n\n";
	}
	else
	{
		if(!am.unpack_files(*archive, _paths))
			std::cout << "Some errors occured:\n" << am.error_log() << "\n\n";
	}

	delete archive;
}

void Action_Processor::append(const std::string &_archive_name, const Archive_Manager::Files_Paths &_paths) const
{
	Archive_Manager am;

	Archive* archive = am.load(_archive_name);
	if(archive == nullptr)
	{
		std::cout << "Some errors occured:\n" << am.error_log() << "\n\n";
		return;
	}

	if(!am.append_files(*archive, _paths))
	{
		std::cout << "Some errors occured:\n" << am.error_log() << "\n\n";
		delete archive;
		return;
	}

	if(!am.save(*archive, _archive_name))
		std::cout << "Some errors occured:\n" << am.error_log() << "\n\n";

	delete archive;
}

void Action_Processor::exclude(const std::string &_archive_name, const Archive_Manager::Files_Paths &_paths) const
{
	Archive_Manager am;

	Archive* archive = am.load(_archive_name);
	if(archive == nullptr)
	{
		std::cout << "Some errors occured:\n" << am.error_log() << "\n\n";
		return;
	}

	if(!am.exclude_files(*archive, _paths))
	{
		std::cout << "Some errors occured:\n" << am.error_log() << "\n\n";
		delete archive;
		return;
	}

	am.save(*archive, _archive_name);

	delete archive;
}

void Action_Processor::merge(const std::string &_result_archive_name, const Archive_Manager::Files_Paths &_paths) const
{
	Archive_Manager am;

	Archive* archive = am.merge_archives(_paths);
	if(archive == nullptr)
	{
		std::cout << "Some errors occured:\n" << am.error_log() << "\n\n";
		return;
	}

	if(!am.save(*archive, _result_archive_name))
		std::cout << "Some errors occured:\n" << am.error_log() << "\n\n";

	delete archive;
}

void Action_Processor::print_help_message() const
{
	std::cout << "Availible Actions:\n"
			  << "1. Create new archive ARCHIVE_FILE from specified FILEs:\n   -c/--create {ARCHIVE_FILE} [FILE_1] [FILE_2] ...\n"
			  << "2. Print list of files in archive ARCHIVE_FILE:\n   -l/--list {ARCHIVE_FILE}\n"
			  << "3. Extract files FILE_1, FILE2, ... from archive ARCHIVE_FILE:\n   -x/--extract {ARCHIVE_FILE} [FILE_1] [FILE_2] ...\n"
			  << "4. Extract all files from archive ARCHIVE_FILE into ARCHIVE_FILE's directory:\n   -x/--extract {ARCHIVE_FILE}\n"
			  << "5. Append specified FILEs to archive ARCHIVE_FILE:\n   -a/--append {ARCHIVE_FILE} [FILE_1] [FILE_2] ...\n"
			  << "6. Remove specified FILEs from archive ARCHIVE_FILE:\n   -d/--delete {ARCHIVE_FILE} [FILE_1] [FILE_2] ...\n"
			  << "7. Merge specified archive FILEs into archive ARCHIVE_FILE:\n   -A/--concatenate {ARCHIVE_FILE} [FILE_1] [FILE_2] ...\n"
			  << "You can specify {ARCHIVE_FILE} as a single-value argument --file=[FILE_NAME] or as -f [FILE_NAME]\n\n";
}

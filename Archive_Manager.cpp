#include "Archive_Manager.h"


void Archive_Manager::clear_error_log()
{
	m_error_log.clear();
	m_errors_count = 0;
}

void Archive_Manager::append_error_message(const std::string &_message)
{
	m_error_log += std::to_string(m_errors_count + 1) + ". " + _message + '\n';
	++m_errors_count;
}



Archive* Archive_Manager::pack_files(const Archive::Files_Paths& _files_to_pack)
{
	clear_error_log();

	if(_files_to_pack.size() == 0)
	{
		append_error_message("No files to pack specified");
		return nullptr;
	}

	bool all_files_exist = true;

	std::list<std::string>::const_iterator it = _files_to_pack.cbegin();
	while(it != _files_to_pack.cend())
	{
		std::ifstream test(*it, std::ios::in);
		if(!test.is_open())
		{
			all_files_exist = false;
			append_error_message("File \"" + *it + "\" not found");
		}
		else
			test.close();

		++it;
	}

	if(!all_files_exist)
		return nullptr;

	Archive* archive = new Archive;

	it = _files_to_pack.cbegin();
	while(it != _files_to_pack.cend())
	{
		std::string actual_file_name;
		unsigned int last_slash_found = 0;
		for(unsigned int i=0; i<it->size(); ++i)
			if((*it)[i] == '/')
				last_slash_found = i;
		for(unsigned int i=last_slash_found + 1; i<it->size(); ++i)
			actual_file_name += (*it)[i];

		std::ifstream file(*it, std::ios::in);

		std::string content;
		file.seekg(0, std::ios::end);
		content.resize(file.tellg());
		file.seekg(0, std::ios::beg);

		for(unsigned int i=0; i<content.size(); ++i)
			content[i] = file.get();

		file.close();

		archive->add_raw_file_data(actual_file_name, (std::string&&)(content));

		++it;
	}

	archive->pack();

	return archive;
}

bool Archive_Manager::unpack_files(const Archive &_archive, const std::string& _where)
{
	clear_error_log();

	if(!_archive.is_ok())
	{
		append_error_message("Unable to unpack archive with errors: " + _archive.error());
		return false;
	}

	std::string path = _where;
	if(path.size() != 0)
	{
		if(path[path.size() - 1] != '/')
			path += '/';
	}

	std::list<Archive::File_Data>::const_iterator it = _archive.unpacked_data().cbegin();
	while(it != _archive.unpacked_data().cend())
	{
		std::ofstream file(path + it->file_name, std::ios::trunc);
		if(!file.is_open())
		{
			append_error_message("Directory \"" + _where + "\" does not exist");
			return false;
		}

		file << it->content;
		file.close();

		++it;
	}

	return true;
}

Archive* Archive_Manager::load(const std::string &_file_name)
{
	clear_error_log();

	std::string path = _file_name + ".haf";

	std::ifstream file(path, std::ios::in);
	if(!file.is_open())
	{
		append_error_message("Archive file \"" + path + "\" not found");
		return nullptr;
	}

	std::string content;
	file.seekg(0, std::ios::end);
	content.resize(file.tellg());
	file.seekg(0, std::ios::beg);

	for(unsigned int i=0; i<content.size(); ++i)
		content[i] = file.get();

	file.close();

	Archive* archive = new Archive;

	archive->unpack((std::string&&)content);

	if(archive->is_ok())
		return archive;

	append_error_message(archive->error());

	delete archive;

	return nullptr;
}

bool Archive_Manager::save(const Archive &_archive, const std::string &_file_name)
{
	clear_error_log();

	if(!_archive.is_ok())
	{
		append_error_message("Unable to save archive with errors: " + _archive.error());
		return false;
	}

	std::string path = _file_name + ".haf";

	std::ofstream file(path, std::ios::trunc);
	if(!file.is_open())
	{
		append_error_message("Could not open \"" + path + "\". Make sure specified directory exists");
		return false;
	}

	file << _archive.packed_data();
	file.close();

	return true;
}


const std::string& Archive_Manager::error_log() const
{
	return m_error_log;
}











































































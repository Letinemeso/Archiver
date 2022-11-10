#include "Archive_Manager.h"


void Archive_Manager::clear_error_log()
{
	m_error_log.clear();
	m_errors_count = 0;
}

void Archive_Manager::append_error_message(const std::string &_message)
{
	m_error_log += _message + '\n';
	++m_errors_count;
}



std::string Archive_Manager::parse_file_name(const std::string &_name_w_path) const
{
	unsigned int last_slash_found = 0;
	for(unsigned int i=0; i<_name_w_path.size(); ++i)
		if((_name_w_path)[i] == '/')
			last_slash_found = i;

	if(last_slash_found == 0)
		return _name_w_path;

	return std::string(_name_w_path.c_str() + last_slash_found + 1);
}



Archive* Archive_Manager::pack_files(const Files_Paths& _files_to_pack)
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
		std::string actual_file_name = parse_file_name(*it);

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

bool Archive_Manager::append_files(Archive &_append_to, const Files_Paths &_files_to_append)
{
	clear_error_log();

	if(!_append_to.is_ok())
	{
		append_error_message(_append_to.error());
		return false;
	}

	if(_files_to_append.size() == 0)
	{
		append_error_message("No files to append specified");
		return false;
	}

	bool all_files_exist = true;

	std::list<std::string>::const_iterator it = _files_to_append.cbegin();
	while(it != _files_to_append.cend())
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
		return false;

	it = _files_to_append.cbegin();
	while(it != _files_to_append.cend())
	{
		std::string actual_file_name = parse_file_name(*it);

		std::ifstream file(*it, std::ios::in);

		std::string content;
		file.seekg(0, std::ios::end);
		content.resize(file.tellg());
		file.seekg(0, std::ios::beg);

		for(unsigned int i=0; i<content.size(); ++i)
			content[i] = file.get();

		file.close();

		_append_to.add_raw_file_data(actual_file_name, (std::string&&)(content));

		++it;
	}

	_append_to.pack();

	return true;
}

bool Archive_Manager::exclude_files(Archive &_exclude_from, const Files_Paths &_files_to_exclude)
{
	clear_error_log();

	if(!_exclude_from.is_ok())
	{
		append_error_message(_exclude_from.error());
		return false;
	}

	if(_files_to_exclude.size() == 0)
	{
		append_error_message("No files to exclude specified");
		return false;
	}

	bool all_files_exist = true;

	if(_files_to_exclude.size() >= _exclude_from.unpacked_data().size())
	{
		append_error_message("Cannot remove all files from archive, just delete it instead");
		return false;
	}

	std::list<std::string>::const_iterator it = _files_to_exclude.cbegin();
	while(it != _files_to_exclude.cend())
	{
		const std::string* check = _exclude_from.unpacked_data(*it);
		if(check == nullptr)
		{
			all_files_exist = false;
			append_error_message("File \"" + *it + "\" not found");
		}

		++it;
	}

	if(!all_files_exist)
		return false;

	it = _files_to_exclude.cbegin();
	while(it != _files_to_exclude.cend())
	{
		_exclude_from.remove_raw_file_data(*it);
		++it;
	}

	_exclude_from.pack();

	return true;
}

Archive* Archive_Manager::merge_archives(const Files_Paths& _archives)
{
	clear_error_log();

	if(_archives.size() < 2)
	{
		append_error_message("Cannot merge less then 2 archives");
		return nullptr;
	}

	Archive* result = new Archive;

	Files_Paths::const_iterator it = _archives.cbegin();
	while(it != _archives.cend())
	{
		Archive_Manager mgr;
		Archive* archive = mgr.load(*it);
		if(archive == nullptr)
		{
			append_error_message(mgr.error_log());
			delete result;
			return nullptr;
		}
		if(!archive->is_ok())
		{
			append_error_message(mgr.error_log());
			delete archive;
			delete result;
			return nullptr;
		}

		Archive::Files_Data& archive_data = archive->unpacked_data();
		Archive::Files_Data::iterator arc_it = archive_data.begin();
		while(arc_it != archive_data.end())
		{
			result->add_raw_file_data(arc_it->first, (std::string&&)arc_it->second);
			++arc_it;
		}

		delete archive;

		++it;
	}

	return result;
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

	Archive::Files_Data::const_iterator it = _archive.unpacked_data().cbegin();
	while(it != _archive.unpacked_data().cend())
	{
		std::ofstream file(path + it->first, std::ios::trunc);
		if(!file.is_open())
		{
			append_error_message("Directory \"" + _where + "\" does not exist");
			return false;
		}

		file << it->second;
		file.close();

		++it;
	}

	return true;
}

bool Archive_Manager::unpack_files(const Archive &_archive, const Files_Paths& _files_to_unpack)
{
	clear_error_log();

	if(!_archive.is_ok())
	{
		append_error_message("Unable to unpack archive with errors: " + _archive.error());
		return false;
	}

	if(_files_to_unpack.size() == 0)
	{
		append_error_message("No files to unpack specified");
		return false;
	}

	bool all_files_exist = true;

	std::list<std::string>::const_iterator it = _files_to_unpack.cbegin();
	while(it != _files_to_unpack.cend())
	{
		std::string file_name = parse_file_name(*it);

		const std::string* file_data = _archive.unpacked_data(file_name);
		if(file_data == nullptr)
		{
			append_error_message("File \"" + file_name + "\" not found in specified archive");
			all_files_exist = false;
		}

		++it;
	}

	if(!all_files_exist)
		return false;


	it = _files_to_unpack.begin();
	while(it != _files_to_unpack.cend())
	{
		std::string file_name = parse_file_name(*it);

		const std::string* file_data = _archive.unpacked_data(file_name);
		if(file_data == nullptr)
		{
			append_error_message("File \"" + file_name + "\" not found in specified archive");
			++it;
			continue;
		}

		std::ofstream file(*it, std::ios::trunc);
		if(!file.is_open())
		{
			append_error_message("Could not open or create file \"" + *it + ".haf\"");
			++it;
			continue;
		}

		file << *file_data;

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











































































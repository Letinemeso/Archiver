#include "Archive_Manager.h"


void Archive_Manager::clear_error_log()
{
	m_error_log.clear();
}

void Archive_Manager::append_error_message(const std::string &_message)
{
	m_error_log += _message + '\n';
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



bool Archive_Manager::pack_files(const Files_Paths& _files_to_pack, const std::string& _archive_name)
{
	clear_error_log();

	if(_files_to_pack.size() == 0)
	{
		append_error_message("No files to pack specified");
		return false;
	}

	bool all_files_exist = true;

	std::list<std::string>::const_iterator it = _files_to_pack.cbegin();
	while(it != _files_to_pack.cend())
	{
		std::ifstream test(*it, std::ios::binary);
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

	Archive::Content_t file_data;

	it = _files_to_pack.cbegin();
	while(it != _files_to_pack.cend())
	{
		Archive::File_Data data;

		data.file_name = parse_file_name(*it);
		data.file.set_path(*it);
		data.size = data.file.length();

		file_data.push_back(data);

		++it;
	}
	Archive archive(_archive_name);
	archive.pack(file_data);

	return true;
}

bool Archive_Manager::append_files(const std::string &_append_to, const Files_Paths &_files_to_append)
{
	clear_error_log();

	File archive_file(_append_to);
	if(!archive_file.exists())
	{
		append_error_message("Specified archive file does not exist");
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
		File test(*it);
		if(!test.exists())
		{
			all_files_exist = false;
			append_error_message("File \"" + *it + "\" not found");
		}

		++it;
	}

	if(!all_files_exist)
		return false;


//	Archive archive;
//	std::list<Archive::File_Data> archived_files = archive.unpack(archive_file);

//	for(auto it = _files_to_append.cbegin(); it != _files_to_append.cend(); ++it)
//		archived_files.push_back({File(*it), parse_file_name(*it)});

//	archive_file.clear();
//	archive.pack(archive_file, archived_files);

	return true;
}

bool Archive_Manager::exclude_files(Archive &_exclude_from, const Files_Paths &_files_to_exclude)
{
	clear_error_log();

//	if(!_exclude_from.is_ok())
//	{
//		append_error_message(_exclude_from.error());
//		return false;
//	}

//	if(_files_to_exclude.size() == 0)
//	{
//		append_error_message("No files to exclude specified");
//		return false;
//	}

//	bool all_files_exist = true;

//	if(_files_to_exclude.size() >= _exclude_from.unpacked_data().size())
//	{
//		append_error_message("Cannot remove all files from archive, just delete it instead");
//		return false;
//	}

//	std::list<std::string>::const_iterator it = _files_to_exclude.cbegin();
//	while(it != _files_to_exclude.cend())
//	{
//		const std::string* check = _exclude_from.unpacked_data(*it);
//		if(check == nullptr)
//		{
//			all_files_exist = false;
//			append_error_message("File \"" + *it + "\" not found");
//		}

//		++it;
//	}

//	if(!all_files_exist)
//		return false;

//	it = _files_to_exclude.cbegin();
//	while(it != _files_to_exclude.cend())
//	{
//		_exclude_from.remove_raw_file_data(*it);
//		++it;
//	}

//	_exclude_from.pack();

	return true;
}

bool Archive_Manager::merge_archives(const Files_Paths& _archives)
{
//	clear_error_log();

//	if(_archives.size() < 2)
//	{
//		append_error_message("Cannot merge less then 2 archives");
//		return nullptr;
//	}

//	Archive* result = new Archive;

//	Files_Paths::const_iterator it = _archives.cbegin();
//	while(it != _archives.cend())
//	{
//		Archive_Manager mgr;
//		Archive* archive = mgr.load(*it);
//		if(archive == nullptr)
//		{
//			append_error_message(mgr.error_log());
//			delete result;
//			return nullptr;
//		}
//		if(!archive->is_ok())
//		{
//			append_error_message(mgr.error_log());
//			delete archive;
//			delete result;
//			return nullptr;
//		}

//		Archive::Files_Data& archive_data = archive->unpacked_data();
//		Archive::Files_Data::iterator arc_it = archive_data.begin();
//		while(arc_it != archive_data.end())
//		{
//			result->add_raw_file_data(arc_it->first, (std::string&&)arc_it->second);
//			++arc_it;
//		}

//		delete archive;

//		++it;
//	}

//	result->pack();

//	if(result->is_ok())
//		return result;

//	append_error_message(result->error());
//	delete result;

	return false;
}

bool Archive_Manager::unpack_files(const std::string& _archive_path, const std::string& _where)
{
	clear_error_log();

	File archive_file(_archive_path);

	if(!archive_file.exists())
	{
		append_error_message("Archive file \"" + _archive_path + "\" not found");
		return false;
	}

	std::string path = _where;
	if(path.size() != 0)
	{
		if(path[path.size() - 1] != '/')
			path += '/';
	}

	Archive archive(_archive_path);
	archive.unpack();

	return true;
}

bool Archive_Manager::unpack_files(const Archive &_archive, const Files_Paths& _files_to_unpack)
{
//	clear_error_log();

//	if(!_archive.is_ok())
//	{
//		append_error_message("Unable to unpack archive with errors: " + _archive.error());
//		return false;
//	}

//	if(_files_to_unpack.size() == 0)
//	{
//		append_error_message("No files to unpack specified");
//		return false;
//	}

//	bool all_files_exist = true;

//	std::list<std::string>::const_iterator it = _files_to_unpack.cbegin();
//	while(it != _files_to_unpack.cend())
//	{
//		std::string file_name = parse_file_name(*it);

//		const std::string* file_data = _archive.unpacked_data(file_name);
//		if(file_data == nullptr)
//		{
//			append_error_message("File \"" + file_name + "\" not found in specified archive");
//			all_files_exist = false;
//		}

//		++it;
//	}

//	if(!all_files_exist)
//		return false;


//	it = _files_to_unpack.begin();
//	while(it != _files_to_unpack.cend())
//	{
//		std::string file_name = parse_file_name(*it);

//		const std::string* file_data = _archive.unpacked_data(file_name);

//		std::ofstream file(*it, std::ios::binary);
//		if(!file.is_open())
//		{
//			append_error_message("Could not open or create file \"" + *it + ".haf\"");
//			++it;
//			continue;
//		}

//		file << *file_data;

//		file.close();

//		++it;
//	}

	return true;
}


const std::string& Archive_Manager::error_log() const
{
	return m_error_log;
}











































































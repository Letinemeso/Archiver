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
	archive.clear();
	archive.append_files(file_data);

	return archive.is_ok();
}

bool Archive_Manager::append_files(const std::string& _append_to, const Files_Paths &_files_to_append)
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

	Archive::Content_t file_data;

	it = _files_to_append.cbegin();
	while(it != _files_to_append.cend())
	{
		Archive::File_Data data;

		data.file_name = parse_file_name(*it);
		data.file.set_path(*it);
		data.size = data.file.length();

		file_data.push_back(data);

		++it;
	}
	Archive archive(_append_to);
//	archive.clear();
	archive.append_files(file_data);

	return archive.is_ok();
}

bool Archive_Manager::exclude_files(const std::string& _exclude_from, const Files_Paths &_files_to_exclude)
{
	clear_error_log();

	if(_files_to_exclude.size() == 0)
	{
		append_error_message("No files to exclude specified");
		return false;
	}

	Archive archive(_exclude_from);

	bool all_files_exist = true;
	for(auto it = _files_to_exclude.cbegin(); it != _files_to_exclude.cend(); ++it)
	{
		if(!archive.contains(*it))
		{
			all_files_exist = false;
			append_error_message("Specified archive does not contain file \"" + *it + "\"");
		}
	}

	if(!all_files_exist)
		return false;

	archive.exclude_files(_files_to_exclude);

	return archive.is_ok();
}

bool Archive_Manager::merge_archives(const std::string& _archive_path, const Files_Paths& _archives)
{
	clear_error_log();

	if(_archives.size() < 2)
	{
		append_error_message("Cannot merge less then 2 archives");
		return false;
	}

	Archive archive(_archive_path);
	archive.clear();

	std::list<Archive> archives_to_merge;
	for(const auto& path : _archives)
		archives_to_merge.push_back(Archive(path));

	Archive::Content_t merged_content;
	for(const auto& archive : archives_to_merge)
	{
		for(const auto& data : archive.content())
			merged_content.push_back(data);
	}

	archive.append_files(merged_content);

	return archive.is_ok();
}

bool Archive_Manager::unpack_files(const std::string& _archive, const Files_Paths& _files_to_unpack)
{
	clear_error_log();

	File archive_file(_archive);

	if(!archive_file.exists())
	{
		append_error_message("Archive file \"" + _archive + "\" not found");
		return false;
	}

	Archive archive(_archive);

	bool contains_all_files = true;
	for(const auto& filename : _files_to_unpack)
	{
		if(!archive.contains(filename))
		{
			contains_all_files = false;
			append_error_message("Specified arachive does not contain file \"" + filename + "\"");
		}
	}

	if(!contains_all_files)
		return false;

	archive.unpack(_files_to_unpack);

	return archive.is_ok();
}


std::list<std::string> Archive_Manager::get_archive_content(const std::string &_archive)
{
	clear_error_log();

	File archive_file(_archive);

	if(!archive_file.exists())
	{
		append_error_message("Archive file \"" + _archive + "\" not found");
		return {};
	}

	Archive archive(_archive);

	if(!archive.is_ok())
	{
		append_error_message(archive.error());
		return {};
	}

	std::list<std::string> result;

	for(const auto& file : archive.content())
		result.push_back(file.file_name);

	return result;
}


const std::string& Archive_Manager::error_log() const
{
	return m_error_log;
}











































































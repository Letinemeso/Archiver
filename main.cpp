#include <iostream>

#include "HCoder.h"
#include "Archive.h"
#include "Archive_Manager.h"
#include "Args_Parser.h"

void print_binary(char c)
{
	for(unsigned int i=0; i<8; ++i)
		std::cout << ((c & (1 << i)) > 0) << " ";
	std::cout << "\n";
}

int main(int args_count, char** args)
{
	std::string args_string;

	for(int i=0; i<args_count; ++i)
	{
		args_string.append(args[i]);
		args_string += ' ';
	}

//	args_string = "./Archiver --file -ass.txt ";

	Args_Parser ap;

	ap.parse(args_string);

//	const Args_Parser::Values* values = ap.values("--ass");
	const Args_Parser::Args& a = ap.args();

	for(const auto& arg : a)
	{
		std::cout << "|" << arg.first << "|\n";
		for(const auto& val : arg.second)
			std::cout << "\t" << val << "\n";
		std::cout << "\n";
	}









	return 0;


	Archive_Manager mgr;

//	Archive arc;

//	arc.add_raw_file_data("test.txt", "some data");
//	arc.pack();

//	if(arc.error().size() > 0)
//	{
//		std::cout << arc.error() << "\n";
//		return 1;
//	}

//	if(!mgr.save(arc, "test"))
//	{
//		std::cout << mgr.error_log() << "\n";
//		return 1;
//	}




//	Archive::Files_Paths paths;
//	paths.push_back("../test.txt");
//	paths.push_back("../test_2.txt");

//	Archive* arc = mgr.pack_files(paths);
//	if(!arc)
//	{
//		std::cout << mgr.error_log() << "\n";
//		return 1;
//	}

//	mgr.save(*arc, "archive");

//	delete arc;




	Archive* arc = mgr.load("archive");

	if(!arc)
	{
		std::cout << mgr.error_log() << "\n";
		return 1;
	}

	if(!mgr.unpack_files(*arc, "../../"))
	{
		std::cout << mgr.error_log() << "\n";
		return 1;
	}

	delete arc;






	return 0;
}

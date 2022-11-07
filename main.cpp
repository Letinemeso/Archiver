#include <iostream>

#include "Data_Block.h"

int main()
{
	std::string str;
	str += 180;
	str += 153;


	Archive archive((const unsigned char*)(str.c_str()), str.size());

	archive.print_blocks();

	return 0;
}

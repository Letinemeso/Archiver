#include <iostream>

#include "Args_Parser.h"
#include "Action_Processor.h"


int main(int args_count, char** args)
{
	Args_Parser ap;

	ap.parse(args, args_count);

	Action_Processor action_processor;

	action_processor.process(ap);



//	File test("../../test.txt");

//	HCoder coder;

//	test.clear();
//	coder.encode("12345678901qwertyuiop[");
//	test.append_block(coder.encoded_data());
//	coder.encode("qwertyuiop[12345678901");
//	test.append_block(coder.encoded_data());
//	coder.encode("alalalalala");
//	test.append_block(coder.encoded_data());
//	coder.encode("12345678901qwertyuiop[12345678901");
//	test.append_block(coder.encoded_data());
//	coder.encode("123111");
//	test.append_block(coder.encoded_data());

//	unsigned int step = 64;
//	for(unsigned int i=0; i<step * 4; i += step)
//	{
//		coder.decode(test.extract_block(i, step));
//		std::cout << coder.decoded_data() << "\n";
//	}

	return 0;
}

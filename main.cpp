#include <iostream>

#include "Args_Parser.h"
#include "Action_Processor.h"


int main(int args_count, char** args)
{
	Args_Parser ap;

	ap.parse(args, args_count);

	Action_Processor action_processor;

	action_processor.process(ap);

	return 0;
}

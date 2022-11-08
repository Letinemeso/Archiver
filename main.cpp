#include <iostream>

#include "HCoder.h"

void print_binary(char c)
{
	for(unsigned int i=0; i<8; ++i)
		std::cout << ((c & (1 << i)) > 0) << " ";
	std::cout << "\n";
}

int main()
{
	std::string str;
//	str += 180;
//	str += 153;
//	str += 2;
//	str += 12;
//	str += 99;
//	str += "\0";
	str = "amnalanlskbk12321snkvlsndlkb     ss";

	HCoder HCoder;

//	HCoder.print_blocks();

	HCoder.encode(str);

	auto raw = HCoder.decoded_data();

	std::cout << "raw: " << raw << "\n";

	for(unsigned int i = 0; i < raw.size(); ++i)
		std::cout << (int)(unsigned char)(raw[i]) << " ";
	std::cout << "\n\n";

	std::cout << "raw binary:\n";
	for(unsigned int i = 0; i < raw.size(); ++i)
		print_binary(raw[i]);
	std::cout << "\n";


	auto encoded = HCoder.encoded_data();

	std::cout << "encoded: ";
	for(unsigned int i = 0; i < encoded.size(); ++i)
		std::cout << encoded[i];
	std::cout << "\n";

	for(unsigned int i = 0; i < encoded.size(); ++i)
		std::cout << (int)(unsigned char)(encoded[i]) << " ";
	std::cout << "\n\n";

	std::cout << "encoded binary:\n";
	for(unsigned int i = 0; i < encoded.size(); ++i)
		print_binary(encoded[i]);
	std::cout << "\n";

	HCoder.decode(encoded);


	std::string decoded = HCoder.decoded_data();

	std::cout << "decoded: ";
	for(unsigned int i = 0; i < decoded.size(); ++i)
		std::cout << decoded[i];
	std::cout << "\n";

	for(unsigned int i = 0; i < decoded.size(); ++i)
		std::cout << (int)(unsigned char)(decoded[i]) << " ";
	std::cout << "\n\n";

	std::cout << "decoded binary:\n";
	for(unsigned int i = 0; i < decoded.size(); ++i)
		print_binary(decoded[i]);
	std::cout << "\n";

	return 0;
}

#include <webrpc/Parser.h>

int main()
{
	// std::string input;
	// std::getline(std::cin, input);
	const std::string input("[true,42,3.14,abc,[1,2,3],{key:value},<00,0a,de,ad,ff>]");

	auto ov = parse_value(input);

	if (ov)
	{
		std::cout
			<< "success"
			<< " input=\"" << input << "\""
			<< " value=\"" << ov.value().to_string() << "\""
			<< std::endl;
	}
	else
	{
		std::cout
			<< "parsing failed"
			<< " input=" << input << std::endl;
	}
}
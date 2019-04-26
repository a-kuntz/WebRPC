#include <webrpc/Parser.h>

int main()
{
	std::string input;
	while (std::getline(std::cin, input))
	{
		auto ov = Parser::parse_value(input);

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
}

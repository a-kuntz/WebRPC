#include <webrpc/Parser.h>

int main()
{
	// std::string input("42/3.14,42,true,false");
	std::string input("[3.14,42,true,false]");
	auto itr = input.begin();

	grammar<std::string::iterator, ascii::space_type> g;

	value_t value;
	// if (qi::phrase_parse(itr, input.end(), g, ascii::space))
	if (qi::phrase_parse(itr, input.end(), g, ascii::space, value))
	{
		std::cout
			<< "success"
			<< " input=\"" << input << "\""
			<< " value=\"" << value.to_string() << "\""
			<< std::endl;
		// std::cout << "remaining input: " << itr << std::endl;
	}
	else
	{
		std::cout
			<< "parsing failed"
			<< " input=" << input << std::endl;
	}
}
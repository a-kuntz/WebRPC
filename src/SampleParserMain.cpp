#include <webrpc/Parser.h>

int main()
{
	std::cout
		<< "parsing: <name>/<value>" << std::endl
		<< "name  = <string>" << std::endl
		<< "value = <double> | <int> | <bool>" << std::endl
		<< std::endl;

	// std::string input("42/3.14,42,true,false");
	std::string input("name/3.14,42,true,false");
	auto itr = input.begin();

	request_grammar<std::string::iterator, ascii::space_type> grammar;

	grammar_type grammar_value;
	// if (qi::phrase_parse(itr, input.end(), grammar, ascii::space))
	if (qi::phrase_parse(itr, input.end(), grammar, ascii::space, grammar_value))
	{
		std::cout << "parsing succeeded: " << std::endl;
		// std::cout << "remaining input: " << itr << std::endl;
	}
	else
	{
		std::cout << "parsing failed: " << input << std::endl;
	}
}
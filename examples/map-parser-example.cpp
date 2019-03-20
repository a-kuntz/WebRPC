#include <boost/spirit/include/qi.hpp>

#include <map>
#include <string>

using namespace boost::spirit;
using namespace boost::spirit::qi;

struct name_class {};
struct quote_class {};
struct item_class {};
struct map_class {};

int main()
{
	std::string input(
		"foo    : bar ,"
		"gorp   : smart ,"
		"falcou : \"crazy frenchman\" " );
	auto iter = input.begin();
	auto iter_end = input.end();

	// auto name = rule<name_class, std::string>()
	// 	= alpha >> *alnum;
	// auto quote = rule<quote_class, std::string>()
	// 	= '"'
	// 	>> lexeme[ *(~char_('"')) ]
	// 	>> '"';
	// auto item = rule<item_class, std::pair<std::string, std::string>>()
	// 	= name >> ':' >> ( quote | name );
	// auto map = rule<map_class, std::map< std::string, std::string >>()
	// 	= item % ',';

	// std::map< std::string, std::string > val;
	// std::vector<std::pair< std::string, std::string >> val;

	// phrase_parse( iter, iter_end, map, space );
	// phrase_parse( iter, iter_end, map, space, val );

	std::string val;
	phrase_parse( iter, iter_end, alpha >> *alnum, space, val );
}
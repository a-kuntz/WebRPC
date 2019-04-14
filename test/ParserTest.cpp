#include <webrpc/Parser.h>

#include <boost/spirit/include/qi.hpp>

#include <gtest/gtest.h>

using namespace boost::spirit;

TEST(BoostSpirit, basic)
{
	auto g = +qi::char_("A-Za-z._-") >> '/' >> +qi::char_("0-9a-zA-Z._-");

	for (const std::string input : {"abc-def_ghi.jkl/4", "abc/5"})
	{
		EXPECT_EQ(true, qi::phrase_parse(input.begin(), input.end(), g, ascii::space)) << "input: " << input;
	}
}

void check(const std::string& in, const value_t::type_info type=value_t::type_info::null_type)
{
	boost::optional<value_t> opt = parse_value(in);
	ASSERT_EQ(true, static_cast<bool>(opt)) << "testcase input=" << in;
	EXPECT_EQ(type, opt.value().type()) << "testcase input=" << in << " expected type=" << type;
	EXPECT_EQ(in, opt.value().to_string());
}

TEST(Parser, PrimitiveValues)
{
	// primitive values
//	check("", value_t::type_info::null_type); // todo: enable parsing empty strings
	check("true", value_t::type_info::bool_type);
	check("42", value_t::type_info::int_type);
	check("3.14", value_t::type_info::double_type);
	// check("abc", value_t::type_info::string_type);
	// check("abc_3", value_t::type_info::string_type);
	check("\"abc\"", value_t::type_info::string_type);
	check("\"a b c?\"", value_t::type_info::string_type);
}

TEST(Parser, ArrayValues)
{
	// arrays
	check("[]", value_t::type_info::array_type);
	check("[[],[],[]]", value_t::type_info::array_type);
	check("[true,false,true]", value_t::type_info::array_type);
	check("[42,43,44]", value_t::type_info::array_type);
	check("[3.14,2.7,1.4]", value_t::type_info::array_type);
	// check("[abc,d_e_f,ghi]", value_t::type_info::array_type);
	check("[\"abc\",\"def\",\"ghi.,;.-_?!§$%&/()=\"]", value_t::type_info::array_type);
	check("[true,42,3.14,\"abc\"]", value_t::type_info::array_type);
	check("[true,42,3.14,\"abc\",[true,42,3.14,\"abc\",[true,42,3.14,\"abc\"]]]", value_t::type_info::array_type);
	check("[{key:\"val\"},{key:\"val\"},{key:\"val\"}]", value_t::type_info::array_type);
	check("[<>,<0x00>,<0x00,0x01,0x0A,0x0F,0xDE,0xAD,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF>]", value_t::type_info::array_type);
	check("[true,42,3.14,\"abc\",{key:\"value\"},[true,42,3.14,\"abc\",{key:\"value\"},[true,42,3.14,\"abc\",{key:\"value\"}]]]", value_t::type_info::array_type);
}

TEST(Parser, StructValues)
{
	// structs
	check("{}", value_t::type_info::struct_type);
	check("{key:\"value\",key2:\"value2\",key3:\"value\"}", value_t::type_info::struct_type);
	check("{key:\"value\",key2:[1,2,3],key3:{k1:\"v\",k2:\"v\",k3:\"v\"},key4:<0xAB,0xCD,0xEF>}", value_t::type_info::struct_type);
}

TEST(Parser, BytestringValues)
{
	// bytestrings
	check("<>", value_t::type_info::bytestring_type);
	check("<0x0F>", value_t::type_info::bytestring_type);
	check("<0x0F,0x0F>", value_t::type_info::bytestring_type);
	check("<0x00,0x11,0x22,0x33>", value_t::type_info::bytestring_type);
	check("<0xAA,0xBB,0xCC,0xDD,0xEE,0xFF>", value_t::type_info::bytestring_type);
	check("<0x00,0x11,0x22,0x33,0x44,0x55,0x66,0xAA,0xBB,0xCC>", value_t::type_info::bytestring_type);
}

void check(const std::string& in, const Uri& uri)
{
	boost::optional<Uri> opt = parse_uri(in);
	ASSERT_EQ(true, static_cast<bool>(opt)) << "testcase input=" << in;
	EXPECT_EQ(uri.host, opt.value().host);
	EXPECT_EQ(uri.port, opt.value().port);
	EXPECT_EQ(uri.target, opt.value().target);
}

TEST(UriParser, BasicValues)
{
	check("http://localhost:1234/request", Uri{"localhost", 1234, "request"});
	check("http://localhost/request", Uri{"localhost", 8080, "request"});
	check("http://127.0.0.1:5678/request", Uri{"127.0.0.1", 5678, "request"});
	check("http://192.168.0.1/request", Uri{"192.168.0.1", 8080, "request"});
}

void check(const std::string& in, const Target& trg)
{
	const auto opt = parse_target(in);
	ASSERT_EQ(true, static_cast<bool>(opt)) << "testcase input=" << in;
	EXPECT_EQ(trg.method, opt.value().method);
	EXPECT_EQ(trg.args, opt.value().args);
}

TEST(TargetParser, BasicValues)
{
	check("method", Target{"method", {}});
	check("system.list_methods", Target{"system.list_methods", {}});
	check("system.list-methods", Target{"system.list-methods", {}});
	check("Sum/[1,2,3,4,5]", Target{"Sum", boost::optional<std::string>{"[1,2,3,4,5]"}});
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

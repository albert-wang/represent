#define BOOST_TEST_MODULE parser test
#include <boost/test/unit_test.hpp>
#include <boost/cstdint.hpp>

#include "parser.hpp"
#include "conversion.hpp"

using namespace Represent;

void compare(const TokenStream& ts, boost::uint32_t * buffer, size_t len)
{
	const std::vector<Token>& tokens = ts.getTokens();

	BOOST_CHECK_EQUAL(len % 2, 0);
	BOOST_CHECK_EQUAL(tokens.size(), len / 2);

	std::vector<Token> comp;
	for (size_t i = 0; i < len / 2; ++i)
	{
		comp.push_back(Token(static_cast<TokenType>(buffer[i * 2 + 0]), buffer[i * 2 + 1]));
	}

	BOOST_CHECK_EQUAL_COLLECTIONS(tokens.begin(), tokens.end(), comp.begin(), comp.end());
}


BOOST_AUTO_TEST_CASE(parse_number)
{
	TokenStream tokens = Represent::parse("1");
	boost::uint32_t expected[] = { TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 1 };

	compare(tokens, expected, 4);
}

BOOST_AUTO_TEST_CASE(parse_binary)
{
	TokenStream tokens = Represent::parse("0b00001111");
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 2, TOKEN_NUMBER, 0, TOKEN_NUMBER, 0, TOKEN_NUMBER, 0, TOKEN_NUMBER, 0, TOKEN_NUMBER, 1, 
		TOKEN_NUMBER, 1, TOKEN_NUMBER, 1, TOKEN_NUMBER, 1};

	compare(tokens, expected, sizeof(expected) / sizeof(expected[0]));
}

BOOST_AUTO_TEST_CASE(parse_octal)
{
	TokenStream tokens = Represent::parse("0777");
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 8, TOKEN_NUMBER, 7, TOKEN_NUMBER, 7, TOKEN_NUMBER, 7}; 

	compare(tokens, expected, sizeof(expected) / sizeof(expected[0]));
}

BOOST_AUTO_TEST_CASE(parse_hex)
{
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 16, TOKEN_NUMBER, 15, TOKEN_NUMBER, 0, TOKEN_NUMBER, 5};
	compare(Represent::parse("0xF05"), expected, sizeof(expected) / sizeof(expected[0]));
}

BOOST_AUTO_TEST_CASE(parse_decimal) 
{
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 0, TOKEN_DECIMAL_POINT, 0, TOKEN_NUMBER, 1};
	compare(Represent::parse("0.1"), expected, sizeof(expected) / sizeof(expected[0]));
}

BOOST_AUTO_TEST_CASE(parse_hex_decimal) 
{
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 16, TOKEN_NUMBER, 10, TOKEN_NUMBER, 5, TOKEN_NUMBER, 14, TOKEN_DECIMAL_POINT, 0, TOKEN_NUMBER, 14};
	compare(Represent::parse("0xA5E.E"), expected, sizeof(expected) / sizeof(expected[0]));
}

BOOST_AUTO_TEST_CASE(octal_zero)
{
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 8, TOKEN_NUMBER, 0};
	compare(Represent::parse("00"), expected, sizeof(expected) / sizeof(expected[0]));
}

BOOST_AUTO_TEST_CASE(octal_decimal) 
{
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 8, TOKEN_NUMBER, 0, TOKEN_DECIMAL_POINT, 0, TOKEN_NUMBER, 0};
	compare(Represent::parse("00.0"), expected, sizeof(expected) / sizeof(expected[0]));
}

BOOST_AUTO_TEST_CASE(parse_empty)
{
	TokenStream tokens = Represent::parse("");
	TokenStream expected;

	BOOST_CHECK_EQUAL_COLLECTIONS(tokens.begin(), tokens.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(convert_decimal)
{
	TokenStream tokens = Represent::parse("21568901345089126350");
	Value v = Represent::convert(tokens);

	BOOST_CHECK_EQUAL(v, Value("21568901345089126350"));
}

BOOST_AUTO_TEST_CASE(parse_simple_expression)
{
	TokenStream tokens = Represent::parse("42 + 5");
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 4, TOKEN_NUMBER, 2, TOKEN_OPERATOR, OPERATOR_ADD, TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 5};

	compare(tokens, expected, sizeof(expected) / sizeof(expected[0]));
}

BOOST_AUTO_TEST_CASE(fail_parse_expression)
{
	TokenStream tokens = Represent::parse("42 + ");
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 4, TOKEN_NUMBER, 2};

	compare(tokens, expected, sizeof(expected) / sizeof(expected[0]));	
}
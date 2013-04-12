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

#define AUTO_COMPARE(first, arr) compare(first, arr, sizeof(arr) / sizeof(arr[0]))

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

	AUTO_COMPARE(tokens, expected);
}

BOOST_AUTO_TEST_CASE(parse_octal)
{
	TokenStream tokens = Represent::parse("0777");
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 8, TOKEN_NUMBER, 7, TOKEN_NUMBER, 7, TOKEN_NUMBER, 7}; 

	AUTO_COMPARE(tokens, expected);
}

BOOST_AUTO_TEST_CASE(parse_hex)
{
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 16, TOKEN_NUMBER, 15, TOKEN_NUMBER, 0, TOKEN_NUMBER, 5};
	AUTO_COMPARE(Represent::parse("0xF05"), expected);
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
	boost::uint32_t expected[] = {TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 4, TOKEN_NUMBER, 2, TOKEN_OPERATOR, OPERATOR_PLUS, TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 5};

	compare(tokens, expected, sizeof(expected) / sizeof(expected[0]));
	AUTO_COMPARE(tokens, expected);
}

BOOST_AUTO_TEST_CASE(fail_parse_expression)
{
	TokenStream tokens = Represent::parse("42 + ");
	BOOST_CHECK(tokens.end() == tokens.begin());
}

BOOST_AUTO_TEST_CASE(parse_unary_plus)
{
	boost::uint32_t expected[] = {TOKEN_OPERATOR, OPERATOR_UNARY_PLUS, TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 4};
	AUTO_COMPARE(Represent::parse("+4"), expected);
}

BOOST_AUTO_TEST_CASE(parse_unary_minus)
{
	boost::uint32_t expected[] = {TOKEN_OPERATOR, OPERATOR_UNARY_MINUS, TOKEN_BASE_FLAG, 2, TOKEN_NUMBER, 1};
	AUTO_COMPARE(Represent::parse("-0b1"), expected);
}

BOOST_AUTO_TEST_CASE(parse_multiple_unary_op)
{
	TokenStream tokens = Represent::parse("+++++4");
	BOOST_CHECK(tokens.begin() == tokens.end());
}

BOOST_AUTO_TEST_CASE(parse_function)
{
	TokenStream tokens = Represent::parse("fun(1, 2, 3)");
	boost::uint32_t expected[] = {
		TOKEN_FUNCTION_IDENTIFIER, 0, 
		TOKEN_IDENTIFIER_RAW, 0, TOKEN_RAW, 'f', TOKEN_RAW, 'u', TOKEN_RAW, 'n', TOKEN_PAREN, 0, 
		TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 1, TOKEN_ARG_DELIMIT, 0, TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 2, TOKEN_ARG_DELIMIT, 0, 
		TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 3, TOKEN_PAREN, 1};

	AUTO_COMPARE(tokens, expected);
}

BOOST_AUTO_TEST_CASE(parse_identifier_only)
{
	TokenStream tokens = Represent::parse("id");
	boost::uint32_t expected[] = {
		TOKEN_IDENTIFIER_RAW, 0, TOKEN_RAW, 'i', TOKEN_RAW, 'd'
	};

	AUTO_COMPARE(tokens, expected);
}

BOOST_AUTO_TEST_CASE(function_names_cannot_end_with_dash)
{
	TokenStream tokens = Represent::parse("fun-(1, 2, 3)");
	BOOST_CHECK(tokens.begin() == tokens.end());
}

BOOST_AUTO_TEST_CASE(parse_identifier)
{
	TokenStream tokens = Represent::parse("pi + 4");
	boost::uint32_t expected[] = {
		TOKEN_IDENTIFIER_RAW, 0, TOKEN_RAW, 'p', TOKEN_RAW, 'i', 
		TOKEN_OPERATOR, OPERATOR_PLUS, 
		TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 4
	};

	AUTO_COMPARE(tokens, expected);
	/*
	"abc = defun [pop,pop,pop,*,*,push]"
	 dot = defun { 
		a pop = 
		b pop =

		a 0 ind b 0 ind *
		a 1 ind b 1 ind *
		a 2 ind b 2 ind *
		a 3 ind b 3 ind *
		+ + +
	}
	*/
}

BOOST_AUTO_TEST_CASE(parse_identifier_2)
{
	TokenStream tokens = Represent::parse("4 + pi");
	boost::uint32_t expected[] = {
		TOKEN_BASE_FLAG, 10, TOKEN_NUMBER, 4,
		TOKEN_OPERATOR, OPERATOR_PLUS, 
		TOKEN_IDENTIFIER_RAW, 0, TOKEN_RAW, 'p', TOKEN_RAW, 'i', 
	};

	AUTO_COMPARE(tokens, expected);
}

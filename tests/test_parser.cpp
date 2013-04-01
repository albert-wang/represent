#define BOOST_TEST_MODULE parser test
#include <boost/test/unit_test.hpp>

#include "parser.hpp"

using namespace Represent;

BOOST_AUTO_TEST_CASE(parse_number)
{
	TokenStream tokens = Represent::parse("1");
	TokenStream expected;
	expected << Token(TOKEN_BASE_FLAG, 10) << Token(TOKEN_NUMBER, 1);

	BOOST_CHECK_EQUAL_COLLECTIONS(tokens.begin(), tokens.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(parse_binary)
{
	TokenStream tokens = Represent::parse("0b00001111");
	TokenStream expected;
	expected << Token(TOKEN_BASE_FLAG, 2) << Token(TOKEN_NUMBER, 0) << Token(TOKEN_NUMBER, 0) << Token(TOKEN_NUMBER, 0) << Token(TOKEN_NUMBER, 0) 
		<< Token(TOKEN_NUMBER, 1) << Token(TOKEN_NUMBER, 1) << Token(TOKEN_NUMBER, 1) << Token(TOKEN_NUMBER, 1);
}
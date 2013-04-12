#include <boost/test/unit_test.hpp>
#include <boost/cstdint.hpp>

#include "eval.hpp"

BOOST_AUTO_TEST_CASE(simple_add)
{
	BOOST_CHECK_EQUAL(Represent::evaluateAs<Represent::Value>("1 + 1"), Represent::Value("2"));
}

BOOST_AUTO_TEST_CASE(no_ops)
{
	BOOST_CHECK_EQUAL(Represent::evaluateAs<Represent::Value>("32"), Represent::Value("32"));
}

BOOST_AUTO_TEST_CASE(multiple_adds)
{
	auto result = Represent::evaluateAs<Represent::Value>("1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1");
	BOOST_CHECK_EQUAL(result, Represent::Value("12"));
}

BOOST_AUTO_TEST_CASE(unary_plus)
{
	auto result = Represent::evaluateAs<Represent::Value>("+42");
	BOOST_CHECK_EQUAL(result, Represent::Value("42"));
}

BOOST_AUTO_TEST_CASE(unary_minus)
{
	auto result = Represent::evaluateAs<Represent::Value>("-42");
	BOOST_CHECK_EQUAL(result, Represent::Value("-42"));
}
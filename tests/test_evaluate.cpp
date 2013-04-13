#include <boost/test/unit_test.hpp>
#include <boost/cstdint.hpp>

#include "eval.hpp"
#include "function.hpp"

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

BOOST_AUTO_TEST_CASE(identifier_add)
{
	Represent::EvaluationContext ctx("four + 4");
	ctx.define("four", Represent::Value(4));

	BOOST_CHECK_EQUAL(ctx.evaluateAs<Represent::Value>(), Represent::Value(8));
}

BOOST_AUTO_TEST_CASE(identifier_add_2)
{
	Represent::EvaluationContext ctx("4 + four");
	ctx.define("four", Represent::Value(4));

	BOOST_CHECK_EQUAL(ctx.evaluateAs<Represent::Value>(), Represent::Value(8));
}

BOOST_AUTO_TEST_CASE(only_identifiers)
{
	Represent::EvaluationContext ctx("four + four");
	ctx.define("four", Represent::Value(4));

	BOOST_CHECK_EQUAL(ctx.evaluateAs<Represent::Value>(), Represent::Value(8));	
}

namespace 
{
	using namespace Represent;

	GenericFunction<Increment> incr;
	GenericFunction<Strlen> stringlength;
}
/*
ctx(
	"add = defun { pop pop + push }"
	"add(increment(3), increment(4) + 4)"
	=> 13 
	*/
BOOST_AUTO_TEST_CASE(test_function_call)
{
	Represent::EvaluationContext ctx("increment(4)");
	ctx.define("increment", Function(incr));

	BOOST_CHECK_EQUAL(ctx.evaluateAs<Represent::Value>(), Represent::Value(5));
}

BOOST_AUTO_TEST_CASE(test_function_call_expr)
{
	Represent::EvaluationContext ctx("increment(4 + 4)");
	ctx.define("increment", Function(incr));

	BOOST_CHECK_EQUAL(ctx.evaluateAs<Represent::Value>(), Represent::Value(9));
}

BOOST_AUTO_TEST_CASE(test_function_call_fcall)
{
	Represent::EvaluationContext ctx("increment(increment(5))");
	ctx.define("increment", Function(incr));
	BOOST_CHECK_EQUAL(ctx.evaluateAs<Represent::Value>(), Represent::Value(7));
}

BOOST_AUTO_TEST_CASE(expression_with_function)
{
	Represent::EvaluationContext ctx("-increment(-increment(4))");
	ctx.define("increment", Function(incr));

	BOOST_CHECK_EQUAL(ctx.evaluateAs<Represent::Value>(), Represent::Value(4));
}

BOOST_AUTO_TEST_CASE(expresion_eval_with)
{
	//There is some inaccuracy here when evaluating with a float.
	Represent::EvaluationContext ctx("1 / 10");

	Represent::Value a = ctx.evaluateAsWith<Represent::Value, Represent::Value>();
	Represent::Value b = ctx.evaluateAsWith<Represent::Value, float>();
	BOOST_CHECK(a != b);
}

BOOST_AUTO_TEST_CASE(strlen_works)
{
	Represent::EvaluationContext ctx("strlen(`abc`)");
	ctx.define("strlen", Function(stringlength));

	Represent::Value a = ctx.evaluateAs<Represent::Value>();
	BOOST_CHECK_EQUAL(a, Represent::Value(3));
}

BOOST_AUTO_TEST_CASE(eval_simple_expressio)
{
	Represent::EvaluationContext ctx("42 + -41 / 4 - 3");
	Represent::Value a = ctx.evaluateAs<Represent::Value>();

	ctx.dumpState();
	BOOST_CHECK_EQUAL(a, Value("28.75"));
}
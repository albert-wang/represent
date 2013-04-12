#include "evalutils.hpp"
#include "token.hpp"

namespace Represent
{
	void OutputCell::operator() (const Null& n) const
	{
		std::cout << "NULL";
	}

	void OutputCell::operator() (const Identifier& id) const
	{
		std::cout << "Id[" << id.name << "]";
	}


	void evaluateOperator(boost::uint32_t op, std::vector<EvaluationContext::StorageCell>& stack, EvaluationContext& ctx)
	{
		switch(op)
		{
			case OPERATOR_PLUS: 
			{
				Value a = boost::get<Value>(ctx.lookup(stack.back()));
				stack.pop_back();

				Value b = boost::get<Value>(ctx.lookup(stack.back()));
				stack.pop_back();

				stack.push_back(a + b);
				break;
			}

			case OPERATOR_UNARY_PLUS:
			{
				Value a = boost::get<Value>(ctx.lookup(stack.back()));
				stack.pop_back();

				stack.push_back(+a);
				break;
			}

			case OPERATOR_UNARY_MINUS:
			{
				Value a = boost::get<Value>(ctx.lookup(stack.back()));
				stack.pop_back();

				stack.push_back(-a);
				break;
			}
		}
	}
}
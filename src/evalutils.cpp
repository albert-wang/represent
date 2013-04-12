#include "evalutils.hpp"
#include "token.hpp"

namespace Represent
{
	void evaluateOperator(boost::uint32_t op, std::vector<EvaluationContext::StorageCell>& stack)
	{
		switch(op)
		{
			case OPERATOR_PLUS: 
			{
				Value a = boost::get<Value>(stack.back());
				stack.pop_back();

				boost::get<Value>(stack.back()) += a;
				break;
			}

			case OPERATOR_UNARY_PLUS:
			{
				Value& a = boost::get<Value>(stack.back());
				a = +a;
				break;
			}

			case OPERATOR_UNARY_MINUS:
			{
				Value& a = boost::get<Value>(stack.back());
				a = -a;
				break;
			}
		}
	}
}
#include "eval.hpp"

#pragma once
namespace Represent
{
	struct OutputCell : public boost::static_visitor<>
	{
		template<typename T>
		void operator()(const T& t) const
		{
			std::cout << t;
		}

		void operator()(const Null& n) const;
		void operator()(const Identifier& id) const;
		void operator()(const Function& f) const;
	};

	namespace Detail
	{
		template<typename T, typename Cell>
		T popAs(std::vector<Cell>& stack)
		{
			Cell top = stack.back();
			stack.pop_back();

			return boost::get<T>(top);
		}
	}

	template<typename Value, typename Cell>
	void evaluateOperator(boost::uint32_t op, std::vector<Cell>& stack, EvaluationContext& ctx)
	{
		switch(op)
		{
		case OPERATOR_PLUS: 
			{
				Value a = Detail::popAs<Value>(stack);
				Value b = Detail::popAs<Value>(stack);

				stack.push_back(b + a);
				break;
			}

		case OPERATOR_MINUS:
			{
				Value a = Detail::popAs<Value>(stack);
				Value b = Detail::popAs<Value>(stack);

				stack.push_back(b - a);
				break;
			}

		case OPERATOR_DIVIDE:
			{
				Value a = Detail::popAs<Value>(stack);
				Value b = Detail::popAs<Value>(stack);

				stack.push_back(b / a);
				break;
			}

		case OPERATOR_UNARY_PLUS:
			{
				Value a = Detail::popAs<Value>(stack);
				stack.push_back(+a);
				break;
			}

		case OPERATOR_UNARY_MINUS:
			{
				Value a = Detail::popAs<Value>(stack);
				stack.push_back(-a);
				break;
			}
		}
	}
}
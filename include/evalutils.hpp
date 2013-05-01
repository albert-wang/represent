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

		template<typename Cell>
		Cell pop(std::vector<Cell>& stack)
		{
			Cell top = stack.back();
			stack.pop_back();

			return top;
		}

		template<typename Value, typename Cell>
		struct AddVisitor
			: public boost::static_visitor<Cell> 
		{

			template<typename T, typename U>
			Cell operator()(const T& t, const U& u) const
			{
				std::cout << "Invalid arguments to +: " << typeid(t).name() << " and " << typeid(u).name() << "\n";
				return Null();
			}

			Cell operator()(const Value& a, const Value& b) const
			{
				return Value(a + b);
			}

			Cell operator()(const Math::Vector4<Value>& a, const Value& b) const
			{
				return a + b;
			}

			Cell operator()(const Math::Vector4<Value>& a, const Math::Vector4<Value>& b) const
			{
				return a + b;
			}

			Cell operator()(const std::string& a, const std::string& b) const
			{
				return a + b;
			}
		};
	}

	template<typename Value, typename Cell>
	void evaluateOperator(boost::uint32_t op, std::vector<Cell>& stack, EvaluationContext& ctx)
	{
		switch(op)
		{
		case OPERATOR_PLUS: 
			{
				Cell b = Detail::pop(stack);
				Cell a = Detail::pop(stack);

				Cell result = boost::apply_visitor(Detail::AddVisitor<Value, Cell>(), a, b);
				stack.push_back(result);
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
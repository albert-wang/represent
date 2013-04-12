#include "eval.hpp"

#include <iostream>
#include <vector>

#include "evalutils.hpp"

namespace Represent
{
	namespace
	{
		//Define some operator values.
		size_t power(boost::uint32_t op)
		{
			switch(op)
			{
				case OPERATOR_PLUS: 
				case OPERATOR_MINUS: 
					return 10;

				case OPERATOR_UNARY_PLUS:
				case OPERATOR_UNARY_MINUS: 
					return 90;

				default:
					std::cout << "Unrecognized operator: " << op;
					return 0;
			}
		}

		//0 is left, 1 is right
		size_t assoc(boost::uint32_t op)
		{
			switch(op)
			{
				default: 
					return 0;
			}
		}
	}


	EvaluationContext::EvaluationContext(const std::string& value)
	{
		TokenStream raw = parse(value);

		//Run a simplification pass on the raw token stream, converting numbers into TOKEN_STACK_REFERENCEs.
		stream = simplify(raw, storage);
	}

	EvaluationContext::StorageCell EvaluationContext::evaluate() const
	{
		//Convert to an RPN representation.
		TokenStream rpn = shuntingYard(stream);

		//Setup predefined functions.
		std::vector<StorageCell> stack;

		auto it = rpn.begin();
		while (it != rpn.end())
		{
			if (it->type != TOKEN_STACK_REFERENCE)
			{
				//Evaluate.
				switch(it->type)
				{
					case TOKEN_OPERATOR:
					{
						evaluateOperator(it->value, stack);
						break;
					}
					default:
					{
						std::cout << "Unrecognized token type: " << *it << "\n";
					}
				}
			}
			else
			{
				stack.push_back(storage.at(it->value));
			}

			++it;
		}

		assert(stack.size() == 1);
		return stack.back();
	}

	EvaluationContext::StorageCell evaluate(const std::string& val)
	{
		EvaluationContext context(val);
		return context.evaluate();
	}

	TokenStream shuntingYard(const TokenStream& stream)
	{
		TokenStream rpn;
		std::vector<Token> operatorStack;

		//Shunting Yard to convert to RPN
		auto it = stream.begin();
		while (it != stream.end())
		{
			switch(it->type)
			{
				case TOKEN_STACK_REFERENCE:
				{
					rpn.push(*it);
					break;
				}
				case TOKEN_OPERATOR: 
				{
					size_t currentPower = power(it->value);
					size_t right = assoc(it->value);

					while (!operatorStack.empty())
					{
						Token top = operatorStack.back();
						if (top.type != TOKEN_OPERATOR)
						{
							//Not an operator token on top of stack, do nothing.
							break;
						}

						if ((right == 1 && currentPower < power(top.value)) || 
							(right == 0 && currentPower <= power(top.value)))
						{
							operatorStack.pop_back();
							rpn.push(top);
						}
					}

					operatorStack.push_back(*it);
					break;
				}
				default:
				{
					//Unexpected token :(
				}
			}

			++it;
		}

		while (!operatorStack.empty())
		{
			rpn.push(operatorStack.back());
			operatorStack.pop_back();
		}

		return rpn;
	}


	TokenStream simplify(const TokenStream& stream, std::vector<EvaluationContext::StorageCell>& storage)
	{
		TokenStream result;

		for (auto it = stream.begin(); it != stream.end();)
		{
			//If its a TOKEN_BASE_FLAG, then what follows is a series of numbers
			//which represents a number. Convert it, and push a TOKEN_STACK_REFERENCE instead.
			while (it->type != TOKEN_BASE_FLAG) 
			{
				result.push(*it);
				++it;
			}

			//Similarly, for a TOKEN_IDENTIFIER_RAW, what follows is a series of characters.
			//Convert to a string, and push it as a TOKEN_STACK_REFERENCE 

			boost::uint32_t index = storage.size();

			Value value; 
			it = convert(it, stream.end(), value);
			storage.push_back(value);
			result.push(Token(TOKEN_STACK_REFERENCE, index));
		}

		return result;
	}
}
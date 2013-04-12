#include "eval.hpp"

#include <iostream>
#include <vector>
#include <boost/utility.hpp>

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

		bool typeCheck(const StorageCell& first, const StorageCell& second)
		{
			if (first.which() == second.which())
			{
				return true;
			}

			if (boost::get<Null>(&first))
			{
				return true;
			}

			return false;
		}
	}

	Function::Function(Function::StackFunction f)
		:function(f)
	{}

	EvaluationContext::EvaluationContext(const std::string& value)
	{
		TokenStream raw = parse(value);

		//Run a simplification pass on the raw token stream, converting numbers into TOKEN_STACK_REFERENCEs.
		stream = simplify(raw, storage, identifiers);
	}

	void EvaluationContext::dumpState()
	{
		std::cout << "Storage ====================\n";
		for (size_t i = 0; i < storage.size(); ++i)
		{
			std::cout << i << "\t";
			boost::apply_visitor(OutputCell(), storage[i]);
			std::cout << "\n";
		}

		std::cout << "Identifiers ================\n";
		for (auto it = identifiers.begin(); it != identifiers.end(); ++it)
		{
			std::cout << it->first << " -> " << it->second << "\n";
		}

		std::cout << "Simplified =================\n";
		for (auto it = stream.begin(); it != stream.end(); ++it)
		{
			std::cout << *it << "\n";
		}

		try
		{
			TokenStream rpn = shuntingYard(stream);
			std::cout << "RPN ========================\n";
			for (auto it = rpn.begin(); it != rpn.end(); ++it)
			{
				std::cout << *it << "\n";
			}
		} catch (...)
		{}
	}

	void EvaluationContext::define(const std::string& name, const StorageCell& cell)
	{
		auto it = identifiers.find(name);
		if (it == identifiers.end())
		{
			boost::uint32_t index = storage.size();
			storage.push_back(cell);

			identifiers[name] = index;
		} else {
			if (typeCheck(storage.at(it->second), cell))
			{
				storage.at(it->second) = cell;
			} 
			else
			{
				std::cout << "Could not define: " << name << " due to a type error.\n";
				std::cout << cell.which() << " incompatible with " << storage.at(it->second).which() << "\n";
			}
		}
	}

	StorageCell& EvaluationContext::lookup(StorageCell& cell)
	{
		Identifier * tryIdent = boost::get<Identifier>(&cell);
		if (tryIdent)
		{
			auto it = identifiers.find(tryIdent->name);
			if (it == identifiers.end())
			{
				//ERROR.
				throw 42;
			}

			return lookup(storage.at(it->second));
		}
		else
		{
			return cell;
		}
	}

	StorageCell EvaluationContext::evaluate()
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
						evaluateOperator(it->value, stack, *this);
						break;
					}
					case TOKEN_FUNCTION_IDENTIFIER:
					{
						Function * target = boost::get<Function>(&lookup(storage.at(it->value)));
						if (target->function)
						{
							target->function(target, stack, *this);
						}
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

	StorageCell evaluate(const std::string& val)
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
				case TOKEN_FUNCTION_IDENTIFIER:
				{
					operatorStack.push_back(*it);
					break;
				}
				case TOKEN_ARG_DELIMIT: 
				{
					while (!operatorStack.empty())
					{
						Token top = operatorStack.back();

						//Left paren
						if (top.type == TOKEN_PAREN && top.value == 0)
						{
							break;
						}

						operatorStack.pop_back();
						rpn.push(top);
					}

					if (operatorStack.empty())
					{
						throw "Mismatched parens or misplaced comma";
					}
					break;
				}
				case TOKEN_PAREN:
				{
					//Open paren 
					if (it->value == 0)
					{
						operatorStack.push_back(*it);
					}
					//Close paren
					else if (it->value == 1)
					{
						while (!operatorStack.empty())
						{
							Token top = operatorStack.back();

							//Left paren
							if (top.type == TOKEN_PAREN && top.value == 0)
							{
								break;
							}

							operatorStack.pop_back();
							rpn.push(top);
						}

						if (operatorStack.empty())
						{
							throw "Mismatched parens";
						}

						Token paren = operatorStack.back();
						if (paren.type != TOKEN_PAREN || paren.value != 0)
						{
							throw "Top used to be open paren, but now is not?";
						}

						operatorStack.pop_back();

						if (operatorStack.empty())
						{
							throw "Empty operator stack - no function call token?";
						}

						Token ident = operatorStack.back();
						if (ident.type != TOKEN_FUNCTION_IDENTIFIER)
						{
							throw "This call is not a function.";
						}

						operatorStack.pop_back();
						rpn.push(ident);
					}
					else
					{
						//Error.
						throw "Paren not open nor close?";
					}

					break;
				}
				default:
				{
					//Unexpected token :(
					std::cout << "Unexpected token: " << *it << "\n";
					throw "Unexpected Token";
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

	//This takes in a stream of raw tokens, and then converts things like
	//numbers, identifiers and functions into single tokens so its easier to convert
	//to RPN later on.
	TokenStream simplify(const TokenStream& stream, std::vector<StorageCell>& storage, boost::unordered_map<std::string, boost::uint32_t>& identifiers)
	{
		TokenStream result;

		for (auto it = stream.begin(); it != stream.end();)
		{
			while (it->type != TOKEN_BASE_FLAG
				&& it->type != TOKEN_FUNCTION_IDENTIFIER
				&& it->type != TOKEN_IDENTIFIER_RAW
				&& it != stream.end()
				) 
			{
				result.push(*it);
				++it;
			}

			if (it == stream.end())
			{
				break;
			}

			//If its a TOKEN_BASE_FLAG, then what follows is a series of numbers
			//which represents a number. Convert it, and push a TOKEN_STACK_REFERENCE instead.
			if (it->type == TOKEN_BASE_FLAG)
			{
				boost::uint32_t index = storage.size();

				Value value; 
				it = convert(it, stream.end(), value);
				storage.push_back(value);
				result.push(Token(TOKEN_STACK_REFERENCE, index));
			}
			else if (it->type == TOKEN_FUNCTION_IDENTIFIER)
			{
				Identifier ident;
				it = convertIdentifier(boost::next(it, 1), stream.end(), ident.name);

				auto lookup = identifiers.find(ident.name);
				if (lookup == identifiers.end())
				{
					//Allocate a storage cell for the identifier. 
					boost::uint32_t idIndex = storage.size();
					storage.push_back(Null());

					identifiers[ident.name] = idIndex;
				} 

				boost::uint32_t index = storage.size();
				storage.push_back(ident);
				result.push(Token(TOKEN_FUNCTION_IDENTIFIER, index));
			}
			//Similarly, for a TOKEN_IDENTIFIER_RAW, what follows is a series of characters.
			//Convert to a string, and push it as a TOKEN_STACK_REFERENCE 
			else if (it->type == TOKEN_IDENTIFIER_RAW)
			{
				Identifier ident;
				it = convertIdentifier(it, stream.end(), ident.name);

				auto lookup = identifiers.find(ident.name);
				if (lookup == identifiers.end())
				{
					//Allocate a storage cell for the identifier. 
					boost::uint32_t idIndex = storage.size();
					storage.push_back(Null());

					identifiers[ident.name] = idIndex;
				} 

				boost::uint32_t index = storage.size();
				storage.push_back(ident);
				result.push(Token(TOKEN_STACK_REFERENCE, index));
			}
		}

		return result;
	}
}
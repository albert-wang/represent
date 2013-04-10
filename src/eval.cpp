#include "eval.hpp"

namespace Represent
{
	EvaluationContext::EvaluationContext(const std::string& value)
	{
		TokenStream raw = parse(value);

		//Run a simplification pass on the raw token stream, converting numbers into TOKEN_STACK_REFERENCEs.
		TokenStream simplified = simplify(raw);
	}

	TokenStream EvaluationContext::simplify(const TokenStream& stream)
	{
		TokenStream result;

		for (auto it = stream.begin(); it != stream.end();)
		{
			while (it->type != TOKEN_BASE_FLAG) 
			{
				result.push(*it);
				++it;
			}

			boost::uint32_t index = stack.size();

			Value value; 
			it = convert(it, stream.end(), result);
			stack.push_back(value);
			result.push(Token(TOKEN_STACK_REFERENCE, index));
		}

		return result;
	}

	EvaluationContext::StorageCell EvaluationContext::evaluate() const
	{
		
	}
}
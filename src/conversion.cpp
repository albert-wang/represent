#include "conversion.hpp"

#include <cassert>

namespace Represent
{
	//Converts a number TokenStream into an actual number.
	Value convert(const TokenStream& stream)
	{
		Value result;
		convert(stream.begin(), stream.end(), result);
		return result;
	}

	TokenStream::const_iterator convert(const TokenStream::const_iterator begin, const TokenStream::const_iterator end, Value& out)
	{
		Token first = *begin;
		auto it = begin; 
		it++;

		Value base(first.value);
		Value result(0);

		while (it->type == TOKEN_NUMBER && it != end)
		{
			result *= base;
			result += it->value;
			++it;
		}

		if (it != end && it->type == TOKEN_DECIMAL_POINT) 
		{
			//Decimal values.
			Value currentBase(first.value);

			++it;
			while (it->type == TOKEN_NUMBER && it != end)
			{
				Value v(it->value);
				v /= currentBase;

				result += v;
				currentBase *= currentBase;
				++it;
			}
		}

		out = result;
		return it;
	}
}
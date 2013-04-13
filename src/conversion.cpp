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

	TokenStream::const_iterator convert(TokenStream::const_iterator begin, TokenStream::const_iterator end, Value& out)
	{
		Token first = *begin;
		auto it = begin; 
		it++;

		Value base(first.value);
		Value result(0);

		while (it != end && it->type == TOKEN_NUMBER)
		{
			result *= base;
			result += it->value;
			++it;
		}

		if (it != end && it->type == TOKEN_DECIMAL_POINT) 
		{
			//Decimal values.
			Value base(first.value);
			Value currentBase(first.value);

			++it;
			while (it != end && it->type == TOKEN_NUMBER)
			{
				Value v(it->value);
				v /= currentBase;

				result += v;
				currentBase *= base;
				++it;
			}
		}

		out = result;
		return it;
	}

	TokenStream::const_iterator convertIdentifier(TokenStream::const_iterator begin, TokenStream::const_iterator end, std::string& out)
	{
		++begin;

		out.clear();
		out.reserve(16);

		while (begin != end && begin->type == TOKEN_RAW)
		{
			out.push_back(static_cast<char>(begin->value));
			++begin;
		}

		return begin;
	}

	TokenStream::const_iterator convertString(TokenStream::const_iterator begin, TokenStream::const_iterator end, std::string& out)
	{
		return convertIdentifier(begin, end, out);
	}
}
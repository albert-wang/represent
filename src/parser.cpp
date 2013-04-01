#include "parser.hpp"

#include <cassert>

namespace Represent
{
	int isNumbericInBase(char ch, size_t base)
	{
		const char * numbers = "0123456789ABCDEF";
		
		size_t ind = std::find(numbers, numbers + 16, ch) - numbers;
		if (ind >= base)
		{
			return -1;
		}
		return static_cast<int>(ind);
	}

	size_t begins(const char * begin, const char * end, const std::string& value)
	{
		size_t len = end - begin;
		if (value.size() >= len)
		{
			return 0;
		}

		for (size_t i = 0; i < value.size(); ++i)
		{
			if (value[i] != begin[i])
			{
				return 0;
			}
		}

		return value.size();
	}

	size_t parseNumber(const char * begin, const char * end, size_t base, TokenStream& out)
	{
		assert(base == 2 || base == 8 || base == 10 || base == 16);
		const char * start = begin;

		int val = isNumbericInBase(*begin, base);
		while(val >= 0 && begin != end)
		{
			out.push(Token(TOKEN_NUMBER, val));
			++begin;
			val = isNumbericInBase(*begin, base);
		}

		return begin - start;
	}

	size_t parseOperator(const char * begin, const char * end)
	{
		return 0;
	}

	TokenStream parse(const std::string& data)
	{
		TokenStream result;
		if (data.size() == 0)
		{
			return result;
		}

		const char * begin = &data[0];
		const char * end = &data[0] + data.size();

		//Look for some identifying data to determine the data.
		if (begins(begin, end, "0b")) 
		{
			result.push(Token(TOKEN_BASE_FLAG, 2));
			parseNumber(begin + 2, end, 2, result);
		}
		else if (begins(begin, end, "0x"))
		{
			result.push(Token(TOKEN_BASE_FLAG, 16)); 
			parseNumber(begin + 2, end, 16, result);
		}
		else if (begins(begin, end, "0"))
		{
			result.push(Token(TOKEN_BASE_FLAG, 8));
			parseNumber(begin + 1, end, 8, result);
		}
		else 
		{
			result.push(Token(TOKEN_BASE_FLAG, 10));
			parseNumber(begin, end, 10, result);
		}

		/*
		expression = { part }
		part = value | operator | variable | function 
		value = vector | matrix | quaternion | integer | decimal | hex | octal | constant | string
		operator = ...
		variable = ....
		function = identifier '(' [value {',' value}] ')' 














		*/

		return result;
	}
}
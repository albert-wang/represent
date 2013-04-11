#include "parser.hpp"
#include "parserutils.hpp"
#include "tables.hpp"

#include <algorithm>
#include <cassert>

namespace Represent
{
	static const char DECIMAL_SEPERATOR = '.';
	static const size_t PARSE_FLAGS_FAILURE = 1 << 0; 

#define EXPECT(target, flags, expr) \
	do { if (flags & PARSE_FLAGS_FAILURE) { break; } size_t result = expr; if (result == 0) { flags |= PARSE_FLAGS_FAILURE; } target += result; } while (false)

#define RESTART(target, start, flags, stream) target = start; flags = 0; stream.clear()

	bool success(size_t flags)
	{
		return !(flags & PARSE_FLAGS_FAILURE);
	}

	size_t parseNumberInBase(const char * begin, const char * end, size_t base, TokenStream& out)
	{
		assert(base == 2 || base == 8 || base == 10 || base == 16);
		const char * start = begin;
		bool foundDecimal = false;

		if (*begin == DECIMAL_SEPERATOR) 
		{
			out.push(Token(TOKEN_DECIMAL_POINT, 0)); 
			foundDecimal = true;
		}

		int val = isNumericInBase(*begin, base);
		while(val >= 0 && begin != end)
		{
			out.push(Token(TOKEN_NUMBER, val));
			++begin;

			if (*begin == DECIMAL_SEPERATOR)
			{
				if (foundDecimal)
				{
					break;
				}
				else
				{
					out.push(Token(TOKEN_DECIMAL_POINT, 0));
					foundDecimal = true;
				}
				++begin;
			}

			val = isNumericInBase(*begin, base);
		}

		return begin - start;
	}

	size_t parseNumber(const char * begin, const char * end, TokenStream& out)
	{
		size_t consumed; 
		TokenStream result;
		//Look for some identifying data to determine the data.
		if (begins(begin, end, "0b")) 
		{
			result.push(Token(TOKEN_BASE_FLAG, 2));
			consumed = parseNumberInBase(begin + 2, end, 2, result) + 2;
		}
		else if (begins(begin, end, "0x"))
		{
			result.push(Token(TOKEN_BASE_FLAG, 16)); 
			consumed = parseNumberInBase(begin + 2, end, 16, result) + 2;
		}
		else if (begins(begin, end, "0."))
		{
			result.push(Token(TOKEN_BASE_FLAG, 10)); 
			consumed = parseNumberInBase(begin, end, 10, result);
		}
		else if (begins(begin, end, "0"))
		{
			result.push(Token(TOKEN_BASE_FLAG, 8));
			consumed = parseNumberInBase(begin + 1, end, 8, result) + 1;
		}
		else 
		{
			result.push(Token(TOKEN_BASE_FLAG, 10));
			consumed = parseNumberInBase(begin, end, 10, result);
		}

		out.push(result);
		return consumed;
	}

	size_t parseOperator(const char * begin, const char * end, TokenStream& out)
	{
		TableEntry entries[] = {
			{ "+", TOKEN_OPERATOR, OPERATOR_ADD }, 
			{ "-", TOKEN_OPERATOR, OPERATOR_MINUS }, 
			{ "*", TOKEN_OPERATOR, OPERATOR_MULTIPLY }, 
			{ "/", TOKEN_OPERATOR, OPERATOR_DIVIDE }, 
		}; 

		size_t size = sizeof(entries) / sizeof(entries[0]);
		size_t consumed = 0;
		
		const TableEntry * entry = lookup(entries, entries + size, begin, end, &consumed);
		if (entry)
		{
			out.push(Token(entry->type, entry->value));
		}

		return consumed;
	}

	size_t expression(const char * begin, const char * end, TokenStream& out)
	{
		const char * start = begin;
		boost::uint32_t parsingFlags = 0;
		TokenStream stream;

		//Try a simple expression
		RESTART(begin, start, parsingFlags, stream);
		EXPECT(begin, parsingFlags, parseNumber(begin, end, stream));
		EXPECT(begin, parsingFlags, parseOperator(begin, end, stream));
		EXPECT(begin, parsingFlags, parseNumber(begin, end, stream));
		if (success(parsingFlags))
		{
			out.push(stream);
			return begin - start;
		}

		//Try a single number.
		RESTART(begin, start, parsingFlags, stream);
		EXPECT(begin, parsingFlags, parseNumber(begin, end, stream));
		if (success(parsingFlags))
		{
			out.push(stream);
			return begin - start;
		}

		return 0;
	}

	TokenStream parse(const std::string& data)
	{
		std::vector<char> buffer; 
		std::copy(data.begin(), data.end(), std::back_inserter(buffer));
		buffer.erase(std::remove_if(buffer.begin(), buffer.end(), isspace), buffer.end());

		TokenStream result;
		if (buffer.size() == 0)
		{
			return result;
		}

		const char * begin = &buffer[0];
		const char * end = &buffer[0] + buffer.size();

		expression(begin, end, result);

		//Simplify the expression - convert numbers into stack references, variables into stack references.
		//Evaluate functions
		//Evaluate entire expression.

		/*
		expression = expression | value | variable | function >> binary_operator >>
		part = expression | value | variable | function
		value = vector | matrix | quaternion | integer | decimal | hex | octal | constant | string
		operator = ...
		variable = ....
		function = identifier '(' [expression {',' expression}] ')' 


		*/

		return result;
	}
}
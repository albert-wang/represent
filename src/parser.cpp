#include "parser.hpp"
#include "parserutils.hpp"
#include "tables.hpp"

#include <algorithm>
#include <cassert>

namespace Represent
{
	namespace Parse
	{
		static const char DECIMAL_SEPERATOR = '.';
		static const size_t PARSE_FLAGS_FAILURE = 1 << 0; 
		static const std::string LEGAL_IDENTIFIER_START = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
		static const std::string LEGAL_IDENTIFIER_REST = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";

		void outputStream(const TokenStream& stream)
		{
			for (auto it = stream.begin(); it != stream.end(); ++it)
			{
				std::cout << *it << "\n";
			}

			std::cout << "END\n";
		}

	#define EXPECT(target, flags, expr) \
		do { if (flags & PARSE_FLAGS_FAILURE) { break; } size_t result = expr; if (result == 0) { flags |= PARSE_FLAGS_FAILURE; } target += result; } while (false)

	#define MAYBE(target, flags, expr) \
		do { if (flags & PARSE_FLAGS_FAILURE) { break; } size_t result = expr; target += result; } while (false)

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

		bool charIn(char ch, boost::string_ref legal)
		{
			auto it = std::find(legal.begin(), legal.end(), ch);
			return it != legal.end();
		}

		size_t parseNumber(const char * begin, const char * end, TokenStream& out)
		{
			size_t consumed = 0;
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

		size_t parseChar(const char * begin, const char * end, char ch, TokenType type, boost::uint32_t val, TokenStream& out)
		{
			if (*begin == ch)
			{
				out.push(Token(type, val));
				return 1;
			}

			return 0;
		}

		size_t binaryOperator(const char * begin, const char * end, TokenStream& out)
		{
			TableEntry entries[] = {
				{ "+", TOKEN_OPERATOR, OPERATOR_PLUS }, 
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

		size_t unaryOperator(const char * begin, const char * end, TokenStream& out)
		{
			TableEntry entries[] = {
				{ "+", TOKEN_OPERATOR, OPERATOR_UNARY_PLUS }, 
				{ "-", TOKEN_OPERATOR, OPERATOR_UNARY_MINUS },
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

		size_t function(const char * begin, const char * end, TokenStream& out)
		{
			const char * start = begin;
			boost::uint32_t parsingFlags = 0;
			TokenStream stream;

			RESTART(begin, start, parsingFlags, stream);
			EXPECT(begin, parsingFlags, identifier(begin, end, stream));
			EXPECT(begin, parsingFlags, parseChar(begin, end, '(', TOKEN_PAREN, 0, stream));
			EXPECT(begin, parsingFlags, expression(begin, end, stream));
			while(!parseChar(begin, end, ')', TOKEN_PAREN, 1, stream) && success(parsingFlags))
			{
				EXPECT(begin, parsingFlags, parseChar(begin, end, ',', TOKEN_ARG_DELIMIT, 0, stream));
				EXPECT(begin, parsingFlags, expression(begin, end, stream));
			}


			if (success(parsingFlags))
			{
				//Consume the ')'.
				begin++;
				out.push(Token(TOKEN_FUNCTION_IDENTIFIER, 0));
				out.push(stream);
				return begin - start;
			}

			return 0;
		}

		size_t identifier(const char * begin, const char * end, TokenStream& out)
		{
			const char * start = begin;

			if (charIn(*begin, LEGAL_IDENTIFIER_START))
			{
				TokenStream result;
				result.push(Token(TOKEN_IDENTIFIER_RAW, 0));
				result.push(Token(TOKEN_RAW, *begin));

				++begin;
				while(charIn(*begin, LEGAL_IDENTIFIER_REST) && begin != end)
				{
					result.push(Token(TOKEN_RAW, *begin));
					++begin;
				}

				//Identifiers cannot end with '-'.
				if (*(begin - 1) == '-')
				{
					result.pop();
					begin--;
				}

				out.push(result);
				return begin - start;
			}

			return 0;
		}

		size_t string(const char * begin, const char * end, TokenStream& out)
		{
			const char * start = begin;
			boost::uint32_t flags = 0;
			TokenStream stream;

			RESTART(begin, start, flags, stream);
			EXPECT(begin, flags, parseChar(begin, end, '`', TOKEN_STRING_START, 0, stream));
			if (!success(flags))
			{
				return 0;
			}

			while (begin != end)
			{
				if (*begin == '`')
				{
					if (*(begin - 1) != '\\')
					{
						//Consume the end `.
						++begin;
						break;
					}
				}

				stream.push(Token(TOKEN_RAW, *begin));
				++begin;
			}

			if (success(flags))
			{
				out.push(stream);
				return begin - start;
			}

			return 0;
		}

		size_t value(const char * begin, const char * end, TokenStream& out)
		{
			const char * start = begin;
			boost::uint32_t flags = 0;
			TokenStream stream;

			//Number?
			RESTART(begin, start, flags, stream);
			MAYBE(begin, flags, unaryOperator(begin, end, stream));
			EXPECT(begin, flags, parseNumber(begin, end, stream));
			if (success(flags)) 
			{
				out.push(stream);
				return begin - start;
			}

			//Function call?
			RESTART(begin, start, flags, stream);
			MAYBE(begin, flags, unaryOperator(begin, end, stream));
			EXPECT(begin, flags, function(begin, end, stream));
			if (success(flags))
			{
				out.push(stream);
				return begin - start;
			}

			//String?
			RESTART(begin, start, flags, stream);
			EXPECT(begin, flags, string(begin, end, stream));
			if (success(flags))
			{
				out.push(stream);
				return begin - start;
			}

			//Identifier?
			RESTART(begin, start, flags, stream);
			MAYBE(begin, flags, unaryOperator(begin, end, stream));
			EXPECT(begin, flags, identifier(begin, end, stream));
			if (success(flags))
			{
				out.push(stream);
				return begin - start;
			}

			return 0;
		}

		size_t expression(const char * begin, const char * end, TokenStream& out)
		{
			const char * start = begin;
			boost::uint32_t parsingFlags = 0;
			TokenStream stream;

	#define FINISH() if (success(parsingFlags)) { out.push(stream); return begin - start; }

			//number >> op >> expression
			RESTART(begin, start, parsingFlags, stream);
			EXPECT(begin, parsingFlags, value(begin, end, stream));
			EXPECT(begin, parsingFlags, binaryOperator(begin, end, stream));
			EXPECT(begin, parsingFlags, expression(begin, end, stream));
			FINISH();

			//Try a single value.
			RESTART(begin, start, parsingFlags, stream);
			EXPECT(begin, parsingFlags, value(begin, end, stream));
			FINISH();

			return 0;
		}
	}

	TokenStream parse(const std::string& data)
	{
		std::vector<char> buffer; 
		std::copy(data.begin(), data.end(), std::back_inserter(buffer));
		
		bool inString = false;
		for (size_t i = 0; i < buffer.size(); ++i)
		{
			if (buffer[i] == '`')
			{
				//Escaped backtick.
				if (i > 0 && inString && buffer[i - 1] == '\\')
				{
					continue;
				}

				inString = !inString;
				continue;
			}

			if (isspace(buffer[i]) && !inString)
			{
				buffer[i] = 0;
			}
		}

		buffer.erase(std::remove(buffer.begin(), buffer.end(), 0), buffer.end());

		TokenStream result;
		if (buffer.size() == 0)
		{
			return result;
		}

		const char * begin = &buffer[0];
		const char * end = &buffer[0] + buffer.size();

		size_t consumed = Parse::expression(begin, end, result);
		if (begin + consumed != end)
		{
			//Failure to parse entire input - return empty.
			/*
			std::cout << "INPUT: " << data << "\n";
			std::cout << "Failed to parse entire output array, ended: " << consumed << " chars at " << std::string(begin + consumed, end) << "\n";
			*/
			return TokenStream();
		}

		/*
		expression = (unary_operator?) >> value | variable | function [ >> binary_operator >> unary_operator? >> expression ]

		value = vector | matrix | quaternion | integer | decimal | hex | octal | constant | string
		operator = ...
		variable = ....
		function = identifier '(' [expression {',' expression}] ')' 


		*/

		return result;
	}
}
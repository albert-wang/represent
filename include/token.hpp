#include <string>
#include <iostream>
#include <boost/cstdint.hpp>
#include <vector>

#include "enummaker.hpp"
#pragma once

namespace Represent
{
#define TOKEN_SOURCE 		\
	(TOKEN_BASE_FLAG)       \
	(TOKEN_NUMBER)	 		\
	(TOKEN_DECIMAL_POINT)   \
	(TOKEN_OPERATOR)		\
	(TOKEN_PAREN)           \
	(TOKEN_ARG_DELIMIT)		\
	(TOKEN_FUNCTION_IDENTIFIER) \
	(TOKEN_IDENTIFIER_RAW)	\
	(TOKEN_RAW)				\
	(TOKEN_STRING_START)	\
	(TOKEN_VECTOR)			\
	(TOKEN_VECTOR_DELIMIT)  \
	(TOKEN_STORAGE_REFERENCE)


	MAKE_FULL_ENUM(TokenType, 0, TOKEN_SOURCE);

#define OPERATOR_TYPES		\
	(OPERATOR_PLUS)			\
	(OPERATOR_MINUS)		\
	(OPERATOR_MULTIPLY)		\
	(OPERATOR_DIVIDE)		\
	(OPERATOR_UNARY_PLUS)	\
	(OPERATOR_UNARY_MINUS)

	MAKE_FULL_ENUM(OperatorType, 0, OPERATOR_TYPES);

	struct Token
	{
		Token(TokenType type, boost::uint32_t value);

		bool operator==(const Token& other) const;
		bool operator!=(const Token& other) const;

		TokenType type;
		boost::uint32_t value;
	};
	std::ostream& operator<<(std::ostream& o, const Token& tk);

	class TokenStream
	{
	public:
		typedef std::vector<Token>::iterator iterator;
		typedef std::vector<Token>::const_iterator const_iterator;

		iterator begin();
		const_iterator begin() const;

		iterator end();
		const_iterator end() const;

		void push(const Token& tk);
		void push(const TokenStream& other);
		void pop();
		
		TokenStream& operator<<(const Token& tk);

		const std::vector<Token> getTokens() const;
		void clear();
	private:
		std::vector<Token> tokens;
	};

	std::string toString(TokenType tk);
	std::ostream& operator<<(std::ostream& o, TokenType tk); 
}
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
	(TOKEN_DELIMIT)         \
	(TOKEN_BINARY_OPERATOR)

	MAKE_FULL_ENUM(TokenType, 0, TOKEN_SOURCE);

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
		TokenStream& operator<<(const Token& tk);
	private:
		std::vector<Token> tokens;
	};

	std::string toString(TokenType tk);
	std::ostream& operator<<(std::ostream& o, TokenType tk); 
}
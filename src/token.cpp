#include "token.hpp"

namespace Represent
{
	Token::Token(TokenType type, boost::uint32_t value)
		:type(type)
		,value(value)
	{}

	bool Token::operator==(const Token& tk) const
	{
		return type == tk.type && value == tk.value;
	}

	bool Token::operator!=(const Token& tk) const
	{
		return !((*this) == tk);
	}

	std::ostream& operator<<(std::ostream& o, const Token& tk)
	{
		o << tk.type << "(" << tk.value << ")";
		return o;
	}

	void TokenStream::push(const Token& tk)
	{
		tokens.push_back(tk);
	}

	void TokenStream::push(const TokenStream& other)
	{
		std::copy(other.begin(), other.end(), std::back_inserter(tokens));
	}

	void TokenStream::pop()
	{
		tokens.pop_back();
	}

	TokenStream& TokenStream::operator<<(const Token& tk)
	{
		push(tk);
		return *this;
	}

	TokenStream::iterator TokenStream::begin()
	{
		return tokens.begin();
	}

	TokenStream::const_iterator TokenStream::begin() const
	{
		return tokens.begin();
	}

	TokenStream::iterator TokenStream::end()
	{
		return tokens.end();
	}

	TokenStream::const_iterator TokenStream::end() const
	{
		return tokens.end();
	}

	const std::vector<Token> TokenStream::getTokens() const
	{
		return tokens;
	}

	void TokenStream::clear() 
	{
		return tokens.clear();
	}

	std::string toString(TokenType tk)
	{
		CONVERT_TO_NARROW_STRING(Represent, tk, TOKEN_SOURCE);
	}

	std::ostream& operator<<(std::ostream& o, TokenType tk)
	{
		o << toString(tk); 
		return o;
	}
}
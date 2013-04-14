#include <string>
#include <iostream>
#include <boost/cstdint.hpp>
#include "token.hpp"
#pragma once

namespace Represent
{
	TokenStream parse(const std::string& text);

	//Parts.
	namespace Parse
	{
		size_t expression(const char * begin, const char * end, TokenStream& out);
		size_t function(const char * begin, const char * end, TokenStream& out);
		size_t unaryOperator(const char * begin, const char * end, TokenStream& out);
		size_t binaryOperator(const char * begin, const char * end, TokenStream& out);
		size_t identifier(const char * begin, const char * end, TokenStream& out);
		size_t string(const char * begin, const char * end, TokenStream& out);
		size_t vector(const char * begin, const char * end, TokenStream& out);
	}
}
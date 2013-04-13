#include <boost/multiprecision/cpp_dec_float.hpp>

#include "token.hpp"

namespace Represent
{
	typedef boost::multiprecision::cpp_dec_float_100 Value;

	Value convert(const TokenStream& stream);

	TokenStream::const_iterator convert(TokenStream::const_iterator begin, TokenStream::const_iterator end, Value& out);
	TokenStream::const_iterator convertIdentifier(TokenStream::const_iterator begin, TokenStream::const_iterator end, std::string& out);
	TokenStream::const_iterator convertString(TokenStream::const_iterator begin, TokenStream::const_iterator end, std::string& out);
}
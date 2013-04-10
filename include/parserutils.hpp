#include <boost/utility/string_ref.hpp>

#pragma once
namespace Represent
{
	int isNumericInBase(char ch, size_t base);
	size_t begins(const char * begin, const char * end, boost::string_ref value);
}
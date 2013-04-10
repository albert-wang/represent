#include "parserutils.hpp"
#include <algorithm>
#include <cassert>

namespace Represent
{
	int isNumericInBase(char ch, size_t base)
	{
		const char * numbers = "0123456789ABCDEF";
		
		size_t ind = std::find(numbers, numbers + 16, ch) - numbers;
		if (ind >= base)
		{
			return -1;
		}
		return static_cast<int>(ind);
	}

	size_t begins(const char * begin, const char * end, boost::string_ref value)
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
}
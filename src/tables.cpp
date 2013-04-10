#include "tables.hpp"
#include "parserutils.hpp"

#include <cassert>

namespace Represent
{
	const TableEntry * lookup(const TableEntry * begin, const TableEntry * end, const char * b, const char * e, size_t * consumed)
	{
		assert(begin && end && b && e && consumed);
		*consumed = 0;

		while (begin != end)
		{
			size_t matched = begins(b, e, begin->string);
			if (matched)
			{
				*consumed = matched;
				return begin;
			}
			++begin;
		}

		return NULL;
	}
}
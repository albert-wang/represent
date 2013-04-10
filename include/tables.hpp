#include <boost/cstdint.hpp>
#include "token.hpp"

#pragma once
namespace Represent
{
	struct TableEntry
	{
		const char * string;
		TokenType type;
		boost::uint32_t value;
	};

	const TableEntry * lookup(const TableEntry * begin, const TableEntry * end, const char * b, const char * e, size_t * consumed);
}
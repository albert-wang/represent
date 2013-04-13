#include "evalutils.hpp"
#include "token.hpp"

namespace Represent
{
	void OutputCell::operator() (const Null& n) const
	{
		std::cout << "NULL";
	}

	void OutputCell::operator() (const Identifier& id) const
	{
		std::cout << "Id[" << id.name << "]";
	}

	void OutputCell::operator() (const Function& f) const
	{
		std::cout << "Function[" << f.name << "]";
	}
}
#include "parser.hpp"
#include <vector>

#ifndef TESTING
int main(int argc, char * argv[])
{
	Represent::parse("1+1");
	return 0;
}
#endif
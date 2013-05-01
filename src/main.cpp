#include "parser.hpp"
#include <vector>

#include "vector2.h"

#ifndef TESTING
int main(int argc, char * argv[])
{
	double * d = new double[24];
	double * e = d + 24;

	std::cout << e - d << "\n";
}
#endif
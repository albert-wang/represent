#include <string>
#include <iostream>
#include <boost/cstdint.hpp>
#include "token.hpp"
#pragma once

namespace Represent
{
	TokenStream parse(const std::string& text);
}
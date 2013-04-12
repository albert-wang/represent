#include "eval.hpp"

#pragma once
namespace Represent
{
	void evaluateOperator(boost::uint32_t op, std::vector<EvaluationContext::StorageCell>& stack);
}
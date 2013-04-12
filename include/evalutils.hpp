#include "eval.hpp"


#pragma once
namespace Represent
{
	struct OutputCell : public boost::static_visitor<>
	{
		template<typename T>
		void operator()(const T& t) const
		{
			std::cout << t;
		}

		void operator()(const Null& n) const;
		void operator()(const Identifier& id) const;
	};


	void evaluateOperator(boost::uint32_t op, std::vector<EvaluationContext::StorageCell>& stack, EvaluationContext& ctx);
}
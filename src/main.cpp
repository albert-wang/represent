#include "eval.hpp"
#include "evalutils.hpp"
#include "function.hpp"
#include <sstream>

#ifndef TESTING
template<typename F>
void deffun(Represent::EvaluationContext& ctx, const std::string& name, F f)
{
	Represent::GenericFunction<F> func;
	ctx.define(name, Represent::Function(func));
}

int main(int argc, char * argv[])
{
	std::stringstream s;
	for (size_t i = 1; i < argc; ++i)
	{
		s << ' ' << argv[i]; 
	}

	Represent::EvaluationContext ctx(s.str());

	deffun(ctx, "incr", Represent::Increment());
	deffun(ctx, "strlen", Represent::Strlen());

	ctx.dumpState();

	Represent::StorageCell full = ctx.evaluateWith<Represent::Value>();
	Represent::StorageCell doub = ctx.evaluateWith<double>();
	Represent::StorageCell sing = ctx.evaluateWith<float>();

	std::cout << "Full Precision:   ";
	std::cout << std::setprecision(100);
	boost::apply_visitor(Represent::OutputCell(), full);
	std::cout << "\n";

	std::cout << "Double Precision: ";
	boost::apply_visitor(Represent::OutputCell(), doub);
	std::cout << "\n";

	std::cout << "Single Precision: ";
	boost::apply_visitor(Represent::OutputCell(), sing);
	std::cout << "\n";
}
#endif
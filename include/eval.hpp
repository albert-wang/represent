#include <boost/variant.hpp>

#include "conversion.hpp"
#include "parser.hpp"

namespace Represent
{
	struct EvaluationContext
	{
	public:
		//TODO: Vector4<Value>, Matrix4<Value>, Quaternion<Value>, GUID, Funciton
		typedef boost::variant<Value, std::string> StorageCell;

		explicit EvaluationContext(const std::string& text);

		StorageCell evaluate() const;

		template<typename T>
		T evaluateAs() const
		{
			return boost::get<T>(evaluate());
		}
	private:
		std::vector<StorageCell> storage;
		TokenStream stream;
	};

	EvaluationContext::StorageCell evaluate(const std::string& text);

	template<typename T>
	T evaluateAs(const std::string& text)
	{
		EvaluationContext context(text);
		return context.evaluateAs<T>();
	}	

	//Some utility functions that evaluation context uses.
	TokenStream simplify(const TokenStream& stream, std::vector<EvaluationContext::StorageCell>& storage);
	TokenStream shuntingYard(const TokenStream& stream);
}
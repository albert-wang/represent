#include <boost/variant.hpp>

#include "conversion.hpp"
#include "parser.hpp"

namespace Represent
{
	struct EvaluationContext
	{
	public:
		typedef boost::variant<Value, std::string> StorageCell;

		explicit EvaluationContext(const std::string& text);

		StorageCell evaluate() const;

		template<typename T>
		T evaluateAs() const
		{
			return boost::get<T>(evaluate());
		}
	private:
		std::vector<StorageCell> stack;
		TokenStream stream;
	};



}
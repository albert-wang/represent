#include <boost/variant.hpp>
#include <boost/unordered_map.hpp>

#include "conversion.hpp"
#include "parser.hpp"

#pragma once
namespace Represent
{
	//A string used to lookup in the identifier map.
	struct Identifier
	{
		std::string name;
	};

	//A storage cell with the Null type can be changed to any other type.
	//This is the only type that this is possible on.
	struct Null
	{};

	struct EvaluationContext
	{
	public:
		//TODO: Vector4<Value>, Matrix4<Value>, Quaternion<Value>, GUID, Funciton
		typedef boost::variant<Value, std::string, Identifier, Null> StorageCell;

		explicit EvaluationContext(const std::string& text);

		StorageCell evaluate();

		template<typename T>
		T evaluateAs()
		{
			return boost::get<T>(evaluate());
		}

		void define(const std::string&, const StorageCell& storage);
		void dumpState();

		//Looks in storage for a value.
		StorageCell& lookup(StorageCell& storage);
	private:
		std::vector<StorageCell> storage;
		boost::unordered_map<std::string, boost::uint32_t> identifiers;
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
	TokenStream simplify(const TokenStream& stream, std::vector<EvaluationContext::StorageCell>& storage, boost::unordered_map<std::string, boost::uint32_t>& identifiers);
	TokenStream shuntingYard(const TokenStream& stream);
}
#include <boost/variant.hpp>
#include <boost/unordered_map.hpp>

#include "vector.h"
#include "quaternion.h"
#include "matrix.h"
#include "conversion.hpp"
#include "parser.hpp"

#pragma once
namespace Represent
{
	class EvaluationContext;
	struct Identifier;
	struct Null;
	struct IFunctionImpl;

	struct Function
	{
		explicit Function(IFunctionImpl& impl);

		template<typename T>
		void invoke(std::vector<T>& stack, EvaluationContext& ctx)
		{
			backing->invoke(stack, ctx);
		}

		std::string name;
		IFunctionImpl * backing;
	};

	//A string used to lookup in the identifier map.
	struct Identifier
	{
		std::string name;
	};

	//A storage cell with the Null type can be changed to any other type.
	//This is the only type that this is possible on.
	struct Null
	{};

	//TODO: Vector4<Value>, Matrix4<Value>, Quaternion<Value>, GUID
	template<typename T>
	struct Storage
	{
		typedef boost::variant<T, Math::Vector<T, 4>, Math::Quaternion<T>, Math::Matrix4<T>, std::string, Function, Identifier, Null> type;
	};

	typedef Storage<Value>::type StorageCell;
	typedef Storage<float>::type StorageCellf;
	typedef Storage<double>::type StorageCelld;

	template<typename Cell, typename Backing>
	struct StorageConvert
	{
		struct Converter
			: public boost::static_visitor<>
		{
			Converter(Cell& t)
				:result(&t)
			{}

			template<typename U>
			void operator()(const U& u)
			{
				*result = u;
			}

			void operator()(const Value& v)
			{
				*result = v.template convert_to<Backing>();
			}

			void operator()(const Math::Vector<Value, 4>& v)
			{
				Math::Vector<Backing, 4> t;
				t[0] = v[0].template convert_to<Backing>();
				t[1] = v[1].template convert_to<Backing>();
				t[2] = v[2].template convert_to<Backing>();
				t[3] = v[3].template convert_to<Backing>();

				*result = t;
			}

			template<typename U>
			void operator()(const Math::Vector<U, 4>& v)
			{
				Math::Vector<Backing, 4> t;
				t[0] = v[0];
				t[1] = v[1];
				t[2] = v[2];
				t[3] = v[3];

				*result = t;
			}

			void operator()(const Math::Quaternion<Value>& v)
			{
				Math::Quaternion<Backing> t;

				t.w = v.w.template convert_to<Backing>();
				t.x = v.x.template convert_to<Backing>();
				t.y = v.y.template convert_to<Backing>();
				t.z = v.z.template convert_to<Backing>();

				*result = t;
			}

			template<typename U>
			void operator()(const Math::Quaternion<U>& v)
			{
				Math::Quaternion<Backing> t;

				t.w = v.w;
				t.x = v.x;
				t.y = v.y;
				t.z = v.z;

				*result = t;
			}

			void operator()(const Math::Matrix4<Value>& v)
			{}

			template<typename U>
			void operator()(const Math::Matrix4<U>& v)
			{}

			Cell * result;
		};

		template<typename U>
		static Cell convert(const U& other)
		{
			Cell result;
			Converter st(result);

			boost::apply_visitor(st, other);

			return result;
		}
	};

	//Implements a function.
	struct IFunctionImpl
	{
		virtual void invoke(std::vector<StorageCell>& stack, EvaluationContext& ctx) = 0;
		virtual void invoke(std::vector<StorageCelld>& stack, EvaluationContext& ctx) = 0;
		virtual void invoke(std::vector<StorageCellf>& stack, EvaluationContext& ctx) = 0;
	};

	class EvaluationContext
	{
	public:
		explicit EvaluationContext(const std::string& text);

		StorageCell evaluate();

		template<typename T>
		T evaluateAs()
		{
			return boost::get<T>(evaluate());
		}

		template<typename T, typename Storage>
		T evaluateAsWith()
		{
			TokenStream rpn = shuntingYard(stream);
			return boost::get<T>(evaluateWith<Storage>(rpn));
		}

		void define(const std::string&, const StorageCell& storage);
		void dumpState();

		//Looks in storage for a value.
		StorageCell& lookup(StorageCell& storage);
	private:

		//Evaluates an expression with T instead of Value to store intermediates.
		//This allows one to determine accuracy loss between computations.
		template<typename T>
		StorageCell evaluateWith(const TokenStream& rpn)
		{
			typedef Storage<T>::type Cell;
			std::vector<Cell> typedStorage;
			typedStorage.reserve(storage.size());

			for (size_t i = 0; i < typedStorage.size(); ++i)
			{
				typedStorage.push_back(StorageConvert<Cell, T>::convert(storage[i]));
			}

			//Setup predefined functions.
			std::vector<Cell> stack;

			auto it = rpn.begin();
			while (it != rpn.end())
			{
				switch (it->type)
				{
				case TOKEN_STORAGE_REFERENCE:
					{
						stack.push_back(StorageConvert<Cell, T>::convert(lookup(storage.at(it->value))));
						break;
					}
				case TOKEN_OPERATOR:
					{
						evaluateOperator<T>(it->value, stack, *this);
						break;
					}
				case TOKEN_FUNCTION_IDENTIFIER:
					{
						Function * target = boost::get<Function>(&lookup(storage.at(it->value)));
						assert(target);

						target->invoke(stack, *this);
						break;
					}
				default:
					{
						std::cout << "Unrecognized token type: " << *it << "\n";
					}
				}
				++it;
			}

			assert(stack.size() == 1);
			return StorageConvert<StorageCell, Value>::convert(stack.back());
		}

		std::vector<StorageCell> storage;
		boost::unordered_map<std::string, boost::uint32_t> identifiers;
		TokenStream stream;
	};

	StorageCell evaluate(const std::string& text);

	template<typename T>
	T evaluateAs(const std::string& text)
	{
		EvaluationContext context(text);
		return context.evaluateAs<T>();
	}

	//Some utility functions that evaluation context uses.
	TokenStream simplify(const TokenStream& stream, std::vector<StorageCell>& storage, boost::unordered_map<std::string, boost::uint32_t>& identifiers);
	TokenStream shuntingYard(const TokenStream& stream);
}
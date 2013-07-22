#include <boost/variant.hpp>
#include <boost/unordered_map.hpp>
#include <vector>

#include "vector2.h"
#include "quaternion.h"
#include "matrix.h"
#include "conversion.hpp"
#include "parser.hpp"

#pragma once
namespace Represent
{	
	namespace Detail
	{
		template<typename T, typename Cell>
		T popAs(std::vector<Cell>& stack)
		{
			Cell top = stack.back();
			stack.pop_back();

			return boost::get<T>(top);
		}

		template<typename Cell>
		Cell pop(std::vector<Cell>& stack)
		{
			Cell top = stack.back();
			stack.pop_back();

			return top;
		}
	}

	class EvaluationContext;
	struct Identifier;
	struct Null;
	struct IFunctionImpl;

	//Forward decls.
	template<typename T>
	void invokeBacking(IFunctionImpl * backing, std::vector<T>& stack, EvaluationContext& ctx, size_t arity);

	template<typename Value, typename Cell>
	void evaluateOperator(boost::uint32_t op, std::vector<Cell>& stack, EvaluationContext& ctx);

	struct Function
	{
		explicit Function(IFunctionImpl& impl);

		template<typename T>
		void invoke(std::vector<T>& stack, EvaluationContext& ctx, size_t arity)
		{
			invokeBacking(backing, stack, ctx, arity);
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
		typedef typename boost::make_recursive_variant<
			T, Math::Vector4<T>, Math::Quaternion<T>, Math::Matrix4<T>, std::vector< boost::recursive_variant_ >, std::string, Function, Identifier, Null
		>::type type;
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

			template<typename U>
			void operator()(const std::vector<U>& u)
			{}

			void operator()(const Value& v)
			{
				*result = v.template convert_to<Backing>();
			}

			void operator()(const Math::Vector4<Value>& v)
			{
				Math::Vector4<Backing> t;
				t[0] = v[0].template convert_to<Backing>();
				t[1] = v[1].template convert_to<Backing>();
				t[2] = v[2].template convert_to<Backing>();
				t[3] = v[3].template convert_to<Backing>();

				*result = t;
			}

			template<typename U>
			void operator()(const Math::Vector4<U>& v)
			{
				Math::Vector4<Backing> t;
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
		virtual void invoke(std::vector<StorageCell>& stack, EvaluationContext& ctx, size_t arity) = 0;
		virtual void invoke(std::vector<StorageCelld>& stack, EvaluationContext& ctx, size_t arity) = 0;
		virtual void invoke(std::vector<StorageCellf>& stack, EvaluationContext& ctx, size_t arity) = 0;
	};


	//Some utility functions that evaluation context uses.
	TokenStream simplify(const TokenStream& stream, std::vector<StorageCell>& storage, boost::unordered_map<std::string, boost::uint32_t>& identifiers);
	TokenStream shuntingYard(const TokenStream& stream);

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

		template<typename Storage>
		StorageCell evaluateWith()
		{
			TokenStream rpn = shuntingYard(stream);
			return evaluateWith<Storage>(rpn);
		}

		void define(const std::string&, const StorageCell& storage);
		void dumpState();

		//Looks in storage for a value.
		StorageCell& lookup(StorageCell& storage);
	private:
		Function * functionLookup(const std::string& name);

		//Evaluates an expression with T instead of Value to store intermediates.
		//This allows one to determine accuracy loss between computations.
		template<typename T>
		StorageCell evaluateWith(const TokenStream& rpn)
		{
			using namespace Detail;

			typedef typename Storage<T>::type Cell;
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
				case TOKEN_RAW_VALUE:
					{
						stack.push_back(StorageConvert<Cell, T>::convert(Cell(it->value)));
						break;
					}
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

						target->invoke(stack, *this, it->extra);
						break;
					}
				case TOKEN_VECTOR:
					{
						T w = Detail::popAs<T>(stack);
						T z = Detail::popAs<T>(stack);
						T y = Detail::popAs<T>(stack);
						T x = Detail::popAs<T>(stack);

						stack.push_back(Math::Vector4<T>(x, y, z, w));
						break;
					}
				case TOKEN_QUATERNION:
					{
						Math::Quaternion<T> quat;

						quat.z = Detail::popAs<T>(stack);
						quat.y = Detail::popAs<T>(stack);
						quat.x = Detail::popAs<T>(stack);
						quat.w = Detail::popAs<T>(stack);

						stack.push_back(quat);
						break;
					}
				case TOKEN_MATRIX:
					{
						Math::Vector4<T> d = Detail::popAs<Math::Vector4<T> >(stack);
						Math::Vector4<T> c = Detail::popAs<Math::Vector4<T> >(stack);
						Math::Vector4<T> b = Detail::popAs<Math::Vector4<T> >(stack);
						Math::Vector4<T> a = Detail::popAs<Math::Vector4<T> >(stack);

						Math::Matrix4<T> mat;
						mat(0, 0) = a[0]; mat(0, 1) = a[1]; mat(0, 2) = a[2]; mat(0, 3) = a[3];
						mat(1, 0) = b[0]; mat(1, 1) = b[1]; mat(1, 2) = b[2]; mat(1, 3) = b[3];
						mat(2, 0) = c[0]; mat(2, 1) = c[1]; mat(2, 2) = c[2]; mat(2, 3) = c[3];
						mat(3, 0) = d[0]; mat(3, 1) = d[1]; mat(3, 2) = d[2]; mat(3, 3) = d[3];
						stack.push_back(mat);

						break;
					}
				case TOKEN_ARRAY:
					{
						boost::uint32_t count = it->value;
						int typeValue = stack.back().which();

						std::vector<Cell> result;
						for (size_t i = 0; i < count; ++i)
						{
							result.push_back(stack.back());
							if (result.back().which() != typeValue)
							{
								throw "Types in the vector not the same!";
							}

							stack.pop_back();
						}

						std::reverse(result.begin(), result.end());
						stack.push_back(result);
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

	template<typename T>
	void invokeBacking(IFunctionImpl * backing, std::vector<T>& stack, EvaluationContext& ctx, size_t arity)
	{
		backing->invoke(stack, ctx, arity);
	}
}
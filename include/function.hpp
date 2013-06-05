#include "eval.hpp"
#include "evalutils.hpp"

#pragma once
namespace Represent
{
	template<typename Impl>
	struct GenericFunction : public IFunctionImpl
	{
		virtual void invoke(std::vector<StorageCell>& stack, EvaluationContext& ctx)
		{
			return Impl::template invoke<Value, StorageCell>(stack, ctx);
		}

		virtual void invoke(std::vector<StorageCelld>& stack, EvaluationContext& ctx)
		{
			return Impl::template invoke<double, StorageCelld>(stack, ctx);
		}

		virtual void invoke(std::vector<StorageCellf>& stack, EvaluationContext& ctx)
		{
			return Impl::template invoke<float, StorageCellf>(stack, ctx);
		}
	};

	struct Increment
	{
		template<typename T, typename Cell>
		static void invoke(std::vector<Cell>& cell, EvaluationContext& ctx)
		{
			T scalar = Detail::popAs<T>(cell);
			cell.push_back(scalar + 1);
		}
	};

	struct Strlen
	{
		template<typename T, typename Cell>
		static void invoke(std::vector<Cell>& cell, EvaluationContext& ctx)
		{
			std::string scalar = Detail::popAs<std::string>(cell);
			cell.push_back(T(scalar.length()));
		}
	};

	struct Duplicate
	{
		template<typename T, typename Cell>
		static void invoke(std::vector<Cell>& cell, EvaluationContext& ctx)
		{
			Cell top = cell.back();
			cell.push_back(top);
		}
	};

	struct MakeQuaternion
	{
		template<typename T, typename Cell>
		static void invoke(std::vector<Cell>& cell, EvaluationContext& ctx)
		{
			Math::Quaternion<T> quat;

			quat.z = Detail::popAs<T>(cell);
			quat.y = Detail::popAs<T>(cell);
			quat.x = Detail::popAs<T>(cell);
			quat.w = Detail::popAs<T>(cell);

			cell.push_back(quat);
		}
	};
}
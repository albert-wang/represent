#ifndef VECTOR_HPP_INCLUDED
#define VECTOR_HPP_INCLUDED
#pragma once

#include <cassert>
#include <boost/preprocessor.hpp>
#include <iostream>

#if !defined(_WIN32) && !defined(__declspec)
#define __declspec(X) 
#endif

namespace Math
{
	template<typename Storage, size_t Length> 
	struct VectorType 
	{};

#define VECTOR_COMMON(Storage, elements, Length) \
	typedef Storage value_type; typedef Storage* iterator; typedef Storage const * const_iterator; \
	size_t size() const { return Length; }													\
	Storage& operator[](size_t i) { assert(i < Length); return elements[i]; }				\
	const Storage& operator[](size_t i) const { assert(i < Length); return elements[i]; } 	\
	iterator begin() { return elements; } iterator end() { return elements + Length; }		\
	const_iterator begin() const { return elements; }										\
	const_iterator end() const { return elements + Length; }								\
	template<typename U> VectorType<Storage, U::ResultLength> operator|(const U& function) const \
	{ return function(*this); }

#define VECTOR_COMPARE2(op)					\
	bool operator op(const Vector2& other) const { return (elements[0] op other[0]) && (elements[1] op other[1]); }

#define VECTOR_SCALAR_MATH2(op)					\
	Vector2& operator op(Storage st) { elements[0] op st; elements[1] op st; return *this; }

#define VECTOR_VECTOR_MATH2(op)					\
	Vector2& operator op(const Vector2& other) { elements[0] op other.elements[0]; elements[1] op other.elements[1]; return *this; }

#define VECTOR_MATH2() \
	VECTOR_SCALAR_MATH2(+=) VECTOR_SCALAR_MATH2(*=) VECTOR_SCALAR_MATH2(-=) VECTOR_SCALAR_MATH2(/=) VECTOR_VECTOR_MATH2(+=) VECTOR_VECTOR_MATH2(-=) VECTOR_VECTOR_MATH2(*=) VECTOR_VECTOR_MATH2(/=)

#define VECTOR_ALL_COMPARE2() \
	VECTOR_COMPARE2(==) VECTOR_COMPARE2(!=) VECTOR_COMPARE2(>) VECTOR_COMPARE2(<) VECTOR_COMPARE2(>=) VECTOR_COMPARE2(<=)

	template<typename Storage>
	class Vector2
	{
	public:
		static const size_t Length = 2;

		Vector2() {}
		Vector2(Storage x, Storage y) 
		{
			elements[0] = x;
			elements[1] = y;
		}

		VECTOR_COMMON(Storage, elements, Length)
		VECTOR_ALL_COMPARE2()
		VECTOR_MATH2()
	private:
		Storage elements[2];
	};

	template<typename T>
	struct VectorType<T, 2>
	{
		typedef Vector2<T> type;
	};

#define VECTOR_COMPARE3(op)					\
	bool operator op(const Vector3& other) const { return (elements[0] op other[0]) && (elements[1] op other[1]) && (elements[2] op other[2]); }

#define VECTOR_SCALAR_MATH3(op)					\
	Vector3& operator op(Storage st) { elements[0] op st; elements[1] op st; elements[2] op st; return *this; }

#define VECTOR_VECTOR_MATH3(op)					\
	Vector3& operator op(const Vector3& other) { elements[0] op other.elements[0]; elements[1] op other.elements[1]; elements[2] op other.elements[2]; return *this; }

#define VECTOR_MATH3() \
	VECTOR_SCALAR_MATH3(+=) VECTOR_SCALAR_MATH3(*=) VECTOR_SCALAR_MATH3(-=) VECTOR_SCALAR_MATH3(/=) VECTOR_VECTOR_MATH3(+=) VECTOR_VECTOR_MATH3(-=) VECTOR_VECTOR_MATH3(*=) VECTOR_VECTOR_MATH3(/=)

#define VECTOR_ALL_COMPARE3() \
	VECTOR_COMPARE3(==) VECTOR_COMPARE3(!=) VECTOR_COMPARE3(>) VECTOR_COMPARE3(<) VECTOR_COMPARE3(>=) VECTOR_COMPARE3(<=)

	template<typename Storage>
	class Vector3
	{
	public:
		static const size_t Length = 3;

		Vector3() {}
		Vector3(Storage x, Storage y, Storage z)
		{
			elements[0] = x;
			elements[1] = y;
			elements[2] = z;
		}

		Vector3(Storage x, const Vector2<Storage>& other)
		{
			elements[0] = x;
			elements[1] = other[0];
			elements[2] = other[1];
		}

		Vector3(const Vector2<Storage>& other, Storage z)
		{
			elements[0] = other[0];
			elements[1] = other[1];
			elements[2] = z;
		}

		VECTOR_COMMON(Storage, elements, Length);
		VECTOR_ALL_COMPARE3();
		VECTOR_MATH3();
	private:
		Storage elements[3];
	};

	template<typename T>
	struct VectorType<T, 3>
	{
		typedef Vector3<T> type;
	};


#define VECTOR_COMPARE4(op)					\
	bool operator op(const Vector4& other) const { return (elements[0] op other[0]) && (elements[1] op other[1]) && (elements[2] op other[2]) && (elements[3] op other[3]); }

#define VECTOR_SCALAR_MATH4(op)					\
	Vector4& operator op(Storage st) { elements[0] op st; elements[1] op st; elements[2] op st; elements[3] op st; return *this; }

#define VECTOR_VECTOR_MATH4(op)					\
	Vector4& operator op(const Vector4& other) { elements[0] op other.elements[0]; elements[1] op other.elements[1]; elements[2] op other.elements[2]; elements[3] op other.elements[3]; return *this; }

#define VECTOR_MATH4() \
	VECTOR_SCALAR_MATH4(+=) VECTOR_SCALAR_MATH4(*=) VECTOR_SCALAR_MATH4(-=) VECTOR_SCALAR_MATH4(/=) \
	VECTOR_VECTOR_MATH4(+=) VECTOR_VECTOR_MATH4(-=) VECTOR_VECTOR_MATH4(*=) VECTOR_VECTOR_MATH4(/=)

#define VECTOR_ALL_COMPARE4() \
	VECTOR_COMPARE4(==) VECTOR_COMPARE4(!=) VECTOR_COMPARE4(>) VECTOR_COMPARE4(<) VECTOR_COMPARE4(>=) VECTOR_COMPARE4(<=)

	template<typename Storage>
	class Vector4
	{
	public:
		static const size_t Length = 4;

		Vector4() {}
		Vector4(Storage x, Storage y, Storage z, Storage w = Storage(0))
		{
			elements[0] = x;
			elements[1] = y;
			elements[2] = z;
			elements[3] = w;
		}

		Vector4(Storage x, const Vector2<Storage>& other)
		{
			elements[0] = x;
			elements[1] = other[0];
			elements[2] = other[1];
			elements[3] = Storage(0);
		}

		Vector4(const Vector2<Storage>& other, Storage z)
		{
			elements[0] = other[0];
			elements[1] = other[1];
			elements[2] = z;
			elements[3] = Storage(0);
		}

		Vector4(const Vector2<Storage>& first, const Vector2<Storage>& second)
		{
			elements[0] = first[0];
			elements[1] = first[1];

			elements[2] = second[0];
			elements[3] = second[1];
		}

		Vector4(const Vector3<Storage>& other, Storage s)
		{
			elements[0] = other[0];
			elements[1] = other[1];
			elements[2] = other[2];
			elements[3] = s;	
		}

		Vector4(Storage s, const Vector3<Storage>& other)
		{
			elements[0] = s;
			elements[1] = other[0];
			elements[2] = other[1];
			elements[3] = other[2];
		}

		VECTOR_COMMON(Storage, elements, Length);
		VECTOR_ALL_COMPARE4();
		VECTOR_MATH4();
	private:
		Storage elements[4];
	};

	template<typename T>
	struct VectorType<T, 4>
	{
		typedef Vector4<T> type;
	};


#define VECTOR_FREE_SCALAR_IMPL(op, n) \
	template<typename T>			\
	BOOST_PP_CAT(Vector, n)<T> operator op(const BOOST_PP_CAT(Vector, n)<T>& l, T r) { BOOST_PP_CAT(Vector, n)<T> temp(l); temp BOOST_PP_CAT(op, =) r; return temp; }

	VECTOR_FREE_SCALAR_IMPL(+, 2) VECTOR_FREE_SCALAR_IMPL(*, 2) VECTOR_FREE_SCALAR_IMPL(/, 2) VECTOR_FREE_SCALAR_IMPL(-, 2)
	VECTOR_FREE_SCALAR_IMPL(+, 3) VECTOR_FREE_SCALAR_IMPL(*, 3) VECTOR_FREE_SCALAR_IMPL(/, 3) VECTOR_FREE_SCALAR_IMPL(-, 3)
	VECTOR_FREE_SCALAR_IMPL(+, 4) VECTOR_FREE_SCALAR_IMPL(*, 4) VECTOR_FREE_SCALAR_IMPL(/, 4) VECTOR_FREE_SCALAR_IMPL(-, 4)

#define VECTOR_FREE_VECTOR_IMPL(op, n)	\
	template<typename T>				\
	BOOST_PP_CAT(Vector, n)<T> operator op(const BOOST_PP_CAT(Vector, n)<T>& l, const BOOST_PP_CAT(Vector, n)<T>& r) { BOOST_PP_CAT(Vector, n)<T> temp(l); temp BOOST_PP_CAT(op, =) r; return temp; }

	VECTOR_FREE_VECTOR_IMPL(+, 2) VECTOR_FREE_VECTOR_IMPL(-, 2) VECTOR_FREE_VECTOR_IMPL(*, 2) VECTOR_FREE_VECTOR_IMPL(/, 2)
	VECTOR_FREE_VECTOR_IMPL(+, 3) VECTOR_FREE_VECTOR_IMPL(-, 3) VECTOR_FREE_VECTOR_IMPL(*, 3) VECTOR_FREE_VECTOR_IMPL(/, 3)
	VECTOR_FREE_VECTOR_IMPL(+, 4) VECTOR_FREE_VECTOR_IMPL(-, 4) VECTOR_FREE_VECTOR_IMPL(*, 4) VECTOR_FREE_VECTOR_IMPL(/, 4)

	template<typename T, size_t L>
	T dot(const typename VectorType<T, L>::type& a, const typename VectorType<T, L>::type& b)
	{
		T result = T(0);
		for (size_t i = 0; i < L; ++i)
		{
			result += a[i] * b[i];
		}

		return result;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& o, const Vector4<T>& t)
	{
		o << "[";
		for (size_t i = 0; i < 4; ++i)
		{
			if (i)
			{
				o << ", ";
			}
			o << t[i];
		}

		o << "]";
		return o;
	}
}

#endif
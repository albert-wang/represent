#ifndef VECTOR_HPP_INCLUDED
#define VECTOR_HPP_INCLUDED

#include <boost/static_assert.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/or.hpp>
#include <cassert>
#include <immintrin.h>
#include <xmmintrin.h>
#include <iostream>

#if !defined(_WIN32) && !defined(__declspec)
//Alright, declspecs should be handled on a per-compiler basis
//But i'm lazy.
#define __declspec(X) 

#endif

#pragma once
#pragma warning(push)
#pragma warning(disable: 4127) //Conditional expression is constant.

class Matrix2;
class Matrix3;
class Matrix4;

//Utility Macros
#define VECTOR_COMPARE_IMPL(other, op) \
	bool result = true;												\
	result = result && (elements[0] op other[0]);					\
	result = result && (elements[1] op other[1]);					\
	if (len >= 3) result = result && (elements[2] op other[2]);		\
	if (len == 4) result = result && (elements[3] op other[3]);		\
	return result

#define VECTOR_MATH_IMPL(other, op) \
	elements[0] op other[0];										\
	elements[1] op other[1];										\
	if (len >= 3) elements[2] op other[2];							\
	if (len == 4) elements[3] op other[3];							\
	return *this

#define VECTOR_MATH_SCALAR_IMPL(other, op) \
	elements[0] op other;											\
	elements[1] op other;											\
	if (len >= 3) elements[2] op other;								\
	if (len == 4) elements[3] op other;								\
	return *this

namespace Math
{
	/*
		Vector4 acts exactly like a Vector3 for the following purposes:
			constructor
			math operations

		And like a 4-element tuple for the following operations:
			constructor
			iterators
			indexing
			logical operations (==, !=)
	*/

	namespace Detail
	{
		template<typename U, size_t len>
		void copy( const U * begin, const U *, U * dest )
		{
			dest[0] = begin[0];
			dest[1] = begin[1];
			if (len >= 3)
			{
				dest[2] = begin[2];
			}

			if (len == 4)
			{
				dest[3] = begin[3];
			}
		}

		//Implement some basic functionality
		template<typename Vec, typename Value>
		class VectorBase
		{
			typedef Value return_type;
		public:
#pragma warning(push)
#pragma warning(disable: 4996) //Checked iterators
			void swap(Vec& other)
			{
				std::swap_ranges(begin(), end(), other.begin());
			}
#pragma warning(pop)

			return_type dot( const Vec& other ) const
			{
				return_type result = return_type(0.0f);
				const Vec& self = *static_cast<const Vec *>(this);

				result += self[0] * other[0];
				result += self[1] * other[1];
				if (Vec::Length >= 3)
				{
					result += self[2] * other[2];
				}

				if (Vec::Length == 4)
				{
					result += self[3] * other[3];
				}

				return result;
			}

			float getMagnitude() const
			{
				return_type sqrMagnitude = dot(*static_cast<const Vec *>(this));
				return std::sqrt(static_cast<float>(sqrMagnitude));
			}

			return_type squareDistanceBetween(const Vec& other) const
			{
				Vec distance = *static_cast<const Vec *>(this) - other;
				return distance.dot(distance);
			}

			float distanceBetween(const Vec& other) const
			{
				return std::sqrt(static_cast<float>(squareDistanceBetween(other)));
			}

			void normalize()
			{
				float mag = getMagnitude();

				if (mag < 0.000001f || mag == 1.0f)
				{
					return;
				}

				const Vec& self = *static_cast<const Vec *>(this);

				self[0] /= mag;
				self[1] /= mag;
				if (len >= 3)
				{
					self[2] /= mag;
				}
			}

			Vec unit() const
			{
				Vec result(*static_cast<const Vec *>(this));
				result.normalize();
				return result;
			}

			Vec cross(const Vec& rhv) const
			{
				BOOST_STATIC_ASSERT(Vec::Length != 2);

				const Vec& self = *static_cast<const Vec *>(this);
				Vec result;
				return_type x = self[1] * rhv[2] - self[2] * rhv[1];
				return_type y = self[2] * rhv[0] - self[0] * rhv[2];
				return_type z = self[0] * rhv[1] - self[1] * rhv[0];

				result[0] = x;
				result[1] = y;
				result[2] = z;

				return result;
			}

			Vec perpendicularTo() const
			{
				Vec result = cross( Vec(0, 0, 1) );
				if (result.dot(result) == 0)
				{
					result = cross( Vec(0, 1, 0) );
				}
				return result;
			}

			void homogeneousDivide()
			{
				BOOST_STATIC_ASSERT(len == 4);
				Vec& self = *static_cast<Vec *>(this);

				return_type inv = return_type(1.0f) / elements[3];

				self[0] *= inv;
				self[1] *= inv;
				self[2] *= inv;
				self[3] = 1;
			}
		};
	}

	template<typename T, size_t len>
	class Vector : public Detail::VectorBase< Vector<T, len>, T>
	{
		BOOST_STATIC_ASSERT( len == 2 || len == 3 || len == 4 );
	public:
		typedef T value_type;
		typedef T* iterator;
		typedef T const * const_iterator;

		enum { Length = len };

		//The augmentation constructors
		Vector(const Vector<T, 2>& v, T z)
		{
			Detail::copy<T, 2>(v.begin(), v.end(), begin());
			elements[2] = z;
		}

		Vector(T x, const Vector<T, 2>& v)
		{
			elements[0] = x;
			Detail::copy<T, 2>(v.begin(), v.end(), begin() + 1 );
		}

		Vector(const Vector<T, 2>&v, T z, T w)
		{
			Detail::copy<T, 2>(v.begin(), v.end(), begin());
			elements[2] = z;
			elements[3] = w;
		}

		Vector(T x, const Vector<T, 2>& v, T w)
		{
			elements[0] = x;
			Detail::copy<T, 2>( v.begin(), v.end(), begin() + 1 );
			elements[3] = w;
		}

		Vector(T x, T y, const Vector<T, 2>& v)
		{
			elements[0] = x;
			elements[1] = y;
			Detail::copy<T, 2>( v.begin(), v.end(), begin() + 2 );
		}

		Vector(const Vector<T, 3>& v, T w)
		{
			Detail::copy<T, 3>(v.begin(), v.end(), begin() );
			elements[3] = w;
		}

		Vector( T x, const Vector<T, 3>& v)
		{
			elements[0] = x;
			Detail::copy<T, 3>( v.begin(), v.end(), begin() + 1 );
		}

		//Normal constructors.
		Vector( T x, T y )
		{
			elements[0] = x;
			elements[1] = y;

			if (len == 4) { elements[3] = T(0); }
		}

		Vector( T x, T y, T z)
		{
			elements[0] = x;
			elements[1] = y;
			elements[2] = z;

			if (len == 4) { elements[3] = T(0); }
		}

		Vector( T x, T y, T z, T w)
		{
			elements[0] = x;
			elements[1] = y;
			elements[2] = z;
			elements[3] = w;
		}

		Vector()
		{
			if (len == 4) { elements[3] = T(0); }
		}

		//Copy constructor
		Vector(const Vector& other)
		{
			Detail::copy<T, len>(other.begin(), other.end(), begin());
		}

		size_t size() const 
		{
			return len;
		}
		
		const T * data() const
		{
			return elements;
		}

		//The View operator.
		template<typename U>
		Vector<T, U::ResultLength> operator|( const U& function ) const
		{
			return function(*this);
		}

		T& operator[](size_t index)
		{
			assert(index < len);
			return elements[index];
		}

		const T& operator[](size_t index) const
		{
			assert(index < len);
			return elements[index];
		}

		iterator begin() 
		{
			return elements;
		}

		iterator end()
		{
			return elements + len;
		}
		
		const_iterator begin() const
		{
			return elements;
		}

		const_iterator end() const
		{
			return elements + len;
		}

		//Relational operators
		bool operator==(const Vector& other) const
		{
			VECTOR_COMPARE_IMPL(other, ==);
		}

		bool operator<(const Vector& other) const
		{
			VECTOR_COMPARE_IMPL(other, <);
		}

		bool operator<=(const Vector& other) const
		{
			VECTOR_COMPARE_IMPL(other, <=);
		}

		bool operator!=(const Vector& other) const
		{
			VECTOR_COMPARE_IMPL(other, !=);
		}

		bool operator>=(const Vector& other) const
		{
			VECTOR_COMPARE_IMPL(other, >=);
		}

		bool operator>(const Vector& other) const
		{
			VECTOR_COMPARE_IMPL(other, >);
		}

		//Math Operators.
		Vector& operator+=(const Vector& other)
		{
			VECTOR_MATH_IMPL(other, +=);
		}

		Vector& operator-=(const Vector& other)
		{
			VECTOR_MATH_IMPL(other, -=);
		}

		Vector& operator/=(const Vector& other)
		{
			VECTOR_MATH_IMPL(other, /=);
		}

		Vector& operator*=(const Vector& other)
		{
			VECTOR_MATH_IMPL(other, *=);
		}

		Vector& operator+=(T other)
		{
			VECTOR_MATH_SCALAR_IMPL(other, +=);
		}

		Vector& operator-=(T other)
		{
			VECTOR_MATH_SCALAR_IMPL(other, -=);
		}

		Vector& operator/=(T other)
		{
			VECTOR_MATH_SCALAR_IMPL(other, /=);
		}

		Vector& operator*=(T other)
		{
			VECTOR_MATH_SCALAR_IMPL(other, *=);
		}
	private:
		T elements[len];
	};

	template<typename T, size_t l>
	T dot( const Vector<T, l>& a, const Vector<T, l>& b)
	{
		return a.dot(b);
	}

	template<typename T, size_t l>
	Vector<T, l> cross( const Vector<T, l>& a, const Vector<T, l>& b)
	{
		return a.cross(b);
	}

	template<typename T, size_t l>
	Vector<T, l> unit( const Vector<T, l>& a)
	{
		return a.unit();
	}

	template<typename T, size_t l>
	Vector<T, l> project( const Vector<T, l>& a, const Vector<T, l>& b )
	{
		return (dot(a, b) / a.getMagnitude()) * a.unit();
	}

	template<typename T, size_t l>
	Vector<T, l> operator+(const Vector<T, l>& a, const Vector<T, l>& b)
	{
		Vector<T, l> result(a);
		result += b;
		return result;
	}

	template<typename T, size_t l>
	Vector<T, l> operator-(const Vector<T, l>& a, const Vector<T, l>& b)
	{
		Vector<T, l> result(a);
		result -= b;
		return result;
	}


	template<typename T, size_t l>
	Vector<T, l> operator*(const Vector<T, l>& a, const Vector<T, l>& b)
	{
		Vector<T, l> result(a);
		result *= b;
		return result;
	}

	template<typename T, size_t l>
	Vector<T, l> operator/(const Vector<T, l>& a, const Vector<T, l>& b)
	{
		Vector<T, l> result(a);
		result /= b;
		return result;
	}

	template<typename T, size_t l, typename U>
	Vector<T, l> operator+(const Vector<T, l>& a, const U& b)
	{
		Vector<T, l> result(a);
		result += static_cast<T>(b);
		return result;
	}

	template<typename T, size_t l, typename U>
	Vector<T, l> operator+(const U& b, const Vector<T, l>& a)
	{
		Vector<T, l> result(a);
		result += static_cast<T>(b);
		return result;
	}

	template<typename T, size_t l, typename U>
	Vector<T, l> operator-(const Vector<T, l>& a, const U& b)
	{
		Vector<T, l> result(a);
		result -= static_cast<T>(b);
		return result;
	}


	template<typename T, size_t l, typename U>
	Vector<T, l> operator*(const Vector<T, l>& a, const U& b)
	{
		Vector<T, l> result(a);
		result *= static_cast<T>(b);
		return result;
	}

	//Disable this from lookup when U = matrix.
	template<typename T, size_t l, typename U>
	typename boost::disable_if<
		boost::mpl::or_<
			boost::is_same<U, Matrix4>,
			boost::is_same<U, Matrix3>, 
			boost::is_same<U, Matrix2>
		>, 
		Vector<T, l>
	>::type operator*(U b, const Vector<T, l>& a)
	{
		Vector<T, l> result(a);
		result *= static_cast<T>(b);
		return result;
	}

	template<typename T, size_t l, typename U>
	Vector<T, l> operator/(const Vector<T, l>& a, U b)
	{
		Vector<T, l> result(a);
		result /= static_cast<T>(b);
		return result;
	}

	//Vector typecasting.
	template<typename T, typename U, size_t Len>
	Vector<T, Len> vector_cast( const Vector<U, Len>& in )
	{
		Vector<T, Len> result;
		std::copy( in.begin(), in.end(), result.begin() );
		return result;
	}

	template<size_t newLen, typename T, size_t Len>
	Vector<T, newLen> vector_cast( const Vector<T, Len>& in)
	{
		return vector_cast<T, newLen, T, Len>(in);
	}

#pragma warning(push)
#pragma warning(disable: 4996)  //Checked iterators
	template<typename T, size_t newLen, typename U, size_t Len>
	Vector<T, newLen> vector_cast( const Vector<U, Len>& in )
	{
		Vector<T, newLen> result;
		if (newLen < Len)
		{
			std::copy( in.begin(), in.begin() + newLen, result.begin());
		} else
		{
			std::copy( in.begin(), in.end(), result.begin() );
		}
		return result;
	}
#pragma warning(pop)

	//Type traits
	template<typename T>
	struct is_vector : boost::false_type
	{};

	template<typename T, size_t L>
	struct is_vector< Vector<T, L> > : boost::true_type
	{};
	
	template<typename T, size_t i>
	std::ostream& operator<<(std::ostream& o, const Math::Vector<T, i>& t)
	{
		o << '[';
		for ( typename Math::Vector<T, i>::const_iterator it = t.begin(); it != t.end(); ++it)
		{
			if (it != t.begin())
			{
				o << ',';
			}
			o << *it;
		}
		o << ']';
		return o;
	}

	template<typename T, size_t i>
	std::wostream& operator<<(std::wostream& o, const Math::Vector<T, i>& t)
	{
		o << L'[';
		for ( typename Math::Vector<T, i>::const_iterator it = t.begin(); it != t.end(); ++it)
		{
			if (it != t.begin())
			{
				o << L',';
			}
			o << *it;
		}
		o << L']';
		return o;
	}
}

#undef VECTOR_COMPARE_IMPL
#undef VECTOR_MATH_IMPL
#undef VECTOR_MATH_SCALAR_IMPL
#endif


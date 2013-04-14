#include <cassert>
#include <iostream>

#pragma once
namespace Math
{
	template<typename Storage>
	class Matrix4
	{
	public:
		Storage& operator()(size_t row, size_t col)
		{
			assert(row < 4 && col < 4);
			return elements[row * 4 + col];
		}

		const Storage& operator()(size_t row, size_t col) const
		{
			assert(row < 4 && col < 4);
			return elements[row * 4 + col];
		}
	private:
		Storage elements[16];
	};

	template<typename Storage>
	std::ostream& operator<<(std::ostream& o, const Matrix4<Storage>& s)
	{
		o << "["; 
		for (size_t i = 0; i < 4; ++i)
		{
			if (i)
			{
				o << "; ";
			}

			o << "[" << s(i, 0) << ", " << s(i, 1) << ", " << s(i, 2) << ", " << s(i, 3) << "]";
		}

		o << "]";
		return o;
	}
}
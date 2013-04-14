#include <iostream>

namespace Math
{
	template<typename Storage>
	struct Quaternion
	{
		Storage w;
		Storage x;
		Storage y;
		Storage z;
	};

	template<typename Storage>
	std::ostream& operator<<(std::ostream& o, const Quaternion<Storage>& s)
	{
		o << "q[" << s.w << ", " << s.x << ", " << s.y << ", " << s.z << "]";
		return o;
	}
}
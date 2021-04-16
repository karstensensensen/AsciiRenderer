#include "Vertices.h"

namespace Asciir
{
	template<typename T>
	arVertex<T>::arVertex(T x, T y)
	: x(x), y(y) {}

	template<typename T>
	arVertex<T>::arVertex(const arBigVertex<T>& derived)
		: x(derived.x), y(derived.y/2) {}

	template<typename T>
	arVertex<T>::arVertex(const arSmallVertex<T>& derived)
		: x(derived.x / 2), y(derived.y) {}

	template<typename T>
	arVertex<T> arVertex<T>::operator+(const arVertex<T>& other) const
	{
		return { x + other.x, y + other.y};
	}

	template<typename T>
	arVertex<T> arVertex<T>::operator-(const arVertex<T>& other) const
	{
		return { x - other.x, y - other.y };
	}

	template<typename T>
	arVertex<T> arVertex<T>::operator*(const arVertex<T>& other) const
	{
		return { x * other.x, y * other.y };
	}

	template<typename T>
	arVertex<T> arVertex<T>::operator/(const arVertex<T>& other) const
	{
		return { x / other.x, y / other.y };
	}

	template<typename T>
	arVertex<T> arVertex<T>::operator%(const arVertex<T>& other) const
	{
		return { x % other.x, y % other.y };
	}


	template<typename T>
	void arVertex<T>::operator+=(const arVertex<T>& other)
	{
		x += other.x;
		y += other.y;
	}

	template<typename T>
	void arVertex<T>::operator-=(const arVertex<T>& other)
	{
		x -= other.x;
		y -= other.y;
	}

	template<typename T>
	void arVertex<T>::operator*=(const arVertex<T>& other)
	{
		x *= other.x;
		y *= other.y;
	}

	template<typename T>
	void arVertex<T>::operator/=(const arVertex<T>& other)
	{
		x /= other.x;
		y /= other.y;
	}

	template<typename T>
	void arVertex<T>::operator%=(const arVertex<T>& other)
	{
		x %= other.x;
		y %= other.y;
	}


	template<typename T>
	template<typename TOther>
	arVertex<T>::arVertex(const arVertex<TOther>& other)
		:x(T(x)), y(T(y)) {}

	template<typename T>
	std::ostream& operator<<(std::ostream& stream, const arVertex<T>& vert)
	{
		stream << '(' << vert.x << ',' << vert.y << ')';
		return stream;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& stream, const arVertices<T>& verts)
	{
		for (size_t i = 0; i < verts.size() - 1; i++)
		{
			stream << verts[i] << ',';
		}

		stream << verts[verts.size() - 1];

		return stream;
	}

	template<typename T>
	arBigVertex<T>::arBigVertex(T x, T y)
		:arVertex<T>(x, y) {}

	template<typename T>
	arBigVertex<T>::arBigVertex(const arVertex<T>& base)
		: arVertex<T>(base) {}

	template<typename T>
	arSmallVertex<T>::arSmallVertex(T x, T y)
		: arVertex<T>(x, y) {}

	template<typename T>
	arSmallVertex<T>::arSmallVertex(const arVertex<T>& base)
		: arVertex<T>(base) {}

}

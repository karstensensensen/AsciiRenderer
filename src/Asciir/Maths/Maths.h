#pragma once

#include "Asciir/Core/Core.h"
#include "Vertices.h"

namespace Asciir
{
	const extern Real PI;

	constexpr Real degToRad(Real degrees)
	{
		return degrees * PI / (Real)180;
	}

	constexpr Real radToDeg(Real radians)
	{
		return radians * (Real)180 / PI;
	}

	// TODO: find a better method of doing this
	/// @brief compare floats with a margin of error
	bool compareMargin(Real a, Real b, Real margin = 0);

	/// @brief test if two Reals are euqal, taking into account the possible error of floating point values.
	/// 
	/// @note this function may be similar to compareMargin, however this is intended to compare two floats and check if they are practicly equal, whereas comapre margin is intended to check wether two floats are within a fixed margin.
	/// 
	/// 
	/// @param a first float to compare
	/// @param b second float to compare
	/// @param e the epsilon value for the comparison
	/// @return wether a and b are equal
	bool fequal(Real a, Real b, Real e = std::numeric_limits<Real>::epsilon());

	/// @brief round the given real to the nearest value divisible by base.  
	///	
	/// the base represents the interval of the nearest value
	/// 
	/// for example:
	/// > round 2.5, base 1:  
	/// > 3  
	/// >  
	/// > round 2.5, base 2:  
	/// > 2  
	/// >  
	/// > round 2.5 base 0.5:  
	/// > 2.5  
	/// 
	/// @param val the value to be rounded
	/// @param base the base to be rounded in
	Real round(Real val, Real base = 1);
	/// @brief rounds both x, and y value using the given base
	/// see round(Real, Real) for implementation details
	Coord round(const Coord& coord, Real base = 1);
	/// @brief rounds the given value up to the nearest value divisible by base
	/// see round(Real, Real) for implementation details
	Real ceil(Real val, Real base = 1);
	/// @brief rounds the given Coord up to the nearest value divisible by base
	/// see round(Real, Real) for implementation details
	Coord ceil(const Coord& coord, Real base = 1);
	/// @brief rounds the given value down to the nearest value divisible by base
	/// see round(Real, Real) for implementation details
	Real floor(Real val, Real base = 1);
	/// @brief rounds the given Coord down to the nearest value divisible by base
	/// see round(Real, Real) for implementation details
	Coord floor(const Coord& coord, Real base = 1);
	/// @brief rounds the value with a bias towards higher numbers, meaning 0.5 round to 1 instead of 0.  
	/// same as round()
	inline Real roundu(Real val, Real base = 1) { return round(val, base);}
	/// @brief rounds the value with a bias towards lower numbers, meaning 0.5 round to 0 instead of 1.  
	inline Real roundl(Real val, Real base = 1) { return round(val - base / 2, base); }
}

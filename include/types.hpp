//------------------------------------------------------------------------------
//
//	by RHL (robs-basement.de) 2007 - 2008
//
//	-> PLEASE READ THE README! <-
//
//------------------------------------------------------------------------------


#ifndef __TYPES_HPP
#define __TYPES_HPP


/**
	@file types.hpp
	@brief Some type, struct and class definitions and common constants.
*/


class CRGBA
{
public:
	uint8 r, g, b, a;

	CRGBA(void)
	{
		r = g = b = 255;
		a = 31;
	}

	CRGBA(const CRGBA &other)
	{
		this->r = other.r;
		this->g = other.g;
		this->b = other.b;
		this->a = other.a;
	}

	CRGBA(uint8 r, uint8 g, uint8 b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 31;
	}

	CRGBA(uint8 r, uint8 g, uint8 b, uint8 a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
};


// some common color constants
const CRGBA	c_black = CRGBA(0, 0, 0, 31),
			c_gray = CRGBA(128, 128, 128, 31),
			c_white = CRGBA(255, 255, 255, 31),
			c_red = CRGBA(255, 0, 0, 31),
			c_green = CRGBA(0, 255, 0, 31),
			c_blue = CRGBA(0, 0, 255, 31);


/**
	@class CView

	This class contains all data needed (at least in this demo) for a (first person) 3D view / "virtual camera".
*/

class CView
{
public:
	// position
	int32 x, y, z;
	// rotation etc.
	int32 angleX, angleY;
};


#endif

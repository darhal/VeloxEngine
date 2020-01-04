#pragma once

#include <Core/Misc/Defines/Common.hpp>

TRE_NS_START

/**
 * Source Blend			- 14 bits.
 * Destination Blend	- 14 bits.
 * Depth Testing		- 8 bits.
 * Windowing order		- 4 bits.
 * Cull face			- 4 bits.
 * Polygon Mode			- 4 bits.
 * Polygon Face			- 4 bits.
 * Shader				- 12 bits.
*/
typedef uint64 Key;

TRE_NS_END
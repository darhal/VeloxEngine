#pragma once

#include <Core/Misc/Defines/Common.hpp>

TRE_NS_START

/** Bucket Key
 --------------LSB--------------
 * Cull mode/face		: 2 bits.
 * Polygon Mode			: 2 bits.
 * Blend equation		: 3 bits	| 3x Stencil actions	: 3*3 bits.
 * Source Blend			: 10 bits.	| Stencil Function		: 3 bits.
 * Destination Blend	: 10 bits.	| Stencil Ref			: 8 bits. / Stencil mask : 3 bits.
 * Depth Testing		: 3 bits.
 * Cull Enable			: 1 bits.
 * Stencil Enable		: 1 bits.
 * Depth Enable			: 1 bits.
 * Shader				: 30 bits.
 * Blend Enable			: 1 bits.
 --------------MSB--------------
 ---	Leftover : 0 bits.	 ---
*/
typedef uint64 BucketKey;

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

TRE_NS_END
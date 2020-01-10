#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Common/Common.hpp>

TRE_NS_START

/** Bucket Key
 --------------LSB--------------
 * MaterialID		: 16 bits.
 * VaoID			: 16 bits.
 * Blend Distance	: 16 bits.
 --------------MSB--------------
 ---	Leftover : 16 bits.	 ---
*/

typedef uint64 ModelKey;

struct ModelRenderParams
{
	CONSTEXPR static uint8 SHIFT_BITS[] = { 16, 16, 16 };

	MaterialID material_id;
	VaoID vao_id;
	uint16 blend_dist;

	FORCEINLINE ModelKey ToKey();

	FORCEINLINE static ModelRenderParams FromKey(ModelKey key);
};

ModelKey ModelRenderParams::ToKey()
{
	ModelKey key = 0;
	key |= material_id;
	key |= vao_id << SHIFT_BITS[0];
	key |= blend_dist << SHIFT_BITS[1];

	return key;
}

ModelRenderParams ModelRenderParams::FromKey(ModelKey key)
{
	CONSTEXPR uint8 total_bits = SHIFT_BITS[0] + SHIFT_BITS[1] + SHIFT_BITS[2];

	ModelRenderParams params;
	params.material_id = key & 0xFFFF;
	params.vao_id = (key & 0xFFFF0000) >> SHIFT_BITS[0];
	params.blend_dist = uint16((key & 0xFFFF00000000) >> (SHIFT_BITS[0] + SHIFT_BITS[1]));

	return params;
}

TRE_NS_END
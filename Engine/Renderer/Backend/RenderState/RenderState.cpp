#include "RenderState.hpp"
#include <bitset>
#include <iostream>

TRE_NS_START

RenderState RenderState::FromKey(const BucketKey& key, uint32* shader_id)
{
	CONSTEXPR uint32 bits_for_key = 64;
	RenderState state;
	uint8 shift_bits = 0;

	// Enable/Disable
	state.blend_enabled = (bool)(key >> (bits_for_key - 1));

	if (shader_id)
		*shader_id = (key << (shift_bits += 1)) >> (bits_for_key - SHADER_BITS);

	state.depth_enabled = (key << (shift_bits += SHADER_BITS)) >> (bits_for_key - 1);
	state.stencil_enabled = (key << (shift_bits += 1)) >> (bits_for_key - 1);
	state.cull_enabled = (key << (shift_bits += 1)) >> (bits_for_key - 1);
	state.depth_func = TestFunction::Decode((key << (shift_bits += 1)) >> (bits_for_key - TestFunction::ENCODING_BITS));

	if (state.blend_enabled) {
		state.blending.blend_dstAlpha = Blending::DecodeFunc((key << (shift_bits += TestFunction::ENCODING_BITS)) >> (bits_for_key - Blending::ENCODING_BITS_FUNC));
		state.blending.blend_dstRGB = Blending::DecodeFunc((key << (shift_bits += Blending::ENCODING_BITS_FUNC)) >> (bits_for_key - Blending::ENCODING_BITS_FUNC));
		state.blending.blend_srcAlpha = Blending::DecodeFunc((key << (shift_bits += Blending::ENCODING_BITS_FUNC)) >> (bits_for_key - Blending::ENCODING_BITS_FUNC));
		state.blending.blend_srcRGB = Blending::DecodeFunc((key << (shift_bits += Blending::ENCODING_BITS_FUNC)) >> (bits_for_key - Blending::ENCODING_BITS_FUNC));
		state.blending.blend_equation = Blending::Decode((key << (shift_bits += Blending::ENCODING_BITS_FUNC)) >> (bits_for_key - Blending::ENCODING_BITS_EQ));
		shift_bits += Blending::ENCODING_BITS_EQ;
	} else if (state.stencil_enabled) {
		state.stencil.stencil_mask = Stencil::StencilMask::Decode((key << (shift_bits += TestFunction::ENCODING_BITS)) >> (bits_for_key - Stencil::StencilMask::ENCODING_BITS));
		state.stencil.stencil_func_ref = (uint8)((key << (shift_bits += Stencil::StencilMask::ENCODING_BITS)) >> (bits_for_key - BITS_PER_BYTE));
		state.stencil.stencil_func = TestFunction::Decode((key << (shift_bits += BITS_PER_BYTE)) >> (bits_for_key - TestFunction::ENCODING_BITS));
		state.stencil.stencil_dppass = Stencil::Decode((key << (shift_bits += TestFunction::ENCODING_BITS)) >> (bits_for_key - Stencil::ENCODING_BITS));
		state.stencil.stencil_dpfail = Stencil::Decode((key << (shift_bits += Stencil::ENCODING_BITS)) >> (bits_for_key - Stencil::ENCODING_BITS));
		state.stencil.stencil_sfail = Stencil::Decode((key << (shift_bits += Stencil::ENCODING_BITS)) >> (bits_for_key - Stencil::ENCODING_BITS));
		shift_bits += Stencil::ENCODING_BITS;
	} else {
		shift_bits += TestFunction::ENCODING_BITS + Blending::ENCODING_BITS_EQ + 4 * Blending::ENCODING_BITS_FUNC;
	}

	state.poly_mode = PolygonMode::Decode((key << shift_bits) >> (bits_for_key - PolygonMode::ENCODING_BITS));
	CullMode::Decode((key << (shift_bits += PolygonMode::ENCODING_BITS)) >> (bits_for_key - CullMode::ENCODING_BITS), state.frontface, state.cullmode);
	return state;
}

BucketKey RenderState::ToKey(uint32 shader_id) const
{
	BucketKey key = 0;
	uint32 shift_bits = 0;

	// Options
	key |= CullMode::Encode(frontface, cullmode);
	key |= (BucketKey)poly_mode << (shift_bits += CullMode::ENCODING_BITS);

	if (blend_enabled) {
		key |= (BucketKey)blending.blend_equation << (shift_bits += PolygonMode::ENCODING_BITS);
		key |= (BucketKey)blending.blend_srcRGB << (shift_bits += Blending::ENCODING_BITS_EQ);
		key |= (BucketKey)blending.blend_srcAlpha << (shift_bits += Blending::ENCODING_BITS_FUNC);
		key |= (BucketKey)blending.blend_dstRGB << (shift_bits += Blending::ENCODING_BITS_FUNC);
		key |= (BucketKey)blending.blend_dstAlpha << (shift_bits += Blending::ENCODING_BITS_FUNC);
		shift_bits += Blending::ENCODING_BITS_FUNC;
	} else if (stencil_enabled) {
		key |= (BucketKey)stencil.stencil_sfail << (shift_bits += PolygonMode::ENCODING_BITS);
		key |= (BucketKey)stencil.stencil_dpfail << (shift_bits += Stencil::ENCODING_BITS);
		key |= (BucketKey)stencil.stencil_dppass << (shift_bits += Stencil::ENCODING_BITS);
		key |= (BucketKey)stencil.stencil_func << (shift_bits += Stencil::ENCODING_BITS);
		key |= (BucketKey)stencil.stencil_func_ref << (shift_bits += TestFunction::ENCODING_BITS);
		key |= (BucketKey)stencil.stencil_mask << (shift_bits += BITS_PER_BYTE);
		shift_bits += Stencil::StencilMask::ENCODING_BITS;
	} else {
		shift_bits += PolygonMode::ENCODING_BITS + Blending::ENCODING_BITS_EQ + 4 * Blending::ENCODING_BITS_FUNC;
	}

	key |= (BucketKey)depth_func << shift_bits;
	// Enable/Disable
	key |= (BucketKey)cull_enabled << (shift_bits += TestFunction::ENCODING_BITS);
	key |= (BucketKey)stencil_enabled << (shift_bits += 1);
	key |= (BucketKey)depth_enabled << (shift_bits += 1);
	key |= (BucketKey)shader_id << (shift_bits += 1);
	key |= (BucketKey)blend_enabled << (shift_bits += SHADER_BITS);

	return key;
}

void RenderState::ApplyStates() const
{
	if (blend_enabled) {
		Enable(Capability::BLENDING);
		BlendFuncSeparate(blending.blend_srcRGB, blending.blend_dstRGB, blending.blend_srcAlpha, blending.blend_dstAlpha);
		BlendEquation(blending.blend_equation);
	} else {
		Disable(Capability::BLENDING);
	}

	/*if (depth_enabled) {
		Enable(Capability::DEPTH_TEST);
		glDepthFunc(depth_func);
	} else {
		Disable(Capability::DEPTH_TEST);
	}

	if (cull_enabled) {
		Enable(Capability::CULL_FACE);
		Call_GL(glFrontFace(frontface));
		Call_GL(glCullFace(cullmode));
	} else {
		Disable(Capability::CULL_FACE);
	}

	if (blend_enabled) {
		Enable(Capability::BLENDING);
		glBlendFuncSeparate(blend_srcRGB, blend_dstRGB, blend_srcAlpha, blend_dstAlpha);
		glBlendEquation(blend_equation);
	} else {
		Disable(Capability::BLENDING);
	}

	if (stencil_enabled) {
		Enable(Capability::STENCIL_TEST);
		StencilOp(stencil_sfail, stencil_dpfail, stencil_dppass);
		StencilFunc(stencil_func, stencil_func_ref, (uint32)stencil_mask);
		StencilMask((uint32)stencil_mask);
	} else {
		Disable(Capability::STENCIL_TEST);
	}

	Call_GL(glPolygonMode(GL_FRONT_AND_BACK, poly_mode));*/
}

TRE_NS_END
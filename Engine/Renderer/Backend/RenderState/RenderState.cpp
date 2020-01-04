#include "RenderState.hpp"

TRE_NS_START

RenderState RenderState::FromKey(const Key& key)
{
	return RenderState();
}

Key RenderState::ToKey() const
{
	Key key = 0;
	uint8 shift_bits = 0;

	// Options
	key |= PolygonMode::Encode(poly_mode);
	key |= CullMode::Encode(frontface, cullmode) << (shift_bits += PolygonMode::ENCODING_BITS);
	key |= TestFunction::Encode(depth_func) << (shift_bits += CullMode::ENCODING_BITS);
	key |= Blending::Encode(blend_equation) << (shift_bits += TestFunction::ENCODING_BITS);
	key |= Blending::Encode(blend_srcRGB) << (shift_bits += Blending::ENCODING_BITS_EQ);
	key |= Blending::Encode(blend_dstRGB) << (shift_bits += Blending::ENCODING_BITS_FUNC);
	key |= Blending::Encode(blend_srcAlpha) << (shift_bits += Blending::ENCODING_BITS_FUNC);
	key |= Blending::Encode(blend_dstAlpha) << (shift_bits += Blending::ENCODING_BITS_FUNC);

	// Enable/Disable
	key |= cull_enabled << (shift_bits += Blending::ENCODING_BITS_FUNC);
	key |= depth_enabled << (shift_bits += 1);
	key |= blend_enabled << (shift_bits += 1);
	return key;
}

void RenderState::ApplyStates() const
{
	if (depth_enabled) {
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
		StencilFunc(stencil_func, stencil_func_ref, stencil_mask);
		StencilMask(stencil_mask);
	} else {
		Disable(Capability::STENCIL_TEST);
	}

	Call_GL(glPolygonMode(GL_FRONT_AND_BACK, poly_mode));
}

TRE_NS_END
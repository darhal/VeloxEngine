#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Common/Common.hpp>
#include <RenderAPI/General/GLContext.hpp>
#include <Renderer/Backend/Keys/BucketKey.hpp>

TRE_NS_START

struct RenderState
{
	CONSTEXPR static uint32 SHADER_BITS = 30;

	bool cull_enabled = true;
	CullMode::front_face_t frontface = CullMode::front_face_t::CW;
	CullMode::cull_mode_t cullmode = CullMode::cull_mode_t::FRONT;

	bool depth_enabled = true;
	TestFunction::test_function_t depth_func = TestFunction::LESS;

	bool blend_enabled = false;
	bool stencil_enabled = false;
	struct BlendingOptions
	{
		Blending::blend_equation_t blend_equation = Blending::blend_equation_t::NONE;
		Blending::blend_func_t blend_srcRGB = Blending::blend_func_t::SRC_ALPHA;
		Blending::blend_func_t blend_srcAlpha = Blending::blend_func_t::ONE;
		Blending::blend_func_t blend_dstRGB = Blending::blend_func_t::ONE_MINUS_SRC_ALPHA;
		Blending::blend_func_t blend_dstAlpha = Blending::blend_func_t::ZERO;

		BlendingOptions() = default;
	};

	struct StencilOptions
	{
		Stencil::stencil_action_t stencil_sfail = Stencil::stencil_action_t::KEEP;
		Stencil::stencil_action_t stencil_dpfail = Stencil::stencil_action_t::KEEP;
		Stencil::stencil_action_t stencil_dppass = Stencil::stencil_action_t::REPLACE;
		TestFunction::test_function_t stencil_func = TestFunction::test_function_t::ALWAYS;
		uint8 stencil_func_ref = 1;
		Stencil::StencilMask::stencil_mask_t stencil_mask = Stencil::StencilMask::MASK1;

		StencilOptions() = default;
	};
	union
	{
		BlendingOptions blending = BlendingOptions();
		StencilOptions stencil /*= StencilOptions()*/;
	};

    PolygonMode::polygon_mode_t poly_mode = PolygonMode::FILL;

	static RenderState FromKey(const BucketKey& key, uint32* shader_id = NULL);

	BucketKey ToKey(uint32 shader_id = 0) const;

	void ApplyStates() const;

	RenderState() {};

	inline void Print();
};

FORCEINLINE bool operator==(const RenderState& a, const RenderState& b)
{
    return a.ToKey() == b.ToKey(); // TODO:
}

FORCEINLINE bool operator!=(const RenderState& a, const RenderState& b)
{
    return !(a == b);
}

void RenderState::Print()
{
	printf(
		"* Polygon Mode : %d\n"
		"* cull_enabled = %s\n"
		"\t frontface = %d\n"
		"\t cullmode = %d\n"
		"* depth_enabled = %s\n"
		"\t depth_func = %d\n",
		poly_mode,
		cull_enabled ? "True" : "False", frontface, cullmode,
		depth_enabled ? "True" : "False", depth_func
	);

	if (blend_enabled) {
		printf(
			"* Blending : Enabled\n"
			"\t blend_equation = %d\n"
			"\t blend_srcRGB = %d\n"
			"\t blend_srcAlpha = %d\n"
			"\t blend_dstRGB = %d\n"
			"\t blend_dstAlpha = %d\n",
			blending.blend_equation,
			blending.blend_srcRGB, blending.blend_srcAlpha,
			blending.blend_dstRGB, blending.blend_dstAlpha
		);
	} else if (stencil_enabled) {
		printf(
			"* Stencil : Enabled\n"
			"\t stencil_sfail = %d\n"
			"\t stencil_dpfail = %d\n"
			"\t stencil_dppass = %d\n"
			"\t stencil_func = %d\n"
			"\t stencil_func_ref = %d\n"
			"\t stencil_mask = %d\n",
			stencil.stencil_sfail,
			stencil.stencil_dpfail, stencil.stencil_dppass,
			stencil.stencil_func, stencil.stencil_func_ref, stencil.stencil_mask
		);
	} else {
		printf("Blending & Stencil: Disabled\n");
	}
}

TRE_NS_END
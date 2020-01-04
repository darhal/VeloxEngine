#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Common/Common.hpp>
#include <RenderAPI/General/GLContext.hpp>
#include <Renderer/Backend/Commands/Key/Key.hpp>

TRE_NS_START

struct RenderState
{
	bool cull_enabled = true;
	CullMode::front_face_t frontface = CullMode::front_face_t::CW;
	CullMode::cull_mode_t cullmode = CullMode::cull_mode_t::FRONT;

    bool depth_enabled = true;
    TestFunction::test_function_t depth_func = TestFunction::LESS;

    bool blend_enabled = false;
    Blending::blend_equation_t blend_equation = Blending::blend_equation_t::ADD;
    Blending::blend_func_t blend_srcRGB = Blending::blend_func_t::SRC_ALPHA;
    Blending::blend_func_t blend_dstRGB = Blending::blend_func_t::ONE_MINUS_SRC_ALPHA;
    Blending::blend_func_t blend_srcAlpha = Blending::blend_func_t::ONE;
    Blending::blend_func_t blend_dstAlpha = Blending::blend_func_t::ZERO;

    bool stencil_enabled = false;
    Stencil::stencil_action_t stencil_sfail = Stencil::stencil_action_t::KEEP;
    Stencil::stencil_action_t stencil_dpfail = Stencil::stencil_action_t::KEEP;
    Stencil::stencil_action_t stencil_dppass = Stencil::stencil_action_t::REPLACE;
    TestFunction::test_function_t stencil_func = TestFunction::test_function_t::ALWAYS;
    int32 stencil_func_ref = 1;
    uint32 stencil_func_mask = 0xFF;
    uint32 stencil_mask = 0x00;

    PolygonMode::polygon_mode_t poly_mode = PolygonMode::FILL;

	static RenderState FromKey(const Key& key);

	Key ToKey() const;

	void ApplyStates() const;
};

FORCEINLINE bool operator==(const RenderState& a, const RenderState& b)
{
    return a.ToKey() == b.ToKey(); // TODO:
}

FORCEINLINE bool operator!=(const RenderState& a, const RenderState& b)
{
    return !(a == b);
}

TRE_NS_END
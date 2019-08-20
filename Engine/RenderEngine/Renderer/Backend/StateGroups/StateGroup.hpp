#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <RenderEngine/Renderer/Common/Common.hpp>
#include <RenderAPI/General/GLContext.hpp>

TRE_NS_START

struct StateGroup
{
    bool depth_enabled = true;
    TestFunction::test_function_t depth_func = TestFunction::LESS;

    bool cull_enabled = true;
    CullMode::front_face_t frontface = CullMode::front_face_t::CW;
    CullMode::cull_mode_t cullmode = CullMode::cull_mode_t::FRONT;

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

    uint8 GetSortKey() const
    {
        return int8(blend_enabled) << 2 | int8(depth_enabled) << 1 | int8(cull_enabled);
    }
    
    // Output 7 bits code.
    StateHash GetHash() const 
    { 
        return 
        (GetSortKey() << (CullMode::GetEncodingBits() + TestFunction::GetEncodingBits() + PolygonMode::GetEncodingBits())) | 
        TestFunction::Encode(depth_func) << (CullMode::GetEncodingBits() + PolygonMode::GetEncodingBits()) | 
        CullMode::Encode(frontface, cullmode) << PolygonMode::GetEncodingBits() |
        PolygonMode::Encode(poly_mode); 
    };

    void ApplyStates() const
    {
        if (depth_enabled){
            Enable(Capability::DEPTH_TEST);
            glDepthFunc(depth_func);
        }else{
            Disable(Capability::DEPTH_TEST);
        }

        if (cull_enabled){
            Enable(Capability::CULL_FACE);
            Call_GL(glFrontFace(frontface));
	        Call_GL(glCullFace(cullmode));
        }else{
            Disable(Capability::CULL_FACE);
        }

        if (blend_enabled){
            Enable(Capability::BLENDING);
            glBlendFuncSeparate(blend_srcRGB, blend_dstRGB, blend_srcAlpha, blend_dstAlpha);
            glBlendEquation(blend_equation);
        }else{
            Disable(Capability::BLENDING);
        }

        if(stencil_enabled){
            Enable(Capability::STENCIL_TEST);
            StencilOp(stencil_sfail, stencil_dpfail, stencil_dppass);
            StencilFunc(stencil_func, stencil_func_ref, stencil_mask);
            StencilMask(stencil_mask); 
        }else{
            Disable(Capability::STENCIL_TEST);
        }

        Call_GL(glPolygonMode(GL_FRONT_AND_BACK, poly_mode));
    }
};

FORCEINLINE bool operator==(const StateGroup& a, const StateGroup& b)
{
    return a.GetHash() == b.GetHash();
}

FORCEINLINE bool operator!=(const StateGroup& a, const StateGroup& b)
{
    return !(a == b);
}

TRE_NS_END
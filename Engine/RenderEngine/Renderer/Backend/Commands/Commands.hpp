#pragma once

#include "Core/Misc/Defines/Common.hpp"

#include "RenderAPI/General/GLContext.hpp"
#include "RenderAPI/Texture/Texture.hpp"
#include "RenderAPI/FrameBuffer/FBO.hpp"
#include "RenderAPI/RenderBuffer/RBO.hpp"

#include "RenderEngine/Renderer/Common/Common.hpp"
#include "RenderEngine/Variables/Variables/Variable.hpp"
#include "RenderEngine/Variables/VariablesSet/VariablesSet.hpp"
#include "RenderEngine/Renderer/Backend/Dispatcher/DrawCallsDispatcher.hpp"

TRE_NS_START

typedef void (*BackendDispatchFunction)(const void*);

class ShaderProgram;
class VAO;
class Material;

struct Commands
{
    /*************************************** DRAW COMMANDS ***************************************/
    struct BasicDrawCommand
    {
        // CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION;
        Mat4f* model;
    };

    struct DrawCmd : public BasicDrawCommand
    {
        // some data ...
        Primitive::primitive_t mode;
        int32 start; 
        int32 end;

        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::Draw;
    };

    struct DrawIndexedCmd : public BasicDrawCommand
    {
        // some data ...
        Primitive::primitive_t mode;
        DataType::data_type_t type; 
        int32 count; 
        intptr offset;

        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::DrawIndexed;
    };

    /*************************************** RESOURCES COMMANDS ***************************************/

    struct GenerateIndexCmd
    {
        Variable<uint32> m_IndexVariable;
        VAO* m_VAO;

        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::GenerateIndex;
    };

    struct GenerateVAOCmd
    {
        VariableSet<> m_VariablesSet;
        VAO* m_VAO;

        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::GenerateVAO;
    };

    struct GenerateVAOFromVertexDataCmd
    {
        Variable<struct VertexData, VertexDataDesc> variable;
        VAO* m_VAO;
        
        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::GenerateVAOFromVertexData;
    };

    struct CreateTexture
    {
        TextureSettings settings;
        Texture* texture;
        
        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateTexture;
    };

	struct CreateFrameBuffer
	{
		FramebufferSettings settings;
		Framebuffer* fbo;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateFrameBuffer;
	};

	struct CreateRenderBuffer
	{
		RenderbufferSettings settings;
		Renderbuffer* rbo;
		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateRenderBuffer;
	};
};

TRE_NS_END
#pragma once

#include "Core/Misc/Defines/Common.hpp"

#include "RenderAPI/General/GLContext.hpp"
#include "RenderAPI/Texture/Texture.hpp"
#include "RenderAPI/FrameBuffer/FBO.hpp"
#include "RenderAPI/RenderBuffer/RBO.hpp"
#include "RenderAPI/VertexArray/VAO.hpp"

#include "RenderEngine/Renderer/Common/Common.hpp"
#include "RenderEngine/Variables/Variables/Variable.hpp"
#include "RenderEngine/Variables/VariablesSet/VariablesSet.hpp"
#include "RenderEngine/Renderer/Backend/Dispatcher/DrawCallsDispatcher.hpp"

TRE_NS_START

typedef void (*BackendDispatchFunction)(const void*);

class ShaderProgram;
class Material;

namespace Commands
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

    struct CreateIndexBuffer
    {
		VertexSettings::VertexBufferData settings;
        VAO* vao;

        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateIndexBuffer;
    };

	struct CreateVAO
	{
		VertexSettings settings;
		VAO* vao;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateVAO;
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
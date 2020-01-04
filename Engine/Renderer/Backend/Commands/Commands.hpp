#pragma once

#include "Core/Misc/Defines/Common.hpp"

#include "RenderAPI/General/GLContext.hpp"
#include "RenderAPI/Texture/Texture.hpp"
#include "RenderAPI/FrameBuffer/FBO.hpp"
#include "RenderAPI/RenderBuffer/RBO.hpp"
#include "RenderAPI/VertexArray/VAO.hpp"

#include "Renderer/Common/Common.hpp"
#include "Renderer/Backend/Dispatcher/DrawCallsDispatcher.hpp"

TRE_NS_START

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
		typedef VAO            Resource;
		typedef VertexSettings Settings;
		typedef typename RMI<Resource>::ID ID;

		VertexSettings settings;
		union { VAO* vao; VAO* resource; };

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateVAO;
	};

    struct CreateTexture
    {
		typedef Texture         Resource;
		typedef TextureSettings Settings;
		typedef typename RMI<Resource>::ID ID;

        TextureSettings settings;
		union { Texture* texture; Texture* resource; };
        
        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateTexture;
    };

	struct CreateFrameBuffer
	{
		typedef Framebuffer         Resource;
		typedef FramebufferSettings Settings;
		typedef typename RMI<Resource>::ID ID;

		FramebufferSettings settings;
		union { Framebuffer* fbo; Framebuffer* resource; };

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateFrameBuffer;
	};

	struct CreateRenderBuffer
	{
		typedef Renderbuffer         Resource;
		typedef RenderbufferSettings Settings;
		typedef typename RMI<Resource>::ID ID;

		RenderbufferSettings settings;
		union { Renderbuffer* rbo; Renderbuffer* resource; };

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateRenderBuffer;
	};
};

TRE_NS_END
#pragma once

#include "Core/Misc/Defines/Common.hpp"

#include "RenderAPI/General/GLContext.hpp"
#include "RenderAPI/Texture/Texture.hpp"
#include "RenderAPI/FrameBuffer/FBO.hpp"
#include "RenderAPI/RenderBuffer/RBO.hpp"
#include "RenderAPI/VertexArray/VAO.hpp"

#include "Renderer/Common/Common.hpp"
#include "Renderer/Backend/Dispatcher/DrawCallsDispatcher.hpp"
#include <Renderer/Materials/MaterialParametres/MaterialParametres.hpp>

TRE_NS_START

namespace Commands
{
	struct LinkCmd
	{
		void* next_cmd;
	};

    /*************************************** DRAW COMMANDS ***************************************/
    struct PreDrawCallCmd : public LinkCmd
    {
        Mat4f model;
		VaoID vao_id;
		const ShaderProgram* shader;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::PreDrawCall;
    };

    struct DrawCmd : public LinkCmd
    {
		// Extra data
		PreDrawCallCmd* prepare_cmd;
		const Material* material;

		// Draw data
        Primitive::primitive_t mode;
        int32 start; 
        int32 end;

        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::Draw;
    };

    struct DrawIndexedCmd : public LinkCmd
    {
		// Extra data
		PreDrawCallCmd* prepare_cmd;
		const Material* material;

		// Draw data
        Primitive::primitive_t mode;
        DataType::data_type_t type; 
        int32 count; 
        intptr offset;

        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::DrawIndexed;
    };

	/*************************************** INSTANCED DRAW COMMANDS ***************************************/
	struct PreInstancedDrawCallCmd : public LinkCmd
	{
		VaoID vao_id;
		const ShaderProgram* shader;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::InstancedPreDrawCall;
	};

	struct InstancedDrawCmd : public LinkCmd
	{
		// Extra data
		PreInstancedDrawCallCmd* prepare_cmd;
		const Material* material;

		// Draw data
		Primitive::primitive_t mode;
		int32 start;
		int32 end;
		uint32 instance_count;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::InstancedDraw;
	};

	struct InstancedDrawIndexedCmd : public LinkCmd
	{
		// Extra data
		PreInstancedDrawCallCmd* prepare_cmd;
		const Material* material;

		// Draw data
		Primitive::primitive_t mode;
		DataType::data_type_t type;
		int32 count;
		intptr offset;
		uint32 instance_count;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::InstancedDrawIndexed;
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

	struct CreateVBO
	{
		typedef VBO                  Resource;
		typedef VertexBufferSettings Settings;
		typedef typename RMI<Resource>::ID ID;

		VertexBufferSettings settings;
		union { VBO* vbo; VBO* resource; };

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateVBO;
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

	/*************************************** MISC COMMANDS ***************************************/

	struct EditSubBuffer
	{
		VBO* vbo;
		const void* data;
		GLintptr offset;
		uint32 size;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::EditSubBuffer;
	};

	struct DispatchCompute : public LinkCmd
	{
		ShaderProgram* shader;
		uint32 workGroupX, workGroupY, workGroupZ;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::DispatchCompute;
	};

	struct UploadUniforms
	{
		ShaderProgram* shader;
		UniformsParams<int32> m_Params;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::UploadUniforms;
	};
};

TRE_NS_END
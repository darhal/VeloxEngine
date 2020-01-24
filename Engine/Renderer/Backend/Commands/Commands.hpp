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
		const ShaderProgram* shader;
		Mat4f model;
		VaoID vao_id;

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
    struct CreateIndexBufferCmd
    {
		VertexSettings::VertexBufferData settings;
        VAO* vao;

        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateIndexBuffer;
    };

	struct CreateVAOCmd
	{
		typedef VAO            Resource;
		typedef VertexSettings Settings;

		VertexSettings settings;
		union { VAO* vao; VAO* resource; };

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateVAO;
	};

	struct CreateVBOCmd
	{
		typedef VBO                  Resource;
		typedef VertexBufferSettings Settings;

		VertexBufferSettings settings;
		union { VBO* vbo; VBO* resource; };

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateVBO;
	};

    struct CreateTextureCmd
    {
		typedef Texture         Resource;
		typedef TextureSettings Settings;

        TextureSettings settings;
		union { Texture* texture; Texture* resource; };
        
        CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateTexture;
    };

	struct CreateFrameBufferCmd
	{
		typedef Framebuffer         Resource;
		typedef FramebufferSettings Settings;

		FramebufferSettings settings;
		union { Framebuffer* fbo; Framebuffer* resource; };

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateFrameBuffer;
	};

	struct CreateRenderBufferCmd
	{
		typedef Renderbuffer         Resource;
		typedef RenderbufferSettings Settings;

		RenderbufferSettings settings;
		union { Renderbuffer* rbo; Renderbuffer* resource; };

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CreateRenderBuffer;
	};

	/*************************************** MISC COMMANDS ***************************************/

	struct EditSubBufferCmd
	{
		VBO* vbo;
		const void* data;
		GLintptr offset;
		uint32 size;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::EditSubBuffer;
	};

	struct DispatchComputeCmd : public LinkCmd
	{
		uint32 workGroupX, workGroupY, workGroupZ;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::DispatchCompute;
	};

	struct UploadUniformsCmd
	{
		ShaderProgram* shader;
		UniformsParams<int32> m_Params;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::UploadUniforms;
	};

	struct BindBufferBaseCmd
	{
		VBO* vbo;
		uint32 binding_point;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::BindBufferBase;
	};

	struct BindBufferRangeCmd
	{
		VBO* vbo;
		uint32 binding_point;
		uint32 offset;
		uint32 size;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::BindBufferRange;
	};

	struct MapBufferCmd
	{
		typedef void(*CallbackFunction)(const void*, const MapBufferCmd*);

		CallbackFunction callback;
		BufferTarget::buffer_target_t target;
		AccessTypes::access_types_t access;
		
		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::MapBufferCmd;
	};

	struct CallFunctionCmd
	{
		typedef void(*CallbackFunction)(const void*);

		CallbackFunction callback;
		void* data;

		CONSTEXPR static BackendDispatchFunction DISPATCH_FUNCTION = &BackendDispatch::CallFunctionCmd;
	};
};

TRE_NS_END
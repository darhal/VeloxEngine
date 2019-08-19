#pragma once

#include <Core/Context/GLDefines.hpp>
#include <Core/Context/Extensions.hpp>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Misc/Utils/Logging.hpp>

#if not defined(_DEBUG) || defined(NDEBUG)

	#define Call_GL(GL_CALL_FUNCTION) \
		GL_CALL_FUNCTION \

#else

	#include <Core/DataStructure/String/String.hpp>

	#define TO_STR(x) #x

	#define Call_GL(GL_CALL_FUNCTION) \
		GL_CALL_FUNCTION; \
		{ \
			GLenum err; \
   	 		while ((err = glGetError()) != GL_NO_ERROR) { \
				String error; \
                switch(err) { \
                    case GL_INVALID_OPERATION:      error = String("INVALID_OPERATION");      break; \
                    case GL_INVALID_ENUM:           error = String("INVALID_ENUM");           break; \
                    case GL_INVALID_VALUE:          error = String("INVALID_VALUE");          break; \
                    case GL_OUT_OF_MEMORY:          error = String("OUT_OF_MEMORY");          break; \
                    case GL_INVALID_FRAMEBUFFER_OPERATION:  error = String("INVALID_FRAMEBUFFER_OPERATION");  break; \
                } \
        		Log(LogType::ERR, "(OPEN GL ERROR) {%s} Occured at (%s:%d) after attempting to call %s\n", error.Buffer(), __FILENAME__, __LINE__, TO_STR(GL_CALL_FUNCTION)); \
    		} \
		} \

#endif

namespace DataType
{
	enum data_type_t
	{
		BYTE = GL_BYTE,
		UBYTE = GL_UNSIGNED_BYTE,
		SHORT = GL_SHORT,
		USHORT = GL_UNSIGNED_SHORT,
		INT = GL_INT,
		UINT = GL_UNSIGNED_INT,
		FLOAT = GL_FLOAT,
		DOUBLE = GL_DOUBLE,

		UBYTE332 = GL_UNSIGNED_BYTE_3_3_2,
		UBYTE233REV = GL_UNSIGNED_BYTE_2_3_3_REV,
		USHORT565 = GL_UNSIGNED_SHORT_5_6_5,
		USHORT565REV = GL_UNSIGNED_SHORT_5_6_5,
		USHORT4444 = GL_UNSIGNED_SHORT_4_4_4_4,
		USHORT4444REV = GL_UNSIGNED_SHORT_4_4_4_4_REV,
		USHORT5551 = GL_UNSIGNED_SHORT_5_5_5_1,
		USHORT1555REV = GL_UNSIGNED_SHORT_1_5_5_5_REV,
		UINT8888 = GL_UNSIGNED_INT_8_8_8_8,
		UINT8888REV = GL_UNSIGNED_INT_8_8_8_8_REV,
		UINT101010102 = GL_UNSIGNED_INT_10_10_10_2
	};
}

namespace TargetType
{
	enum target_type_t {
		NONE = 0,

		// VAO:
		VAO = 1,

		// Shader:
		SHADER = 2,

		// Buffers:
		ARRAY_BUFFER = GL_ARRAY_BUFFER,
		ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER,
		COPY_READ_BUFFER = GL_COPY_READ_BUFFER,
		COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER,
		DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER,
		DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER,
		ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
		PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER,
		PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER,
		QUERY_BUFFER = GL_QUERY_BUFFER,
		SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER,
		TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
		TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,
		UNIFORM_BUFFER = GL_UNIFORM_BUFFER,

		// Textures:
		TEX2D = GL_TEXTURE_2D,
		TEX1D = GL_TEXTURE_1D,
		TEX3D = GL_TEXTURE_3D,
		TEX1D_ARRAY = GL_TEXTURE_1D_ARRAY,
		TEX2D_ARRAY = GL_TEXTURE_2D_ARRAY,
		TEX_RECTANGLE = GL_TEXTURE_RECTANGLE,
		TEX_CUBE = GL_TEXTURE_CUBE_MAP,
		TEX_CUBE_MAP = GL_TEXTURE_CUBE_MAP_ARRAY,
		TEX_BUFFER = GL_TEXTURE_BUFFER,
		TEX2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,
		TEX2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,

		// Framebuffers:
		FBO_DRAW = GL_DRAW_FRAMEBUFFER,
		FBO_READ = GL_READ_FRAMEBUFFER,
		FBO = GL_FRAMEBUFFER,

		// Renderbuffers:
		RBO = GL_RENDERBUFFER,

	};
}

TRE_NS_START

class VAO;
class VBO;
class FBO;
class RBO;
class Texture;
class Shader;
class ShaderProgram;

TRE_NS_END
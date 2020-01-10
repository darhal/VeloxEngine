#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <time.h>
#include <unordered_map>
#include <bitset>

#include <Core/Context/Extensions.hpp>
#include <Core/Window/Window.hpp>
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Utils/Image.hpp>
#include <Core/Context/Context.hpp>
#include <Core/FileSystem/Directory/Directory.hpp>
#include <Core/DataStructure/PackedArray/RefPackedArray.hpp>
#include <Core/DataStructure/HashMap/HashMap.hpp>
#include <Core/TaskSystem/TaskManager/TaskManager.hpp>
#include <Core/DataStructure/PackedArray/PackedArray.hpp>

#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <RenderAPI/Shader/Shader.hpp>
#include <RenderAPI/VertexArray/VAO.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>
#include <RenderAPI/Texture/Texture.hpp>
#include <RenderAPI/General/GLContext.hpp>
#include <RenderAPI/RenderBuffer/RBO.hpp>
#include <RenderAPI/FrameBuffer/FBO.hpp>
#include <RenderAPI/GlobalState/GLState.hpp>


#include <Core/DataStructure/Tuple/Tuple.hpp>
#include "ShaderValidator.hpp"
#include <Core/DataStructure/StringBuffer/StringBuffer.hpp>
#include <Core/DataStructure/FixedString/FixedString.hpp>

#include <Core/ECS/ECS/ECS.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/DataStructure/Utils/Utils.hpp>

#include <Renderer/Backend/ICommandBuffer/ICommandBuffer.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>
#include <Renderer/Backend/RenderState/RenderState.hpp>
#include <Core/Misc/Singleton/Singleton.hpp>

using namespace TRE;


#define INIT_BENCHMARK std::chrono::time_point<std::chrono::high_resolution_clock> start, end; std::chrono::microseconds duration;

#define BENCHMARK(name, bloc_of_code) \
	start = std::chrono::high_resolution_clock::now(); \
	bloc_of_code; \
	end = std::chrono::high_resolution_clock::now();\
	duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); \
	std::cout << "\nExecution of '" << name << "' took : " << duration.count() << " microsecond(s)" << std::endl; \

int main()
{
	// TODO: SMALL BUGS ON THE MAPS (last iterator is not went throught)
	// settings
	const unsigned int SCR_WIDTH = 1900 / 2;
	const unsigned int SCR_HEIGHT = 1000 / 2;

	/*TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (OpenGL 3.3)", WindowStyle::Resize);
	window.initContext(3, 3);

	printf("- GPU Vendor    	: %s\n", glGetString(GL_VENDOR));
	printf("- Graphics      	: %s\n", glGetString(GL_RENDERER));
	printf("- Version       	: %s\n", glGetString(GL_VERSION));
	printf("- GLSL Version  	: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("- Hardware Threads 	: %d\n", std::thread::hardware_concurrency());*/

	const uint32 bucket_count = 10;
	const uint32 cmds_count = 5'000;
	ICommandBuffer cmd_buffer = ICommandBuffer();
	CommandBucket& bucket = cmd_buffer.CreateBucket();
	INIT_BENCHMARK;
	
	for (uint32 i = 0; i < bucket_count; i++) {
		for (uint32 j = 0; j < cmds_count; j++) {
			Commands::DrawCmd* cmd = bucket.SubmitCommand<Commands::DrawCmd>(i, std::rand() % 30);
			cmd->start = i;
			cmd->end = i + 1;
			cmd->mode = Primitive::TRIANGLES;
		}
	}

	bucket.End();

	// INIT_BENCHMARK;
	//while (true) {
		BENCHMARK("Dispatching All Commands", cmd_buffer.DispatchCommands());
	//}
	

	/*Event ev;
	while (true) {
		if (window.getEvent(ev)) {
		}

		glClearColor(0.59, 0.59, 0.59, 1.0);
		Clear(Buffer::COLOR);
		

		window.Present();
	}*/


	getchar();
	return 0;
}
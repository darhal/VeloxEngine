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

using namespace TRE;

int main()
{
	/*ICommandBuffer cmd_buffer = ICommandBuffer();
	CommandBucket& bucket = cmd_buffer.CreateBucket();
	Commands::DrawCmd* cmds[10];

	for (uint32 i = 0; i < 10; i++) {
		for (uint32 j = 0; j < 10; j++) {
			cmds[i] = bucket.SubmitCommand<Commands::DrawCmd>(i, std::rand() % 30);
			cmds[i]->start = i;
			cmds[i]->end = i + 1;
			cmds[i]->mode = Primitive::TRIANGLES;
		}
	}

	bucket.End();
	cmd_buffer.DispatchCommands();*/
	/* 
	*/
	RenderState r;
	r.stencil_enabled = true;
	r.stencil = RenderState::StencilOptions();
	r.Print();
	BucketKey k = r.ToKey(17);
	printf("KEY : %lld\n", k); std::cout << "Sig : " << std::bitset<64>(k) << std::endl;
	printf("----------------- build from key -----------------\n");
	RenderState r2 = RenderState::FromKey(k);
	r2.Print();

	getchar();
	return 0;
}
// #ifdef bougabouga

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

#include <Renderer/Backend/CommandBuffer/CommandBuffer.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>
#include <Renderer/Backend/RenderState/RenderState.hpp>
#include <Core/Misc/Singleton/Singleton.hpp>
#include <Renderer/MeshLoader/Model/Model.hpp>
#include <Renderer/MeshLoader/MeshLoader.hpp>
#include <Renderer/RenderPipline/Forward/Forward.hpp>
#include <Renderer/Components/LightComponents/DirectionalLightComponent/DirectionalLight.hpp>
#include <Renderer/Components/LightComponents/PointLightComponent/PointLight.hpp>
#include <Renderer/Components/LightComponents/SpotLightComponent/SpotLight.hpp>
#include "ShaderValidator.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManagerHelper.hpp>
#include <Renderer/ShaderParser/ShaderParser.hpp>

using namespace TRE;

#define INIT_BENCHMARK std::chrono::time_point<std::chrono::high_resolution_clock> start, end; std::chrono::microseconds duration;

#define BENCHMARK(name, bloc_of_code) \
	start = std::chrono::high_resolution_clock::now(); \
	bloc_of_code; \
	end = std::chrono::high_resolution_clock::now();\
	duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); \
	std::cout << "\nExecution of '" << name << "' took : " << duration.count() << " microsecond(s)" << std::endl; \

void HandleEvent(float dt, Mat4f& projecton, Camera& camera, const Event& e);

float vertices[] = {      
   25.0f, -0.5f,  25.0f,
   -25.0f, -0.5f,  25.0f,
   -25.0f, -0.5f, -25.0f,

	25.0f, -0.5f,  25.0f,
	-25.0f, -0.5f, -25.0f,
	25.0f, -0.5f, -25.0f,
};

float textures[] = {
	25.0f,  0.0f,
	0.0f,  0.0f,
	0.0f, 25.0f,

	25.0f,  0.0f,
	0.0f, 25.0f,
	25.0f, 25.0f
};

float normals[] = {
	0.0f, 1.0f, 0.0f, 
	0.0f, 1.0f, 0.0f, 
	0.0f, 1.0f, 0.0f,  

	0.0f, 1.0f, 0.0f, 
	0.f, 1.0f, 0.0f,  
    0.0f, 1.0f, 0.0f, 
};


float deltaTime = 0.5f;
double  frameRate = 30;
bool start = false;
int32 speed = 1;
const unsigned int SCR_WIDTH = 1920 / 2;
const unsigned int SCR_HEIGHT = 1080 / 2;

float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
ShaderProgram& debugQuad(TextureID depthMap);
void RenderDebugQuad();

void main2()
{
	TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (OpenGL 4.3)", WindowStyle::Resize);
	window.initContext(4, 3);
	ShaderParser generic_vs("Forward/generic.vs");
	ShaderParser generic_fs("Forward/generic.fs");

	ShaderID shader_id2 = ResourcesManager::Instance().AllocateResource<ShaderProgram>(
		Shader(generic_vs.GetCode(), ShaderType::VERTEX),
		Shader(generic_fs.GetCode(), ShaderType::FRAGMENT)
	);

	ShaderID shader_id3 = ResourcesManager::Instance().AllocateResource<ShaderProgram>(
		Shader(generic_vs.Define({ "TEXTURED" }), ShaderType::VERTEX),
		Shader(generic_fs.Define({ "TEXTURED" }), ShaderType::FRAGMENT)
	);
	ShaderID shader_id4 = ResourcesManager::Instance().AllocateResource<ShaderProgram>(
		Shader(generic_vs.Define({ "TEXTURED", "SHADOWS" }), ShaderType::VERTEX),
		Shader(generic_fs.Define({ "TEXTURED", "SHADOWS" }), ShaderType::FRAGMENT)
	);
}

int main()
{
	/*main2();
	getchar();
	return 0;*/

	// settings
	TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (OpenGL 4.3)", WindowStyle::Resize);
	window.initContext(4, 3);

	printf("- GPU Vendor........: %s\n", glGetString(GL_VENDOR));
	printf("- Graphics..........: %s\n", glGetString(GL_RENDERER));
	printf("- Version...........: %s\n", glGetString(GL_VERSION));
	printf("- GLSL Version......: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("- Hardware Threads..: %d\n", std::thread::hardware_concurrency());

	ShaderParser generic_vs("Forward/generic.vs");
	ShaderParser generic_fs("Forward/generic.fs");

	ShaderID shader_id2 = ResourcesManager::Instance().AllocateResource<ShaderProgram>(
		Shader(generic_vs.GetCode(), ShaderType::VERTEX), 
		Shader(generic_fs.GetCode(), ShaderType::FRAGMENT)
	);
	ShaderID shader_id3 = ResourcesManager::Instance().AllocateResource<ShaderProgram>(
		Shader(generic_vs.Define({ "TEXTURED" }), ShaderType::VERTEX), 
		Shader(generic_fs.Define({ "TEXTURED" }), ShaderType::FRAGMENT)
	);
	ShaderID shader_id4 = ResourcesManager::Instance().AllocateResource<ShaderProgram>(
		Shader(generic_vs.Define({ "TEXTURED", "SHADOWS" }), ShaderType::VERTEX), 
		Shader(generic_fs.Define({ "TEXTURED", "SHADOWS" }), ShaderType::FRAGMENT)
	);
	{
		for (uint32 i = shader_id2; i <= shader_id4; i++) {
			ShaderProgram& shader = ResourcesManager::Instance().Get<ShaderProgram>(i);
			shader.LinkProgram();
			shader.Use();

			shader.SetUniformBlockBinding("LightUBO", 0);

			// shader.AddUniform("MVP");
			shader.AddUniform("u_ViewPosition");
			shader.AddUniform("u_ProjView");
			shader.AddUniform("u_Model");
			shader.AddUniform("u_LightSpaceMatrix");

			shader.AddUniform("material.ambient");
			shader.AddUniform("material.diffuse");
			shader.AddUniform("material.specular");
			shader.AddUniform("material.alpha");
			shader.AddUniform("material.shininess");

			shader.AddSamplerSlot("material.specular_tex");
			shader.AddSamplerSlot("material.diffuse_tex");
			shader.AddSamplerSlot("u_ShadowMap");

			shader.SetFloat("material.alpha", 1.f);
			shader.SetFloat("material.shininess", 1.0f);
		}
	}

	ForwardRenderer renderer;
	renderer.Initialize(SCR_WIDTH, SCR_HEIGHT);
	CommandBucket& bucket = renderer.GetCommandQueue().GetCommandBucker(ForwardRenderer::MAIN_PASS);
	EntityManager& ent_manager = ResourcesManager::Instance().GetRenderWorld().GetEntityManager();

	ModelData data;
	data.vertices = vertices;
	data.vertexSize = ARRAY_SIZE(vertices);
	data.textures = textures;
	data.textureSize = ARRAY_SIZE(textures);
	data.normals = normals;
	data.normalSize = ARRAY_SIZE(normals);
	Model model(data);
	AbstractMaterial* mat = model.GetMaterials().At(0).material;
	mat->GetRenderStates().cull_enabled = false;
	mat->GetParametres().AddParameter<TextureID>("material.diffuse_tex", mat->AddTexture("res/img/wood_texture.png"));
	mat->GetParametres().AddParameter<TextureID>("material.specular_tex", mat->AddTexture("res/img/wood_texture.png"));
	StaticMesh plane_mesh = model.LoadMesh(shader_id4);
	plane_mesh.GetTransformationMatrix().scale(vec3(1.0, 1.0, 1.0) * 2.f);
	//plane_mesh.GetTransformationMatrix().rotate(vec3(1.f, 0.f, 0.f), Math::ToRad(90.0));
	
	MeshLoader loader("res/obj/lowpoly/carrot_box.obj");
	Model carrot = loader.LoadAsOneObject();
	StaticMesh carrot_mesh = carrot.LoadMesh(shader_id2);
	carrot_mesh.GetTransformationMatrix().translate(vec3(-1.f, 3.f, 2.f));

	MeshLoader loader2("res/obj/lowpoly/deagle.obj");
	RenderState& state  = loader2.GetMaterialLoader().GetMaterialFromName("Fire.001")->GetRenderStates();
	state.blend_enabled = true;
	state.blending = RenderState::BlendingOptions();
	Model gun = loader2.LoadAsOneObject();
	StaticMesh gun_mesh = gun.LoadMesh(shader_id4);
	gun_mesh.GetTransformationMatrix().translate(vec3(0.0, 1.0, 0.f));

	/*DirectionalLightComponent comp;
	comp.SetDirection(vec3(0, -1.2, 0));
	comp.SetLightColor(vec3(0.1f, 0.5f, 0.2f));
	renderer.GetLightSystem().AddLight((Mat4f*)&comp);*/
	vec3 light_pos = vec3(-2.0, 8.0, -1.0);
	PointLightComponent pointlight;
	pointlight.SetPosition(light_pos);
	pointlight.SetLightColor(vec3(1,1,1));
	pointlight.SetLinear(0.09f);
	pointlight.SetQuadratic(0.032f);
	pointlight.SetConstant(0.5f);
	renderer.GetLightSystem().AddLight((Mat4f*)&pointlight);
	// ent1.GetComponent<TransformComponent>()->transform_matrix.translate(light_pos);
	// ent1.GetComponent<TransformComponent>()->transform_matrix.scale(vec3(0.2, 0.2, 0.2));

	Event ev;
	Enable(Capability::DEPTH_TEST);
	// Enable(Capability::CULL_FACE);
	Enable(GL_MULTISAMPLE);
	// ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	ContextOperationQueue& op_queue = ResourcesManager::Instance().GetContextOperationsQueue();


	ShaderProgram& debugShader = debugQuad(renderer.GetDepthMap());
	/*CommandBucket& debugBucket = renderer.GetCommandQueue().CreateBucket();
	uint8* extra_data = debugBucket.GetExtraBuffer();
	new (extra_data) uint32(renderer.GetDepthMap());
	debugBucket.SetOnKeyChangeCallback(
		[](ResourcesManager& manager, const BucketKey& key,
			const Mat4f& proj_view, const Mat4f& proj,
			const Camera& camera, const uint8* extra_data) -> ShaderProgram& {
		uint32 shader_id;
		RenderState state = RenderState::FromKey(key, &shader_id);
		state.ApplyStates();

		// Set Shader
		ShaderProgram& shader = manager.Get<ShaderProgram>(ResourcesTypes::SHADER, shader_id);
		shader.Bind();
		const uint32& depth_map_id = *reinterpret_cast<const uint32*>(extra_data);
		Texture& shadow_map = manager.Get<Texture>(ResourcesTypes::TEXTURE, depth_map_id);
		ActivateTexture(0);
		shadow_map.Bind();
		return shader;
	});*/
	INIT_BENCHMARK;
	while (true) {
		//BENCHMARK("Render Thread",

		if (window.getEvent(ev)) {
			HandleEvent(deltaTime, bucket.GetProjectionMatrix(), bucket.GetCamera(), ev);
		}

		op_queue.Flush();

		//ResourcesManager::Instance().GetRenderWorld().UpdateSystems(0);
		//renderer.Render();
		//renderer.Draw(mesh);
		renderer.Draw(carrot_mesh);
		renderer.Draw(plane_mesh);
		renderer.Draw(gun_mesh);
		// debugScreen.Submit(debugBucket, -1, 0);
		renderer.Render();

		/*debugShader.Bind();
		ActivateTexture(0);
		Texture& texture = ResourcesManager::Instance().Get<Texture>(ResourcesTypes::TEXTURE, renderer.GetDepthMap());
		texture.Bind();
		RenderDebugQuad();*/

		window.Present();

		//);
	}

	getchar();
	return 0;
}


void HandleEvent(float dt, Mat4f& projecton, Camera& camera, const Event& e)
{
	if (e.Type == Event::TE_RESIZE) {
		glViewport(0, 0, e.Window.Width, e.Window.Height);
		projecton = mat4::perspective((float)camera.Zoom, (float)e.Window.Width / (float)e.Window.Height, 0.1f, 300.f);
		printf("DETECTING RESIZE EVENT(%d, %d)\n", e.Window.Width, e.Window.Height);
	} else if (e.Type == Event::TE_KEY_DOWN) {
		switch (e.Key.Code) {
		case Key::Up:
			speed += 1;
			printf("Changing camera speed to %d\n", speed);
			break;
		case Key::Down:
			speed -= 1;
			printf("Changing camera speed to %d\n", speed);
			break;
		case Key::Space:
			start = !start;
			deltaTime = 0;
			break;
		case Key::Z:
			camera.ProcessKeyboard(FORWARD, speed * dt);
			break;
		case Key::S:
			camera.ProcessKeyboard(BACKWARD, speed * dt);
			break;
		case Key::Q:
			camera.ProcessKeyboard(LEFT, speed * dt);
			break;
		case Key::D:
			camera.ProcessKeyboard(RIGHT, speed * dt);
			break;
		default:
			break;
		}
	} else if (e.Type == Event::TE_MOUSE_MOVE) {
		float xpos = (float)e.Mouse.X;
		float ypos = (float)e.Mouse.Y;

		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

ShaderProgram& debugQuad(TextureID depthMap)
{
	ShaderID shader = ResourcesManager::Instance().AllocateResource<ShaderProgram>(
		Shader(File("res/Shader/Forward/Debug/debug_quad.vs"), ShaderType::VERTEX),
		Shader(File("res/Shader/Forward/Debug/debug_quad.fs"), ShaderType::FRAGMENT)
	);
	{
		ShaderProgram& s = ResourcesManager::Instance().Get<ShaderProgram>(shader);
		s.LinkProgram();
		s.Use();

		s.AddUniform("u_near_plane");
		s.AddUniform("u_far_plane");
		s.AddUniform("u_Model");
		s.AddSamplerSlot("u_DepthMap");

		s.SetInt("u_DepthMap", 0);
		s.SetFloat("u_near_plane", 1.f);
		s.SetFloat("u_far_plane", 50.f);
		return s;
	}

	/*float quadVertices[] = {
		// positions
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
	};

	float quadTextures[] = {
		// texture Coords
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};

	ModelData data;
	data.vertices = quadVertices;
	data.vertexSize = ARRAY_SIZE(quadVertices);
	data.textures = quadTextures;
	data.textureSize = ARRAY_SIZE(quadTextures);
	Model model(data);
	AbstractMaterial* mat = model.GetMaterials().At(0).material;
	mat->GetRenderStates().cull_enabled = false;
	mat->GetParametres().AddParameter<float>("u_near_plane", 1.f);
	mat->GetParametres().AddParameter<float>("u_far_plane", 7.5f);
	mat->GetParametres().AddParameter<TextureID>("u_DepthMap", depthMap);
	StaticMesh screen = model.LoadMesh(shader);
	return screen;*/
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void RenderDebugQuad()
{
	if (quadVAO == 0) {
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

// #endif
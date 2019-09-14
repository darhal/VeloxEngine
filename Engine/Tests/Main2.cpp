//#ifdef BLABLA

#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <time.h>

#include <Core/Context/Extensions.hpp>
#include <Core/Window/Window.hpp>
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Utils/Image.hpp>
#include <Core/Context/Context.hpp>
#include <Core/FileSystem/Directory/Directory.hpp>
#include <Core/Memory/Allocators/StackAlloc/DoubleStackAllocator.hpp>
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

#include <RenderEngine/Loader/MeshLoader.hpp>
#include <RenderEngine/Mesh/ModelLoader/ModelLoader.hpp>
#include <RenderEngine/Renderer/Frontend/Camera/Camera.hpp>
#include <RenderEngine/Mesh/StaticMesh/StaticMesh.hpp>
#include <RenderEngine/Managers/ResourcesManager/ResourcesManager.hpp>
#include <RenderEngine/Scenegraph/Scene/Scene.hpp>
#include <RenderEngine/Renderer/Backend/CommandBuffer/ICommandBuffer/ICommandBuffer.hpp>
#include <RenderEngine/Renderer/Backend/CommandBuffer/RenderCommandBucket/RenderCommandBucket.hpp>
#include <RenderEngine/Managers/RenderManager/RenderManager.hpp>

using namespace TRE;

// world space positions of our cubes
vec3 cubePositions[] = {
	vec3(0.0f,  0.0f,  0.0f),
	vec3(2.0f,  5.0f, -15.0f),
	vec3(-1.5f, -2.2f, -2.5f),
	vec3(-3.8f, -2.0f, -12.3f),
	vec3(2.4f, -0.4f, -3.5f),
	vec3(-1.7f,  3.0f, -7.5f),
	vec3(1.3f, -2.0f, -2.5f),
	vec3(1.5f,  2.0f, -2.5f),
	vec3(1.5f,  0.2f, -1.5f),
	vec3(-1.3f,  1.0f, -1.5f)
};

// settings
const unsigned int SCR_WIDTH = 1900/2;
const unsigned int SCR_HEIGHT = 1000/2;

float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// camera
Camera camera(vec3(0.0f, 0.0f, 3.0f));
double deltaTime = 0;
double  frameRate = 30;
double  averageFrameTimeMilliseconds = 33.333;
bool vsync = true;
bool start = false;
int32 speed = 1;
vec3 LightPos = vec3(-0.2f, -1.0f, -0.3f);
mat4 lightProjection = mat4::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f);
mat4 lightView = mat4::look_at(LightPos, vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f));

double clockToMilliseconds(clock_t ticks){
    // units/(units/time) => time (seconds) * 1000 = milliseconds
    return (ticks/(double)CLOCKS_PER_SEC)*1000.0;
}

void HandleEvent(Scene* scene, const Event&);
void clip(const TRE::Window&);
void PrepareThread(Scene* scene, TRE::Window*);

void empty_job1(Job* t, const void*)
{
	printf("Hello! From %d\n", t->m_WorkerID);
}

void empty_job2(Job* t, const void*)
{
	printf("Hello! From %d\n", t->m_WorkerID);
}


void DoJobs1(TaskExecutor* ts, int8 x)
{
	Task* root = Task::CreateTask(&empty_job1);
	ts->Run(root);
	for (usize i = 0; i < 7; i++){
		Task* t = Task::CreateTask(&empty_job1);
		ts->Run(t);
	}
	ts->Wait(root);
}

void DoJobs2(TaskExecutor* ts, int8 x)
{
	Task* root = Task::CreateTask(&empty_job1);
	ts->Run(root);
	for (usize i = 0; i < 3; i++){
		Task* t = Task::CreateTask(&empty_job2);
		ts->Run(t);
	}
	ts->Wait(root);
}

/*
	MULTI THREADED:
		T1 : Render Thread (Handles also events)
		T2 : Updte Thread (Updates the command buffer)
		TEST1 :
			[T2] : MIN FPS = 247| MAX FPS = 100000 | AVG FPS = 1194
			[T1] : MIN FPS = 60| MAX FPS = 8403 | AVG FPS = 905
		TEST2 :
			[T2] : MIN FPS = 313| MAX FPS = 90909 | AVG FPS = 1312
			[T1] : MIN FPS = 30| MAX FPS = 6944 | AVG FPS = 819

	---------------------------------------------------------------------------
	SINGLE THREADED:
		TEST1:
			[T1] : MIN FPS = 340| MAX FPS = 3816 | AVG FPS = 967
		TEST2:
			[T1] : MIN FPS = 348| MAX FPS = 3174 | AVG FPS = 881
*/

std::atomic<uint8> IsWindowOpen;


void LoadObjects(Scene* scene)
{
	String mesh_file_path;

	/*StaticMesh* room = new StaticMesh();
	{
		mesh_file_path.Clear();
		mesh_file_path = "res/obj/lowpoly/room/room_r.obj";
		MeshLoader obj_loader(mesh_file_path.Buffer());

		for(auto& mat : obj_loader.GetMaterialLoader().GetMaterials()){
			mat.second.GetRenderStates().cull_enabled = false;
		}

		ModelLoader loader(std::move(obj_loader.LoadAsOneObject()));
		loader.ProcessData(*room, 1);
		scene->AddMeshInstance(room);
	}*/

	StaticMesh* deagle = new StaticMesh();
	{
		mesh_file_path.Clear();
		mesh_file_path = "res/obj/lowpoly/deagle.obj";
		// res_dir.SearchRecursive("deagle.obj", mesh_file_path);
		MeshLoader obj_loader(mesh_file_path.Buffer());
		obj_loader.GetMaterialLoader().GetMaterialFromName("Fire.001").GetRenderStates().blend_enabled = true;
		obj_loader.GetMaterialLoader().GetMaterialFromName("Fire.001").GetRenderStates().cull_enabled = false;
		for (auto& mat_node : obj_loader.GetMaterialLoader().GetMaterials()) {
			mat_node.second.GetParametres().AddParameter<TextureID>("shadowMap", RenderManager::GetRenderer().GetShadowMap());
		}
		ModelLoader loader(std::move(obj_loader.LoadAsOneObject()));
		loader.ProcessData(*deagle);
		scene->AddMeshInstance(deagle);
	}
	deagle->GetTransformationMatrix().translate(vec3(0.f, 3.f, 0.f));

	StaticMesh* carrot_box = new StaticMesh();
	{
		mesh_file_path.Clear();
		mesh_file_path = "res/obj/lowpoly/carrot_box.obj";
		// res_dir.SearchRecursive("carrot_box.obj", mesh_file_path);
		MeshLoader obj_loader(mesh_file_path.Buffer());
		for (auto& mat_node : obj_loader.GetMaterialLoader().GetMaterials()) {
			mat_node.second.GetParametres().AddParameter<TextureID>("shadowMap", RenderManager::GetRenderer().GetShadowMap());
		}
		ModelLoader loader(std::move(obj_loader.LoadAsOneObject()));
		loader.ProcessData(*carrot_box, 1);
		scene->AddMeshInstance(carrot_box);
	}
	carrot_box->GetTransformationMatrix().translate(vec3(10.f, 5.f, -3.f));

	StaticMesh* trees = new StaticMesh();
	{
		mesh_file_path.Clear();
		mesh_file_path = "res/obj/lowpoly/all_combined_smooth.obj";
		//res_dir.SearchRecursive("all_combined_smooth.obj", mesh_file_path);
		MeshLoader obj_loader(mesh_file_path.Buffer());
		for (auto& mat_node : obj_loader.GetMaterialLoader().GetMaterials()) {
			mat_node.second.GetParametres().AddParameter<TextureID>("shadowMap", RenderManager::GetRenderer().GetShadowMap());
		}
		ModelLoader loader(std::move(obj_loader.LoadAsOneObject()));
		loader.ProcessData(*trees, 1);
		scene->AddMeshInstance(trees);

	}
	trees->GetTransformationMatrix().translate(vec3(0, 0, 6));

	float planeVertices[] = {
		// positions          // texture Coords 
		50.0f, -0.5f,  50.0f,
		-50.0f, -0.5f,  50.0f,
		-50.0f, -0.5f, -50.0f,

		 50.0f, -0.5f,  50.0f,
		-50.0f, -0.5f, -50.0f,
		50.0f, -0.5f, -50.0f,
	};
	float planeTexCoords[] = {
		20.0f, 0.0f, 0.0f, 0.0f, 0.0f, 20.0f,
		20.0f, 0.0f, 0.0f, 20.0f, 20.0f, 20.0f
	};
	uint32 indices2[] = { 0, 1, 2, 3, 4, 5 };

	StaticMesh* plane = new StaticMesh();
	ModelSettings settings2(planeVertices, planeTexCoords, planeVertices, indices2);
	settings2.CopyData();
	ModelLoader loader2(settings2);
	loader2.GetMaterials().PopBack();
	AbstractMaterial abst_mat2;
	abst_mat2.GetRenderStates().cull_enabled = false;
	abst_mat2.GetParametres().AddParameter<TextureID>("material.diffuse_tex", abst_mat2.AddTexture("res/img/ground.jpg"));
	abst_mat2.GetParametres().AddParameter<TextureID>("material.specular_tex", abst_mat2.AddTexture("res/img/ground.jpg"));
	abst_mat2.GetParametres().AddParameter<TextureID>("shadowMap", RenderManager::GetRenderer().GetShadowMap());
	abst_mat2.GetParametres().AddParameter<float>("material.shininess", 500.f);
	loader2.GetMaterials().EmplaceBack(abst_mat2, loader2.GetVertexCount());
	loader2.ProcessData(*plane);
	scene->AddMeshInstance(plane);

	// plane->GetTransformationMatrix().scale(vec3(50.f, 0.f, 50.f));
}

void RenderThread()
{
	TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (OpenGL 3.3)", WindowStyle::Resize);
	window.initContext(3, 3);
	
	printf("- GPU Vendor    	: %s\n", glGetString(GL_VENDOR));
	printf("- Graphics      	: %s\n", glGetString(GL_RENDERER));
	printf("- Version       	: %s\n", glGetString(GL_VERSION));
	printf("- GLSL Version  	: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("- Hardware Threads 	: %d\n", std::thread::hardware_concurrency()); 

	Scene& scene = RenderManager::GetRenderer().GetScene();

	typename RMI<ShaderProgram>::ID shaderID, texShaderID, scrShader, shadowMapDepthID;
	
	ResourcesManager::GetGRM().Create<ShaderProgram>(shaderID, 
		Shader("res/Shader/texture.vs", ShaderType::VERTEX),
		Shader("res/Shader/texture.fs", ShaderType::FRAGMENT)
	);

	ResourcesManager::GetGRM().Create<ShaderProgram>(texShaderID, 
		Shader("res/Shader/cam.vs", ShaderType::VERTEX),
		Shader("res/Shader/cam.fs", ShaderType::FRAGMENT)
	);

	VBO vertexUBO(BufferTarget::UNIFORM_BUFFER);
	vertexUBO.FillData(NULL, 4 * sizeof(mat4) + sizeof(vec4));
	vertexUBO.Unbind();

	for (auto& shader_id : ResourcesManager::GetGRM().GetResourceContainer<ShaderProgram>()){
		ShaderProgram& shader = shader_id.second;

		shader.BindAttriute(0, "aPos");
		shader.BindAttriute(1, "aNormals");
		shader.BindAttriute(2, "aTexCoord");
		shader.LinkProgram();
		shader.Use();

		shader.AddUniform("MVP");
		shader.AddUniform("model");
		shader.AddUniform("shadowMap");

		shader.AddUniform("light.diffuse");
		shader.AddUniform("light.position");
		//shader.AddUniform("light.specular");
		//shader.AddUniform("light.ambient");
		
		shader.AddUniform("material.ambient");
		shader.AddUniform("material.diffuse");
		shader.AddUniform("material.specular");
		shader.AddUniform("material.alpha");
		shader.AddUniform("material.shininess");
		shader.AddUniform("material.diffuse_tex");
		shader.AddUniform("material.specular_tex");

		// shader.SetVec3("light.ambient", 0.09f, 0.09f, 0.09f);
		shader.SetVec3("light.diffuse", 0.7f, 0.7f, 0.7f);
		shader.SetVec3("light.position", LightPos);
		shader.SetFloat("material.alpha", 1.f);
		shader.SetFloat("material.shininess", 1.0f);
		
		vertexUBO.Bind();
		shader.SetUniformBlockBinding("VertexBlock", 0);
		shader.BindBufferRange(vertexUBO, 0, 0, 4 * sizeof(mat4) + sizeof(vec4));
	}

	ResourcesManager::GetGRM().Create<ShaderProgram>(scrShader,
		Shader("res/Shader/screen.vs", ShaderType::VERTEX),
		Shader("res/Shader/screen.fs", ShaderType::FRAGMENT)
	);

	ResourcesManager::GetGRM().Create<ShaderProgram>(shadowMapDepthID,
		Shader("res/Shader/shadow_mapping_depth.vs", ShaderType::VERTEX),
		Shader("res/Shader/shadow_mapping_depth.fs", ShaderType::FRAGMENT)
	);

	ShaderProgram& shader = ResourcesManager::GetGRM().Get<ShaderProgram>(scrShader);
	shader.BindAttriute(0, "aPos");
	shader.BindAttriute(1, "aTexCoord");
	shader.LinkProgram();
	shader.SetInt(shader.AddUniform("screenTexture").second, 0);

	ShaderProgram& shadow_shader = ResourcesManager::GetGRM().Get<ShaderProgram>(shadowMapDepthID);
	shadow_shader.BindAttriute(0, "aPos");
	shadow_shader.LinkProgram();
	shadow_shader.AddUniform("MVP");
	shadow_shader.AddUniform("model");

	IsWindowOpen = true;
	std::thread prepareThread(PrepareThread, &scene, &window);

	clock_t fps = 0, avgfps = 0, maxfps = 0, minfps = 9999999;
	double avgdt = 0, maxdt = 0, mindt = 9999999;
	uint64 frames = 1;
	Event ev;
	printf("\n");

	RenderSettings::DEFAULT_STATE.ApplyStates();
	while (IsWindowOpen) {
		auto start = std::chrono::high_resolution_clock::now();

		if (window.getEvent(ev)) {
			HandleEvent(&scene, ev);
		}

		IsWindowOpen = window.isOpen();

		vertexUBO.Bind();
		mat4 t_proj = scene.GetProjectionMatrix()->transpose();
		mat4 t_view = scene.GetCurrentCamera()->GetViewMatrix().transpose();
		mat4 t_proj_view = t_view * t_proj;
		mat4 t_light_space_mat = (lightProjection * lightView).transpose();
		vertexUBO.SubFillData(&t_proj, 0, sizeof(mat4));
		vertexUBO.SubFillData(&t_view, sizeof(mat4), sizeof(mat4));
		vertexUBO.SubFillData(&t_proj_view, 2 * sizeof(mat4), sizeof(mat4));
		vertexUBO.SubFillData(&t_light_space_mat, 3 * sizeof(mat4), sizeof(mat4));
		vertexUBO.SubFillData(&scene.GetCurrentCamera()->Position, 4 * sizeof(mat4), sizeof(vec4));
		vertexUBO.Unbind();

		RenderManager::Update();

		auto end = std::chrono::high_resolution_clock::now();
		auto diff = end - start;
		auto dt = std::chrono::duration<double, std::milli>(diff).count();
		deltaTime += dt;

		window.Present();
	}

	prepareThread.join();
	printf("[T1] : MIN FPS = %lf | MAX FPS = %lf | AVG FPS = %lf\n", mindt, maxdt, avgdt / frames);
	getchar();
}

void PrepareThread(Scene* scene, TRE::Window* window)
{
#if defined(OS_LINUX)
	Directory res_dir("res/");
#endif
	RenderManager::GetRenderer().GetRenderCommandBuffer().GetRenderTarget(0)->m_Projection = scene->GetProjectionMatrix();
	RenderManager::GetRenderer().GetRenderCommandBuffer().GetRenderTarget(0)->m_View = &scene->GetCurrentCamera()->GetViewMatrix();
	RenderManager::GetRenderer().GetRenderCommandBuffer().GetRenderTarget(0)->m_Width = SCR_WIDTH;
	RenderManager::GetRenderer().GetRenderCommandBuffer().GetRenderTarget(0)->m_Height = SCR_HEIGHT;
	RenderManager::Init();

	//auto lightProjection = mat4::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.f, 7.5f);
	//auto lightView = mat4::look_at(LightPos, vec3(0.f, 0.f, 0.f), vec3(0.0, 1.0, 0.0));
	//RenderManager::GetRenderer().GetRenderCommandBuffer().GetRenderTarget(0)->m_Projection = &lightProjection;
	//RenderManager::GetRenderer().GetRenderCommandBuffer().GetRenderTarget(0)->m_View = &lightView;

	LoadObjects(scene);

	//RenderManager::GetRRC().GetResourcesCommandBuffer().SwapCmdBuffer();

	while(IsWindowOpen){
		auto& render_cmd = RenderManager::GetRenderer().GetRenderCommandBuffer();

		if(render_cmd.SwapCmdBuffer()){
			render_cmd.Clear(); // Clear write side
			scene->Submit(); // write on write
		}
	}

	//printf("[T2] : MIN FPS = %lf | MAX FPS = %lf | AVG FPS = %lf\n", mindt, maxdt, avgdt / frames);
}

void output(int x)
{
	printf("X is %d\n", x);
}

int main()
{
	RenderThread();
}

void PrintScreenshot()
{
	Color* pixels = new Color[SCR_WIDTH * SCR_HEIGHT];
	glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	Image img = Image(SCR_WIDTH, SCR_HEIGHT, pixels);
	img.Save("test.png", ImageFileFormat::PNG);
	printf("Screenshot taken.\n");
	delete[] pixels;
}

void HandleEvent(Scene* scene, const Event& e)
{
	if (e.Type == Event::TE_RESIZE) {
		glViewport(0, 0, e.Window.Width, e.Window.Height);
		scene->CreateProjectionMatrix(Vec2f(e.Window.Width, e.Window.Height), Scene::NEAR_PLANE, Scene::FAR_PLANE);
		printf("DETECTING RESIZE EVENT(%d, %d)\n", e.Window.Width, e.Window.Height);
	}else if (e.Type == Event::TE_KEY_DOWN) {
		if (e.Key.Code == Key::F11) {
			PrintScreenshot();
			printf("Taking screenshot...\n");
		}
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
			scene->GetCurrentCamera()->ProcessKeyboard(FORWARD, speed * (float)clockToMilliseconds(deltaTime)/1000.f);
			break;
		case Key::S:
			scene->GetCurrentCamera()->ProcessKeyboard(BACKWARD, speed * (float)clockToMilliseconds(deltaTime)/1000.f);
			break;
		case Key::Q:
			scene->GetCurrentCamera()->ProcessKeyboard(LEFT, speed * (float)clockToMilliseconds(deltaTime)/1000.f);
			break;
		case Key::D:
			scene->GetCurrentCamera()->ProcessKeyboard(RIGHT, speed * (float)clockToMilliseconds(deltaTime)/1000.f);
			break;
		default:
			break;
		}
	}else if (e.Type == Event::TE_MOUSE_MOVE) {
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

		scene->GetCurrentCamera()->ProcessMouseMovement(xoffset, yoffset);
	}
}

/*void clip(const Window& win)
{
	RECT _clip;
	HWND _window = win.window;

	//Create a RECT out of the window
	GetWindowRect(_window, &_clip);

	//Modify the rect slightly, so the frame doesn't get clipped with
	_clip.left += 5;
	_clip.top += 30;
	_clip.right -= 5;
	_clip.bottom -= 5;

	//Clip the RECT
	ClipCursor(&_clip);
	ShowCursor(FALSE);

}*/

//#endif


/*********************
	float quadVertices[] = { // positions
		// positions
		-1.0f, 1.0f, 0.f,
		-1.0f, -1.0f, 0.f,
		1.0f, -1.0f, 0.f,
		-1.0f, 1.0f, 0.f,
		1.0f, -1.0f, 0.f,
		1.0f,  1.0f, 0.f,
	};
	float quadTexCoords[] = { // texture Coords
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};
	//uint32 indices[] = {0, 1, 2, 3, 4, 5};

	StaticMesh quad;
	ModelSettings settings;
	settings.vertices = quadVertices;
	settings.vertexSize = ARRAY_SIZE(quadVertices);
	settings.textures = quadTexCoords;
	settings.textureSize = ARRAY_SIZE(quadTexCoords);

	// Creating a frame buffer.
	TextureID tex_id = 0; FboID fbo_id = 0; RboID rbo_id = 0;
	auto& rrc = RenderManager::GetRRC();
	//Commands::CreateTexture* tex_cmd = res_buffer.AddCommand<Commands::CreateTexture>(0);
	//tex_cmd->texture = ResourcesManager::GetGRM().Create<Texture>(tex_id);
	auto tex_settings = TextureSettings(TexTarget::TEX2D, SCR_WIDTH, SCR_HEIGHT, NULL,
		Vector<TexParamConfig>{
			{ TexParam::TEX_MIN_FILTER, TexFilter::LINEAR },
			{ TexParam::TEX_MAG_FILTER, TexFilter::LINEAR }
		}
	);
	auto tex_cmd = rrc.CreateResource<Commands::CreateTexture>(&tex_id, tex_settings);

	//Commands::CreateRenderBuffer* rbo_cmd = res_buffer.AppendCommand<Commands::CreateRenderBuffer>(tex_cmd);
	//rbo_cmd->rbo = ResourcesManager::GetGRM().Create<RBO>(rbo_id);
	//rbo_cmd->settings = RenderbufferSettings(SCR_WIDTH, SCR_HEIGHT);
	auto rbo_cmd = rrc.CreateResourceAfter<Commands::CreateRenderBuffer>(tex_cmd, NULL, RenderbufferSettings(SCR_WIDTH, SCR_HEIGHT));

	//Commands::CreateFrameBuffer* fbo_cmd = res_buffer.AppendCommand<Commands::CreateFrameBuffer>(rbo_cmd);
	//fbo_cmd->fbo = ResourcesManager::GetGRM().Create<FBO>(fbo_id);
	//fbo_cmd->settings = FramebufferSettings({ tex_cmd->texture }, FBOTarget::FBO, rbo_cmd->rbo);
	auto fbo_cmd = rrc.CreateResourceAfter<Commands::CreateFrameBuffer>(rbo_cmd, &fbo_id, FramebufferSettings({ tex_cmd->texture }, FBOTarget::FBO, rbo_cmd->rbo));

	ModelLoader loader(settings, indices2);
	loader.GetMaterials().PopBack();
	AbstractMaterial abst_mat;
	abst_mat.GetRenderStates().depth_enabled = false;
	// abst_mat.GetRenderStates().poly_mode = PolygonMode::polygon_mode_t::LINE;
	ShaderID shaderID = 2;
	abst_mat.GetParametres().AddParameter<TextureID>("screenTexture", tex_id);
	loader.GetMaterials().EmplaceBack(abst_mat, loader.GetVertexCount());
	loader.ProcessData(quad, shaderID);

	// Setting up render targets.
	RenderManager::GetRenderer().GetRenderCommandBuffer().PopRenderTarget();
	RenderManager::GetRenderer().GetRenderCommandBuffer().PushRenderTarget(RenderTarget(fbo_id, SCR_WIDTH, SCR_HEIGHT, scene->GetProjectionMatrix(), &scene->GetCurrentCamera()->GetViewMatrix()));

	Renderer::FramebufferCmdBuffer::FrameBufferPiriority f;
	RenderTarget* rt = ResourcesManager::GetGRM().Create<RenderTarget>(f.render_target_id);
	rt->m_Width = SCR_WIDTH;
	rt->m_Height = SCR_HEIGHT;
	//auto key = RenderManager::GetRenderer().GetFramebufferCommandBuffer().GenerateKey(f, shaderID, quad.GetVaoID(), quad.GetSubMeshes().At(0).m_MaterialID);
	//auto cmd = RenderManager::GetRenderer().GetFramebufferCommandBuffer().AddCommand<Commands::DrawCmd>(key);
	//auto& obj = quad.GetSubMeshes().At(0);
	//cmd->mode = obj.m_Geometry.m_Primitive;
	//cmd->start = 0;
	//cmd->end = loader.GetVertexCount();
	//cmd->model = &quad.GetTransformationMatrix();
	//RenderManager::GetRenderer().GetFramebufferCommandBuffer().SwapCmdBuffer();

	quad.Submit(RenderManager::GetRenderer().GetFramebufferCommandBuffer(), f.render_target_id);
	RenderManager::GetRenderer().GetFramebufferCommandBuffer().SwapCmdBuffer();
*********************/
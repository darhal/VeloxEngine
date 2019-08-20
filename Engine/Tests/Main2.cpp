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

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
float vertices[] = {
	-0.5f, -0.5f, -0.5f, 
	0.5f, -0.5f, -0.5f, 
	0.5f,  0.5f, -0.5f,  
	0.5f,  0.5f, -0.5f, 
	-0.5f,  0.5f, -0.5f,  
	-0.5f, -0.5f, -0.5f, 

	-0.5f, -0.5f,  0.5f,  
	0.5f, -0.5f,  0.5f,  
	0.5f,  0.5f,  0.5f,  
	0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f,  
	-0.5f, -0.5f,  0.5f, 

	-0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f, -0.5f, 
	-0.5f, -0.5f, -0.5f,  
	-0.5f, -0.5f, -0.5f,  
	-0.5f, -0.5f,  0.5f, 
	-0.5f,  0.5f,  0.5f,  

	0.5f,  0.5f,  0.5f,  
	0.5f,  0.5f, -0.5f,  
	0.5f, -0.5f, -0.5f,  
	0.5f, -0.5f, -0.5f,  
	0.5f, -0.5f,  0.5f, 
	0.5f,  0.5f,  0.5f, 

	-0.5f, -0.5f, -0.5f,  
	0.5f, -0.5f, -0.5f,  
	0.5f, -0.5f,  0.5f,  
	0.5f, -0.5f,  0.5f,  
	-0.5f, -0.5f,  0.5f,  
	-0.5f, -0.5f, -0.5f,  

	-0.5f,  0.5f, -0.5f,  
	0.5f,  0.5f, -0.5f,  
	0.5f,  0.5f,  0.5f,  
	0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f, 
	-0.5f,  0.5f, -0.5f,  
};

float texCoord[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f
};

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
const unsigned int SCR_WIDTH = 1920 / 2;
const unsigned int SCR_HEIGHT = 1080 / 2;

float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// camera
Camera camera(vec3(0.0f, 0.0f, 3.0f));
clock_t deltaTime = 0;
double  frameRate = 30;
double  averageFrameTimeMilliseconds = 33.333;
bool vsync = true;
bool start = false;
int32 speed = 1;
vec3 LightPos = vec3(-1.0f, 5.0f, -0.3f);

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

void RenderThread()
{
	TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (OpenGL 3.3)", WindowStyle::Resize);
	window.initContext(3, 3);
	
	printf("- GPU Vendor    	: %s\n", glGetString(GL_VENDOR));
	printf("- Graphics      	: %s\n", glGetString(GL_RENDERER));
	printf("- Version       	: %s\n", glGetString(GL_VERSION));
	printf("- GLSL Version  	: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("- Hardware Threads 	: %d\n", std::thread::hardware_concurrency()); 

	Scene scene;

	typename RMI<ShaderProgram>::ID shaderID, texShaderID;
	
	ResourcesManager::GetGRM().Create<ShaderProgram>(shaderID, 
		Shader("res/Shader/texture.vs", ShaderType::VERTEX),
		Shader("res/Shader/texture.fs", ShaderType::FRAGMENT)
	);

	ResourcesManager::GetGRM().Create<ShaderProgram>(texShaderID, 
		Shader("res/Shader/cam.vs", ShaderType::VERTEX),
		Shader("res/Shader/cam.fs", ShaderType::FRAGMENT)
	);

	VBO vertexUBO(BufferTarget::UNIFORM_BUFFER);
	vertexUBO.FillData(NULL, 2 * sizeof(mat4));
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
		shader.AddUniform("viewPos");

		shader.AddUniform("light.ambient");
		shader.AddUniform("light.diffuse");
		shader.AddUniform("light.specular");
		shader.AddUniform("light.position");

		shader.AddUniform("material.ambient");
		shader.AddUniform("material.diffuse");
		shader.AddUniform("material.specular");
		shader.AddUniform("material.alpha");
		shader.AddUniform("material.shininess");
		shader.AddUniform("material.diffuse_tex");
		shader.AddUniform("material.specular_tex");

		shader.SetVec3("light.ambient", 0.09f, 0.09f, 0.09f);
		shader.SetVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
		shader.SetVec3("light.specular", 0.1f, 0.1f, 0.1f);
		shader.SetVec3("light.position", LightPos);
		shader.SetFloat("material.alpha", 1.f);
		shader.SetFloat("material.shininess", 1.0f);
	}

	IsWindowOpen = true;
	std::thread prepareThread(PrepareThread, &scene, &window);

	clock_t fps = 0, avgfps = 0, maxfps = 0, minfps = 9999999;
	uint64 frames = 1;
	Event ev;
	printf("\n");

	ResourcesManager::GetGRM().GetState(RenderSettings::DEFAULT_STATE_HASH).ApplyStates();
	while (IsWindowOpen) {
		clock_t beginFrame = clock();

		if (window.getEvent(ev)) {
			HandleEvent(&scene, ev);
		}

		IsWindowOpen = window.isOpen();

		ClearColor({ 51.f, 76.5f, 76.5f, 255.f });
		Clear();
		
		mat4 view = scene.GetCurrentCamera()->GetViewMatrix();
		vertexUBO.SubFillData(scene.GetProjectionMatrix(), 0, sizeof(mat4));
		vertexUBO.SubFillData(&view, sizeof(mat4), sizeof(mat4));
		vertexUBO.Unbind();
		
		scene.Render();

		clock_t endFrame = clock();
		deltaTime += endFrame - beginFrame;		
		frames++;
	
		if(endFrame - beginFrame > 0)
       	 	fps = CLOCKS_PER_SEC / (endFrame - beginFrame);
		
		if (fps > maxfps)
			maxfps = fps;
		
		if (fps < minfps)
			minfps = fps;
		
		avgfps += fps;

		//printf("                                                   \r");
    	//printf("\033[1;31m[T1] Delta Time : %lf ms - FPS : %lu\r", clockToMilliseconds(endFrame - beginFrame), fps);
		//fflush(stdout);

		window.Present();
	}

	prepareThread.join();
	printf("\033[1;31m[T1] : MIN FPS = %u| MAX FPS = %u | AVG FPS = %u\n", (uint32) minfps, (uint32) maxfps, uint32(avgfps / frames));
	getchar();
}

void PrepareThread(Scene* scene, TRE::Window* window)
{
#if defined(OS_LINUX)
	Directory res_dir("res/");
#endif
	String mesh_file_path;
	StaticMesh deagle;
	{
		mesh_file_path.Clear();
		mesh_file_path = "res/obj/lowpoly/deagle.obj";
		// res_dir.SearchRecursive("deagle.obj", mesh_file_path);
		MeshLoader obj_loader(mesh_file_path.Buffer());
		obj_loader.GetMaterialLoader().GetMaterialFromName("Fire.001").GetRenderStates().blend_enabled = true;
		obj_loader.GetMaterialLoader().GetMaterialFromName("Fire.001").GetRenderStates().cull_enabled = false;
		ModelLoader loader(std::move(obj_loader.LoadAsOneObject()));
		loader.ProcessData(deagle);
		scene->AddMeshInstance(&deagle);
	}

	StaticMesh carrot_box;
	{
		mesh_file_path.Clear();
		mesh_file_path = "res/obj/lowpoly/carrot_box.obj";
		// res_dir.SearchRecursive("carrot_box.obj", mesh_file_path);
		MeshLoader obj_loader(mesh_file_path.Buffer());
		ModelLoader loader(std::move(obj_loader.LoadAsOneObject()));
		loader.ProcessData(carrot_box, 1);
		scene->AddMeshInstance(&carrot_box);

	}
	carrot_box.GetTransformationMatrix().translate(LightPos - vec3(-5, 0, 0));

	StaticMesh trees;
	{
		mesh_file_path.Clear();
		mesh_file_path = "res/obj/lowpoly/all_combined_smooth.obj";
		//res_dir.SearchRecursive("all_combined_smooth.obj", mesh_file_path);
		MeshLoader obj_loader(mesh_file_path.Buffer());
		ModelLoader loader(std::move(obj_loader.LoadAsOneObject()));
		loader.ProcessData(trees, 1);
		scene->AddMeshInstance(&trees);

	}
	trees.GetTransformationMatrix().translate(vec3(0, 0, 6));

	
	
	clock_t fps = 0, avgfps = 0, maxfps = 0, deltaTime = 0, minfps = 9999999;
	uint64 frames = 1;
	// Event ev;
	// printf("\n");
	while(IsWindowOpen){
		clock_t beginFrame = clock();
    
		auto& render_cmd = RenderManager::GetRenderer().GetRenderCommandBuffer();
		if(render_cmd.SwapCmdBuffer()){
			render_cmd.Clear(); // Clear write side
			scene->Submit(); // write on write
		}

		clock_t endFrame = clock();
		deltaTime += endFrame - beginFrame;
		frames++;

		// trees.GetTransformationMatrix().rotateY(clockToMilliseconds(endFrame - beginFrame) / 500);
		
		if(endFrame - beginFrame > 0)
       	 	fps = CLOCKS_PER_SEC / (endFrame - beginFrame);

		if (fps > maxfps)
			maxfps = fps;
		
		if (fps < minfps)
			minfps = fps;
		
		avgfps += fps;
    	// printf("\033[1;32m[T2] Delta Time : %lf ms - FPS : %lu\r", clockToMilliseconds(endFrame - beginFrame), fps);
		//fflush(stdout);
	}

	printf("\n\033[1;32m[T2] : MIN FPS = %u| MAX FPS = %u | AVG FPS = %u\n", (uint32) minfps, (uint32) maxfps, uint32(avgfps / frames));
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
	Color* pixels = new Color[SCR_WIDTH*SCR_HEIGHT];
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
			break;
		case Key::Down:
			speed -= 1;
			break;
		case Key::Space:
			start = !start;
			deltaTime = 0;
			break;
		case Key::Z:
			scene->GetCurrentCamera()->ProcessKeyboard(FORWARD, (float)clockToMilliseconds(deltaTime)/1000.f);
			break;
		case Key::S:
			scene->GetCurrentCamera()->ProcessKeyboard(BACKWARD, (float)clockToMilliseconds(deltaTime)/1000.f);
			break;
		case Key::Q:
			scene->GetCurrentCamera()->ProcessKeyboard(LEFT, (float)clockToMilliseconds(deltaTime)/1000.f);
			break;
		case Key::D:
			scene->GetCurrentCamera()->ProcessKeyboard(RIGHT, (float)clockToMilliseconds(deltaTime)/1000.f);
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
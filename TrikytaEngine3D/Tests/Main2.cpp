#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <RenderAPI/Shader/Shader.hpp>
#include <RenderAPI/VertexArray/VAO.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>
#include <RenderAPI/Texture/Texture.hpp>
#include <RenderAPI/General/GLContext.hpp>
#include <RenderAPI/RenderBuffer/RBO.hpp>
#include <RenderAPI/Framebuffer/FBO.hpp>

#include <Core/Context/Extensions.hpp>
#include <Core/Window/Window.hpp>
#include <iostream>
#include <chrono>
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Utils/Image.hpp>
#include <thread>
#include <future>
#include <Core/Context/Context.hpp>
#include "Camera.hpp"

#include <RenderEngine/Loader/MeshLoader.hpp>
#include <RenderEngine/RawModel/RawModel.hpp>
#include <RenderAPI/GlobalState/GLState.hpp>

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
float deltaTime = 0.f;

void handleEvent(const Event&);
void clip(const Window&);

int main()
{
	TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", WindowStyle::Resize);
	TRE::Context& gl = window.initContext(3, 2);
	Event ev;
	printf("- GPU Vendor    : %s\n", glGetString(GL_VENDOR));
	printf("- Graphics      : %s\n", glGetString(GL_RENDERER));
	printf("- Version       : %s\n", glGetString(GL_VERSION));
	printf("- GLSL Version  : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	MeshLoader carrot("res/obj/lowpoly/deagle.obj");
	Vector<RawModel<true>> carrotModel;
	carrot.ProcessData(&carrotModel);

	/*MeshLoader box("res/obj/lowpoly/cube.obj");
	Vector<RawModel<true>> boxModel;
	box.ProcessData(&boxModel);*/

	ShaderProgram ourShader({
		Shader("res/Shader/cam.vs", ShaderType::VERTEX),
		Shader("res/Shader/cam.fs", ShaderType::FRAGMENT)
	});

	mat4 projection = mat4::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	float timer = 0.f;
	float lastFrame = 0.f;

	Enable(Capability::DEPTH_TEST);
	Enable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	ourShader.Use();
	ourShader.SetVec3("light.ambient", 0.12f, 0.12f, 0.12f);
	ourShader.SetVec3("light.diffuse", 0.7f, 0.7f, 0.7f);
	ourShader.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
	ourShader.SetVec3("light.position", 15.f, 15.f, 15.f);
	ourShader.SetVec3("ViewPos", camera.Position);
	ourShader.SetInt("material.diffuse_tex", 0);
	ourShader.SetInt("material.specular_tex", 1);

	while (window.isOpen())
	{
		if (window.getEvent(ev)) {
			handleEvent(ev);
		}

		ClearColor({ 51.f, 76.5f, 76.5f, 255.f });
		Clear();

		timer += 0.0005f;
		deltaTime = timer - lastFrame;
		lastFrame = timer;
	
		mat4 view = camera.GetViewMatrix();
		for (const RawModel<true>& obj : carrotModel) {
			obj.Use(ourShader);
			uint8 i = 0;
			//for (unsigned int i = 0; i < 10; i++){
				// calculate the model matrix for each object and pass it to shader before drawing
				mat4 model;
				model.translate(cubePositions[i]);
				float angle = timer * 1.f;
				model.rotate(vec3(.0f, 1.0f, 0.f), angle);
				model.scale(vec3(0.9f, 0.9f, 0.9f));
				mat4 MVP = projection * view * model;
				ourShader.SetMat4("MVP", MVP);
				ourShader.SetMat4("model", model);
				//obj.Render();
				obj.Render(ourShader);
			//}
		}
		window.Present();
	}

	window.Present();
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

void handleEvent(const Event& e)
{
	if (e.Type == Event::TE_RESIZE) {
		glViewport(0, 0, e.Window.Width, e.Window.Height);
		printf("DETECTING RESIZE EVENT(%d, %d)\n", e.Window.Width, e.Window.Height);
	}
	else if (e.Type == Event::TE_KEY_DOWN) {
		if (e.Key.Code == Key::F11) {
			PrintScreenshot();
			printf("Taking screenshot...\n");
		}
		switch (e.Key.Code) {
		case Key::Z:
			camera.ProcessKeyboard(FORWARD, deltaTime);
			break;
		case Key::S:
			camera.ProcessKeyboard(BACKWARD, deltaTime);
			break;
		case Key::Q:
			camera.ProcessKeyboard(LEFT, deltaTime);
			break;
		case Key::D:
			camera.ProcessKeyboard(RIGHT, deltaTime);
			break;
		}
	}
	else if (e.Type == Event::TE_MOUSE_MOVE) {
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

void clip(const Window& win)
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

}
#include <Renderer/Shader/ShaderProgram.hpp>
#include <Renderer/Shader/Shader.hpp>
#include <Renderer/VertexArray/VAO.hpp>
#include <Renderer/VertexBuffer/VBO.hpp>
#include <Renderer/Texture/Texture.hpp>
#include <Renderer/General/GLContext.hpp>
#include <Renderer/RenderBuffer/RBO.hpp>
#include <Renderer/Framebuffer/FBO.hpp>

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

using namespace TRE;

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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
void Render(Window* window, const Texture& texture1, const ShaderProgram& ourShader, const VAO& vao);
void clip(const Window&);

int main()
{
	TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", WindowStyle::Resize);
	TRE::Context& gl = window.initContext(3, 2);
	Event ev;
	printf("%s\n", glGetString(GL_VERSION));

	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	ShaderProgram ourShader({
		Shader("Shader/cam.vs", ShaderType::VERTEX),
		Shader("Shader/cam.fs", ShaderType::FRAGMENT)
	});

	ShaderProgram screenShader({
	Shader("Shader/screen.vs", ShaderType::VERTEX),
	Shader("Shader/screen.fs", ShaderType::FRAGMENT)
		});
	VAO vao; //glGenVertexArrays(1, &VAO);
	vao.Use(); //glBindVertexArray(VAO);
	VBO vbo(BufferTarget::ARRAY_BUFFER); // glGenBuffers(1, &VBO);
	vbo.FillData(vertices); //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	vao.BindAttribute<DataType::FLOAT>(0, vbo, 3, 5, 0); // glEnableVertexAttribArray(0); && glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	vao.BindAttribute<DataType::FLOAT>(1, vbo, 2, 5, 3); //glEnableVertexAttribArray(1);  && glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)))
	
	Texture texture1 = Texture("img/box1.jpg", TexTarget::TEX2D, {
		{TexParam::TEX_WRAP_S , TexWrapping::REPEAT},
		{TexParam::TEX_WRAP_T, TexWrapping::REPEAT},
		{TexParam::TEX_MIN_FILTER, TexFilter::LINEAR},
		{TexParam::TEX_MAG_FILTER, TexFilter::LINEAR}
	});


	FBO fbo(FBOTarget::FBO);
	TextureSettings set;
	set.width = SCR_WIDTH;
	set.height = SCR_HEIGHT;
	set.datatype = DataType::UBYTE;
	set.format = TexFormat::RGB;
	set.internalFormat = TexInternalFormat::RGB;
	set.target = TexTarget::TEX2D;
	set.paramList = { {TexParam::TEX_MIN_FILTER, TexFilter::LINEAR}, {TexParam::TEX_MAG_FILTER, TexFilter::LINEAR} };
	Texture tex(set);
	fbo.AttachTexture(tex, FBOAttachement::COLOR_ATTACH);
	RBO rbo;
	rbo.SetStorage(SCR_WIDTH, SCR_HEIGHT, RBOInternal::DEPTH24_STENCIL8);
	rbo.AttachToFBO(fbo, FBOAttachement::DEPTH_STENCIL_ATTACHMENT);
	fbo.IsComplete();
	fbo.Unuse();

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	};

	VAO quadVAO;
	quadVAO.Use();
	VBO quadVBO(BufferTarget::ARRAY_BUFFER);
	quadVBO.FillData(quadVertices);
	quadVAO.BindAttribute<DataType::FLOAT>(0, quadVBO, 2, 4, 0);
	quadVAO.BindAttribute<DataType::FLOAT>(1, quadVBO, 2, 4, 2);

	screenShader.Use();
	screenShader.SetInt("screenTexture", 0);
	ourShader.Use();
	ourShader.SetInt("texture", 0);

	mat4 projection = mat4::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	//ourShader.SetMat4("projection", projection);
	//clip(window);
	//Enable(GL_MULTISAMPLE);
	//wglMakeCurrent(GetDC(window.window), NULL);
	//std::thread t = std::thread(Render, &window, texture1, ourShader, vao);
	//t.detach();
	float timer = 0.f;
	float lastFrame = 0.f;

	while (window.isOpen())
	{
		// input
		// -----
		if (window.getEvent(ev)) {
			handleEvent(ev);
		}
		timer += 0.0005f;
		deltaTime = timer - lastFrame;
		lastFrame = timer;

		// render
		// ------
		//printf("\rTIME = %f", timer);
		fbo.Use();
		Enable(Capability::DEPTH_TEST);
		ActivateTexture(0);
		texture1.Use();
		ClearColor({ 51.f, 76.5f, 76.5f, 255.f });
		Clear();

		ourShader.Use(); // activate shader
		mat4 view = camera.GetViewMatrix();
		//ourShader.SetMat4("view", view);

		// render boxes
		vao.Use();
		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			mat4 model;
			model.translate(cubePositions[i]);
			float angle = 20.0f * i;
			model.rotate(vec3(1.0f, 0.3f, 0.5f), rad(angle));
			mat4 MVP = projection * view * model;
			ourShader.SetMat4("MVP", MVP);
			DrawArrays(Primitive::TRIANGLES, 0, 36);
		}
		fbo.Unuse();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		Disable(Capability::DEPTH_TEST);
		ClearColor({255.f, 255.f, 255.f, 255.f });
		Clear(Buffer::COLOR);
		screenShader.Use();
		quadVAO.Use();
		ActivateTexture(0);
		tex.Use();
		DrawArrays(Primitive::TRIANGLES, 0, 6);
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
	}else if (e.Type == Event::TE_KEY_DOWN) {
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
	}else if (e.Type == Event::TE_MOUSE_MOVE) {
		float xpos = (float)e.Mouse.X;
		float ypos = (float)e.Mouse.Y;
		if (firstMouse){
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


void Render(Window* window, const Texture& texture1, const ShaderProgram& ourShader, const VAO& vao)
{
	float timer = 0.f;
	wglMakeCurrent(GetDC(window->window), window->context->context);
	Event ev;
	while (window->isOpen()) {
		if (window->getEvent(ev)) {
			handleEvent(ev);
		}

		// render
		// ------
		printf("\rTIME = %f", timer);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		texture1.Use();
		// activate shader
		ourShader.Use();
		// camera/view transformation
		mat4 view; // make sure to initialize matrix to identity matrix first
		float radius = 10.0f;
		float camX = sin(timer) * radius;
		float camZ = cos(timer) * radius;
		view = mat4::look_at(vec3(camX, 0.0f, camZ), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		ourShader.SetMat4("view", view);

		// render boxes
		vao.Use();
		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			mat4 model;
			model.translate(cubePositions[i]);
			float angle = 20.0f * i;
			model.rotate(vec3(1.0f, 0.3f, 0.5f), rad(angle));
			ourShader.SetMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		timer += 0.0005f;
		window->Present();
	}
}

void clip(const Window& win)
{
	int a;
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
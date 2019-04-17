#include <Renderer/Shader/ShaderProgram.hpp>
#include <Renderer/Shader/Shader.hpp>
#include <Renderer/VertexArray/VAO.hpp>
#include <Renderer/VertexBuffer/VBO.hpp>
#include <Renderer/Texture/Texture.hpp>
#include <Core/Context/Extensions.hpp>
#include <Core/Window/Window.hpp>
#include <iostream>
#include <chrono>
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Utils/Image.hpp>
#include <thread>
#include <future>

// settings
const unsigned int SCR_WIDTH = 1920 / 2;
const unsigned int SCR_HEIGHT = 1080 / 2;

using namespace TRE;

void handleEvent(const Event&);

int main()
{
	TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", WindowStyle::Resize);
	TRE::Context& gl = window.initContext(3, 2);
	Event ev;
	printf("%s\n", glGetString(GL_VERSION));

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

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

	// build and compile our shader zprogram
	// ------------------------------------
	ShaderProgram ourShader({
		Shader("Shader/cam.vs", ShaderType::VERTEX),
		Shader("Shader/cam.fs", ShaderType::FRAGMENT)
		});
	VAO vao; //glGenVertexArrays(1, &VAO);
	vao.Bind(); //glBindVertexArray(VAO);
	VBO vbo(BufferTarget::ARRAY_BUFFER); // glGenBuffers(1, &VBO);
	vbo.FillData(vertices); //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	vao.BindAttribute<float>(0, vbo, DataType::FLOAT, 3, 5, 0); // glEnableVertexAttribArray(0); && glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	vao.BindAttribute<float>(1, vbo, DataType::FLOAT, 2, 5, 3); //glEnableVertexAttribArray(1);  && glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	Texture texture1 = Texture("img/box1.jpg", TexTarget::TEX2D, {
		{TexParam::TEX_WRAP_S , TexWrapping::REPEAT},
		{TexParam::TEX_WRAP_T, TexWrapping::REPEAT},
		{TexParam::TEX_MIN_FILTER, TexFilter::LINEAR},
		{TexParam::TEX_MAG_FILTER, TexFilter::LINEAR}
	});

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.Use();
	ourShader.SetInt("texture", 0);

	// pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
	// -----------------------------------------------------------------------------------------------------------
	mat4 projection = mat4::perspective(rad(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	ourShader.SetMat4("projection", projection);

	// render loop
	// -----------
	float timer = 0.f;
	glEnable(GL_MULTISAMPLE);
	while (window.isOpen())
	{
		// input
		// -----
		if (window.getEvent(ev)) {
			handleEvent(ev);
		}

		//printf("\rTIME = %f", timer);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
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
		glBindVertexArray(vao);
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
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
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
	}
}

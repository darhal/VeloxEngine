#include <Core/Misc/Defines/Debug.hpp>
#include "Extensions.hpp"
#include "Context.hpp"

#ifdef OS_WINDOWS

TRE::Context::Context(uint8 color, uint8 depth, uint8 stencil, uint antialias, HDC dc)
{
	// Create dummy window
	HWND dummyWindow = CreateWindowA("STATIC", "", WS_POPUP | WS_DISABLED, 0, 0, 1, 1, NULL, NULL, GetModuleHandle(NULL), NULL);
	HDC dummyDC = GetDC(dummyWindow);

	// Set dummy pixel format
	PIXELFORMATDESCRIPTOR dummyFormatDescriptor =
	{
		sizeof(dummyFormatDescriptor), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA, color, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, depth, stencil, 0, PFD_MAIN_PLANE,
		0, 0, 0, 0
	};

	int dummyPixelFormat = ChoosePixelFormat(dummyDC, &dummyFormatDescriptor);
	SetPixelFormat(dummyDC, dummyPixelFormat, &dummyFormatDescriptor);

	// Create dummy context
	HGLRC dummyContext = wglCreateContext(dummyDC);
	wglMakeCurrent(dummyDC, dummyContext);

	// Load OpenGL extensions
	LoadGL();

	// Choose final pixel format
	const int pixelAttribs[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, (int)color,
		WGL_DEPTH_BITS_ARB, (int)depth,
		WGL_STENCIL_BITS_ARB, (int)stencil,
		WGL_SAMPLE_BUFFERS_ARB, antialias > 1 ? GL_TRUE : GL_FALSE,
		WGL_SAMPLES_ARB, antialias > 1 ? antialias : 0,
		0
	};

	int pixelFormat;
	uint formatCount;
	wglChoosePixelFormatARB(dc, pixelAttribs, NULL, 1, &pixelFormat, &formatCount);
	ASSERTF(!(formatCount == 0), "No pixel format could be found with support for the specified buffer depths and anti-aliasing.");
	SetPixelFormat(dc, pixelFormat, &dummyFormatDescriptor);

	// Create OpenGL 3.2 context		
	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 2,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	context = wglCreateContextAttribsARB(dc, NULL, attribs);

	// Clean up
	wglMakeCurrent(dc, NULL);
	wglDeleteContext(dummyContext);
	wglMakeCurrent(dc, context);
	DestroyWindow(dummyWindow);

	this->dc = dc;
	this->owned = true;

	glGetIntegerv(GL_VIEWPORT, (GLint*)&defaultViewport);

	QueryPerformanceCounter(&timeOffset);
}

TRE::Context::~Context()
{
	if (!owned) return;

	wglMakeCurrent(dc, NULL);
	wglDeleteContext(context);
}

void TRE::Context::Activate()
{
	if (owned && wglGetCurrentContext() != context) wglMakeCurrent(dc, context);
}

void TRE::Context::SetVerticalSync(bool enabled)
{
	wglSwapIntervalEXT(enabled ? 1 : 0);
}

float TRE::Context::Time()
{
	LARGE_INTEGER time, freq;
	QueryPerformanceCounter(&time);
	QueryPerformanceFrequency(&freq);

	return ((time.QuadPart - timeOffset.QuadPart) * 1000 / freq.QuadPart) / 1000.0f;
}

TRE::Context::Context()
{
	// Prepare class for using unowned context (i.e. created by external party)
	LoadGL();
	owned = false;

	glGetIntegerv(GL_VIEWPORT, (GLint*)&defaultViewport);

	QueryPerformanceCounter(&timeOffset);
}


#endif
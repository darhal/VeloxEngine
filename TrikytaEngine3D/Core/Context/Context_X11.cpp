#include <Core/Misc/Defines/Debug.hpp>
#include "Extensions.hpp"
#include "Context.hpp"

#ifdef OS_LINUX

#pragma GCC diagnostic ignored "-Wunused-parameter"


TRE_NS_START

// Stub error handler for surpressing undesired Xlib errors
typedef int(*XERRORHANDLER) (Display*, XErrorEvent*);
int XErrorSurpressor(Display* display, XErrorEvent* ev)
{
	return 0;
}

Context::Context(uint8 vmajor, uint8 vminor, uint8 color, uint8 depth, uint8 stencil, uint antialias, Display* display, int screen, ::Window window)
{
	//LoadGL();
	// Load OpenGL extensions
	// Choose an appropriate config
	const int pixelAttribs[] = {
		GLX_X_RENDERABLE, GL_TRUE,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_DOUBLEBUFFER, GL_TRUE,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_BUFFER_SIZE, color,
		GLX_DEPTH_SIZE, depth,
		GLX_STENCIL_SIZE, stencil,
		GLX_SAMPLE_BUFFERS, antialias > 1 ? GL_TRUE : GL_FALSE,
		GLX_SAMPLES, antialias > 1 ? (int)antialias : 0,
		0
	};

	glXCreateContextAttribsARB =(GLXCREATECONTEXTATTRIBSARB)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

	int configCount;
	GLXFBConfig* configs = glXChooseFBConfig(display, screen, pixelAttribs, &configCount);
	ASSERTF(!(configCount == 0), "PixelFormatException");
	GLXFBConfig config = configs[0];
	XFree(configs);

	// Create OpenGL 3.2 context
	int attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, vmajor,
		GLX_CONTEXT_MINOR_VERSION_ARB, vminor,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};
	XERRORHANDLER oldHandler = XSetErrorHandler(&XErrorSurpressor);
	context = glXCreateContextAttribsARB(display, config, NULL, GL_TRUE, attribs);
	ASSERTF(!(!context), "VersionException");
	XSetErrorHandler(oldHandler);

	// Activate context
	glXMakeCurrent(display, window, context);
	
	LoadGL();
	
	this->display = display;
	this->window = window;
	this->owned = true;
	glGetIntegerv(GL_VIEWPORT, (GLint*)&defaultViewport);
	gettimeofday(&timeOffset, NULL);
}

Context::~Context()
{
	if (!owned) return;

	glXMakeCurrent(display, 0, NULL);
	glXDestroyContext(display, context);
}

void Context::Activate()
{
	if (owned && glXGetCurrentContext() != context) glXMakeCurrent(display, window, context);
}

void Context::SetVerticalSync(bool enabled)
{
	glXSwapIntervalSGI(enabled ? 1 : 0);
}

float Context::Time()
{
	timeval time;
	gettimeofday(&time, NULL);

	return time.tv_sec - timeOffset.tv_sec + (time.tv_usec - timeOffset.tv_usec) / 1000000.0f;
}

Context::Context()
{
	// Prepare class for using unowned context (i.e. created by external party)
	LoadGL();
	owned = false;

	glGetIntegerv(GL_VIEWPORT, (GLint*)&defaultViewport);

	gettimeofday(&timeOffset, NULL);
}

TRE_NS_END

#endif

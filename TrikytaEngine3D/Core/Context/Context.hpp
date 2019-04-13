#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/PlatformInclude.hpp>

namespace TRE
{
class Window;
class Context
{
public:
	void Activate();

	void SetVerticalSync(bool enabled);

	float Time();

	static Context UseExistingContext();

	~Context();

private:
	friend class Window;

	Context();

	bool owned;
	int defaultViewport[4];

#if defined(OS_WINDOWS)
	Context(uint8 vmajor, uint8 vminor, uint8 color, uint8 depth, uint8 stencil, uint32 antialias, HDC dc);

	HDC dc;
	HGLRC context;

	LARGE_INTEGER timeOffset;
#elif defined(OS_LINUX)
	Context(uint8 vmajor, uint8 vminor, uint8 color, uint8 depth, uint8 stencil, uint antialias, Display* display, int screen, ::Window window);

	GLXWindow glxWindow;
	GLXContext context;
	Display* display;
	::Window window;

	timeval timeOffset;
#endif
};

}
#pragma once
#include "Platform.hpp"

#if defined(OS_WINDOWS)
	#include <Windows.h>
	#include <WindowsX.h>
	#include <GL/GL.h>
#elif defined(OS_LINUX)
	#include <X11/Xlib.h>
	#include <GL/gl.h>
	#include <GL/glx.h>
	#include <sys/time.h>
#endif
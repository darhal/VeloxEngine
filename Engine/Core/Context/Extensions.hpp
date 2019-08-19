#pragma once

#include <Core/Misc/Defines/Platform.hpp>
#include <Core/Misc/Defines/Common.hpp>

/*
	Extension loader
*/

#if !defined(__gl_h_)

#define __gl_h_

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#include <windows.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY APIENTRY
#endif

#ifdef __cplusplus
extern "C" {
#endif

	struct GLversionStruct {
		int major;
		int minor;
	};

	typedef void* (*loadproc)(const char *name);
#ifndef GLAPI
# if defined(_GLAPI_EXPORT)
#  if defined(_WIN32) || defined(__CYGWIN__)
#   if defined(_GLAPI_EXPORT_BUILD)
#    if defined(__GNUC__)
#     define GLAPI __attribute__ ((dllexport)) extern
#    else
#     define GLAPI __declspec(dllexport) extern
#    endif
#   else
#    if defined(__GNUC__)
#     define GLAPI __attribute__ ((dllimport)) extern
#    else
#     define GLAPI __declspec(dllimport) extern
#    endif
#   endif
#  elif defined(__GNUC__) && defined(_GLAPI_EXPORT_BUILD)
#   define GLAPI __attribute__ ((visibility ("default"))) extern
#  else
#   define GLAPI extern
#  endif
# else
#  define GLAPI extern
# endif
#endif

	GLAPI struct GLversionStruct GLVersion;

	GLAPI int LoadGL(void);
	GLAPI int LoadGLLoader(loadproc);

#include <stddef.h>
#ifndef GLEXT_64_TYPES_DEFINED
	/* This code block is duplicated in glxext.h, so must be protected */
#define GLEXT_64_TYPES_DEFINED
/* Define int32_t, int64_t, and uint64_t types for UST/MSC */
/* (as used in the GL_EXT_timer_query extension). */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <inttypes.h>
#elif defined(__sun__) || defined(__digital__)
#include <inttypes.h>
#if defined(__STDC__)
#if defined(__arch64__) || defined(_LP64)
	typedef long int int64_t;
	typedef unsigned long int uint64_t;
#else
	typedef long long int int64_t;
	typedef unsigned long long int uint64_t;
#endif /* __arch64__ */
#endif /* __STDC__ */
#elif defined( __VMS ) || defined(__sgi)
#include <inttypes.h>
#elif defined(__SCO__) || defined(__USLC__)
#include <stdint.h>
#elif defined(__UNIXOS2__) || defined(__SOL64__)
	typedef long int int32_t;
	typedef long long int int64_t;
	typedef unsigned long long int uint64_t;
#elif defined(_WIN32) && defined(__GNUC__)
#include <stdint.h>
#elif defined(_WIN32)
	typedef __int32 int32_t;
	typedef __int64 int64_t;
	typedef unsigned __int64 uint64_t;
#else
/* Fallback if nothing above works */
#include <inttypes.h>
#endif
#endif
	typedef unsigned int GLenum;
	typedef unsigned char GLboolean;
	typedef unsigned int GLbitfield;
	typedef void GLvoid;
	typedef signed char GLbyte;
	typedef short GLshort;
	typedef int GLint;
	typedef int GLclampx;
	typedef unsigned char GLubyte;
	typedef unsigned short GLushort;
	typedef unsigned int GLuint;
	typedef int GLsizei;
	typedef float GLfloat;
	typedef float GLclampf;
	typedef double GLdouble;
	typedef double GLclampd;
	typedef void *GLeglClientBufferEXT;
	typedef void *GLeglImageOES;
	typedef char GLchar;
	typedef char GLcharARB;
#ifdef __APPLE__
	typedef void *GLhandleARB;
#else
	typedef unsigned int GLhandleARB;
#endif
	typedef unsigned short GLhalfARB;
	typedef unsigned short GLhalf;
	typedef GLint GLfixed;
	typedef intptr_t GLintptr;
	typedef ssize_type GLsizeiptr;
	typedef int64_t GLint64;
	typedef uint64_t GLuint64;
#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ > 1060)
	typedef long GLintptrARB;
#else
	typedef ptrdiff_t GLintptrARB;
#endif
#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ > 1060)
	typedef long GLsizeiptrARB;
#else
	typedef ptrdiff_t GLsizeiptrARB;
#endif
	typedef int64_t GLint64EXT;
	typedef uint64_t GLuint64EXT;
	typedef struct __GLsync *GLsync;
	struct _cl_context;
	struct _cl_event;
	typedef void (APIENTRY *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
	typedef void (APIENTRY *GLDEBUGPROCARB)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
	typedef void (APIENTRY *GLDEBUGPROCKHR)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
	typedef void (APIENTRY *GLDEBUGPROCAMD)(GLuint id, GLenum category, GLenum severity, GLsizei length, const GLchar *message, void *userParam);
	typedef unsigned short GLhalfNV;
	typedef GLintptr GLvdpauSurfaceNV;
	typedef void (APIENTRY *GLVULKANPROCNV)(void);

#include "GLDefines.hpp"

#ifndef GL_VERSION_1_0
#define GL_VERSION_1_0 1
	GLAPI int _GL_VERSION_1_0;
	typedef void (APIENTRYP PFNGLCULLFACEPROC)(GLenum mode);
	GLAPI PFNGLCULLFACEPROC _glCullFace;
#define glCullFace _glCullFace
	typedef void (APIENTRYP PFNGLFRONTFACEPROC)(GLenum mode);
	GLAPI PFNGLFRONTFACEPROC _glFrontFace;
#define glFrontFace _glFrontFace
	typedef void (APIENTRYP PFNGLHINTPROC)(GLenum target, GLenum mode);
	GLAPI PFNGLHINTPROC _glHint;
#define glHint _glHint
	typedef void (APIENTRYP PFNGLLINEWIDTHPROC)(GLfloat width);
	GLAPI PFNGLLINEWIDTHPROC _glLineWidth;
#define glLineWidth _glLineWidth
	typedef void (APIENTRYP PFNGLPOINTSIZEPROC)(GLfloat size);
	GLAPI PFNGLPOINTSIZEPROC _glPointSize;
#define glPointSize _glPointSize
	typedef void (APIENTRYP PFNGLPOLYGONMODEPROC)(GLenum face, GLenum mode);
	GLAPI PFNGLPOLYGONMODEPROC _glPolygonMode;
#define glPolygonMode _glPolygonMode
	typedef void (APIENTRYP PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
	GLAPI PFNGLSCISSORPROC _glScissor;
#define glScissor _glScissor
	typedef void (APIENTRYP PFNGLTEXPARAMETERFPROC)(GLenum target, GLenum pname, GLfloat param);
	GLAPI PFNGLTEXPARAMETERFPROC _glTexParameterf;
#define glTexParameterf _glTexParameterf
	typedef void (APIENTRYP PFNGLTEXPARAMETERFVPROC)(GLenum target, GLenum pname, const GLfloat *params);
	GLAPI PFNGLTEXPARAMETERFVPROC _glTexParameterfv;
#define glTexParameterfv _glTexParameterfv
	typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
	GLAPI PFNGLTEXPARAMETERIPROC _glTexParameteri;
#define glTexParameteri _glTexParameteri
	typedef void (APIENTRYP PFNGLTEXPARAMETERIVPROC)(GLenum target, GLenum pname, const GLint *params);
	GLAPI PFNGLTEXPARAMETERIVPROC _glTexParameteriv;
#define glTexParameteriv _glTexParameteriv
	typedef void (APIENTRYP PFNGLTEXIMAGE1DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
	GLAPI PFNGLTEXIMAGE1DPROC _glTexImage1D;
#define glTexImage1D _glTexImage1D
	typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
	GLAPI PFNGLTEXIMAGE2DPROC _glTexImage2D;
#define glTexImage2D _glTexImage2D
	typedef void (APIENTRYP PFNGLDRAWBUFFERPROC)(GLenum buf);
	GLAPI PFNGLDRAWBUFFERPROC _glDrawBuffer;
#define glDrawBuffer _glDrawBuffer
	typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
	GLAPI PFNGLCLEARPROC _glClear;
#define glClear _glClear
	typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	GLAPI PFNGLCLEARCOLORPROC _glClearColor;
#define glClearColor _glClearColor
	typedef void (APIENTRYP PFNGLCLEARSTENCILPROC)(GLint s);
	GLAPI PFNGLCLEARSTENCILPROC _glClearStencil;
#define glClearStencil _glClearStencil
	typedef void (APIENTRYP PFNGLCLEARDEPTHPROC)(GLdouble depth);
	GLAPI PFNGLCLEARDEPTHPROC _glClearDepth;
#define glClearDepth _glClearDepth
	typedef void (APIENTRYP PFNGLSTENCILMASKPROC)(GLuint mask);
	GLAPI PFNGLSTENCILMASKPROC _glStencilMask;
#define glStencilMask _glStencilMask
	typedef void (APIENTRYP PFNGLCOLORMASKPROC)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
	GLAPI PFNGLCOLORMASKPROC _glColorMask;
#define glColorMask _glColorMask
	typedef void (APIENTRYP PFNGLDEPTHMASKPROC)(GLboolean flag);
	GLAPI PFNGLDEPTHMASKPROC _glDepthMask;
#define glDepthMask _glDepthMask
	typedef void (APIENTRYP PFNGLDISABLEPROC)(GLenum cap);
	GLAPI PFNGLDISABLEPROC _glDisable;
#define glDisable _glDisable
	typedef void (APIENTRYP PFNGLENABLEPROC)(GLenum cap);
	GLAPI PFNGLENABLEPROC _glEnable;
#define glEnable _glEnable
	typedef void (APIENTRYP PFNGLFINISHPROC)(void);
	GLAPI PFNGLFINISHPROC _glFinish;
#define glFinish _glFinish
	typedef void (APIENTRYP PFNGLFLUSHPROC)(void);
	GLAPI PFNGLFLUSHPROC _glFlush;
#define glFlush _glFlush
	typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
	GLAPI PFNGLBLENDFUNCPROC _glBlendFunc;
#define glBlendFunc _glBlendFunc
	typedef void (APIENTRYP PFNGLLOGICOPPROC)(GLenum opcode);
	GLAPI PFNGLLOGICOPPROC _glLogicOp;
#define glLogicOp _glLogicOp
	typedef void (APIENTRYP PFNGLSTENCILFUNCPROC)(GLenum func, GLint ref, GLuint mask);
	GLAPI PFNGLSTENCILFUNCPROC _glStencilFunc;
#define glStencilFunc _glStencilFunc
	typedef void (APIENTRYP PFNGLSTENCILOPPROC)(GLenum fail, GLenum zfail, GLenum zpass);
	GLAPI PFNGLSTENCILOPPROC _glStencilOp;
#define glStencilOp _glStencilOp
	typedef void (APIENTRYP PFNGLDEPTHFUNCPROC)(GLenum func);
	GLAPI PFNGLDEPTHFUNCPROC _glDepthFunc;
#define glDepthFunc _glDepthFunc
	typedef void (APIENTRYP PFNGLPIXELSTOREFPROC)(GLenum pname, GLfloat param);
	GLAPI PFNGLPIXELSTOREFPROC _glPixelStoref;
#define glPixelStoref _glPixelStoref
	typedef void (APIENTRYP PFNGLPIXELSTOREIPROC)(GLenum pname, GLint param);
	GLAPI PFNGLPIXELSTOREIPROC _glPixelStorei;
#define glPixelStorei _glPixelStorei
	typedef void (APIENTRYP PFNGLREADBUFFERPROC)(GLenum src);
	GLAPI PFNGLREADBUFFERPROC _glReadBuffer;
#define glReadBuffer _glReadBuffer
	typedef void (APIENTRYP PFNGLREADPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
	GLAPI PFNGLREADPIXELSPROC _glReadPixels;
#define glReadPixels _glReadPixels
	typedef void (APIENTRYP PFNGLGETBOOLEANVPROC)(GLenum pname, GLboolean *data);
	GLAPI PFNGLGETBOOLEANVPROC _glGetBooleanv;
#define glGetBooleanv _glGetBooleanv
	typedef void (APIENTRYP PFNGLGETDOUBLEVPROC)(GLenum pname, GLdouble *data);
	GLAPI PFNGLGETDOUBLEVPROC _glGetDoublev;
#define glGetDoublev _glGetDoublev
	typedef GLenum(APIENTRYP PFNGLGETERRORPROC)(void);
	GLAPI PFNGLGETERRORPROC _glGetError;
#define glGetError _glGetError
	typedef void (APIENTRYP PFNGLGETFLOATVPROC)(GLenum pname, GLfloat *data);
	GLAPI PFNGLGETFLOATVPROC _glGetFloatv;
#define glGetFloatv _glGetFloatv
	typedef void (APIENTRYP PFNGLGETINTEGERVPROC)(GLenum pname, GLint *data);
	GLAPI PFNGLGETINTEGERVPROC _glGetIntegerv;
#define glGetIntegerv _glGetIntegerv
	typedef const GLubyte * (APIENTRYP PFNGLGETSTRINGPROC)(GLenum name);
	GLAPI PFNGLGETSTRINGPROC _glGetString;
#define glGetString _glGetString
	typedef void (APIENTRYP PFNGLGETTEXIMAGEPROC)(GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
	GLAPI PFNGLGETTEXIMAGEPROC _glGetTexImage;
#define glGetTexImage _glGetTexImage
	typedef void (APIENTRYP PFNGLGETTEXPARAMETERFVPROC)(GLenum target, GLenum pname, GLfloat *params);
	GLAPI PFNGLGETTEXPARAMETERFVPROC _glGetTexParameterfv;
#define glGetTexParameterfv _glGetTexParameterfv
	typedef void (APIENTRYP PFNGLGETTEXPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
	GLAPI PFNGLGETTEXPARAMETERIVPROC _glGetTexParameteriv;
#define glGetTexParameteriv _glGetTexParameteriv
	typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERFVPROC)(GLenum target, GLint level, GLenum pname, GLfloat *params);
	GLAPI PFNGLGETTEXLEVELPARAMETERFVPROC _glGetTexLevelParameterfv;
#define glGetTexLevelParameterfv _glGetTexLevelParameterfv
	typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERIVPROC)(GLenum target, GLint level, GLenum pname, GLint *params);
	GLAPI PFNGLGETTEXLEVELPARAMETERIVPROC _glGetTexLevelParameteriv;
#define glGetTexLevelParameteriv _glGetTexLevelParameteriv
	typedef GLboolean(APIENTRYP PFNGLISENABLEDPROC)(GLenum cap);
	GLAPI PFNGLISENABLEDPROC _glIsEnabled;
#define glIsEnabled _glIsEnabled
	typedef void (APIENTRYP PFNGLDEPTHRANGEPROC)(GLdouble n, GLdouble f);
	GLAPI PFNGLDEPTHRANGEPROC _glDepthRange;
#define glDepthRange _glDepthRange
	typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
	GLAPI PFNGLVIEWPORTPROC _glViewport;
#define glViewport _glViewport
	typedef void (APIENTRYP PFNGLNEWLISTPROC)(GLuint list, GLenum mode);
	GLAPI PFNGLNEWLISTPROC _glNewList;
#define glNewList _glNewList
	typedef void (APIENTRYP PFNGLENDLISTPROC)(void);
	GLAPI PFNGLENDLISTPROC _glEndList;
#define glEndList _glEndList
	typedef void (APIENTRYP PFNGLCALLLISTPROC)(GLuint list);
	GLAPI PFNGLCALLLISTPROC _glCallList;
#define glCallList _glCallList
	typedef void (APIENTRYP PFNGLCALLLISTSPROC)(GLsizei n, GLenum type, const void *lists);
	GLAPI PFNGLCALLLISTSPROC _glCallLists;
#define glCallLists _glCallLists
	typedef void (APIENTRYP PFNGLDELETELISTSPROC)(GLuint list, GLsizei range);
	GLAPI PFNGLDELETELISTSPROC _glDeleteLists;
#define glDeleteLists _glDeleteLists
	typedef GLuint(APIENTRYP PFNGLGENLISTSPROC)(GLsizei range);
	GLAPI PFNGLGENLISTSPROC _glGenLists;
#define glGenLists _glGenLists
	typedef void (APIENTRYP PFNGLLISTBASEPROC)(GLuint base);
	GLAPI PFNGLLISTBASEPROC _glListBase;
#define glListBase _glListBase
	typedef void (APIENTRYP PFNGLBEGINPROC)(GLenum mode);
	GLAPI PFNGLBEGINPROC _glBegin;
#define glBegin _glBegin
	typedef void (APIENTRYP PFNGLBITMAPPROC)(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
	GLAPI PFNGLBITMAPPROC _glBitmap;
#define glBitmap _glBitmap
	typedef void (APIENTRYP PFNGLCOLOR3BPROC)(GLbyte red, GLbyte green, GLbyte blue);
	GLAPI PFNGLCOLOR3BPROC _glColor3b;
#define glColor3b _glColor3b
	typedef void (APIENTRYP PFNGLCOLOR3BVPROC)(const GLbyte *v);
	GLAPI PFNGLCOLOR3BVPROC _glColor3bv;
#define glColor3bv _glColor3bv
	typedef void (APIENTRYP PFNGLCOLOR3DPROC)(GLdouble red, GLdouble green, GLdouble blue);
	GLAPI PFNGLCOLOR3DPROC _glColor3d;
#define glColor3d _glColor3d
	typedef void (APIENTRYP PFNGLCOLOR3DVPROC)(const GLdouble *v);
	GLAPI PFNGLCOLOR3DVPROC _glColor3dv;
#define glColor3dv _glColor3dv
	typedef void (APIENTRYP PFNGLCOLOR3FPROC)(GLfloat red, GLfloat green, GLfloat blue);
	GLAPI PFNGLCOLOR3FPROC _glColor3f;
#define glColor3f _glColor3f
	typedef void (APIENTRYP PFNGLCOLOR3FVPROC)(const GLfloat *v);
	GLAPI PFNGLCOLOR3FVPROC _glColor3fv;
#define glColor3fv _glColor3fv
	typedef void (APIENTRYP PFNGLCOLOR3IPROC)(GLint red, GLint green, GLint blue);
	GLAPI PFNGLCOLOR3IPROC _glColor3i;
#define glColor3i _glColor3i
	typedef void (APIENTRYP PFNGLCOLOR3IVPROC)(const GLint *v);
	GLAPI PFNGLCOLOR3IVPROC _glColor3iv;
#define glColor3iv _glColor3iv
	typedef void (APIENTRYP PFNGLCOLOR3SPROC)(GLshort red, GLshort green, GLshort blue);
	GLAPI PFNGLCOLOR3SPROC _glColor3s;
#define glColor3s _glColor3s
	typedef void (APIENTRYP PFNGLCOLOR3SVPROC)(const GLshort *v);
	GLAPI PFNGLCOLOR3SVPROC _glColor3sv;
#define glColor3sv _glColor3sv
	typedef void (APIENTRYP PFNGLCOLOR3UBPROC)(GLubyte red, GLubyte green, GLubyte blue);
	GLAPI PFNGLCOLOR3UBPROC _glColor3ub;
#define glColor3ub _glColor3ub
	typedef void (APIENTRYP PFNGLCOLOR3UBVPROC)(const GLubyte *v);
	GLAPI PFNGLCOLOR3UBVPROC _glColor3ubv;
#define glColor3ubv _glColor3ubv
	typedef void (APIENTRYP PFNGLCOLOR3UIPROC)(GLuint red, GLuint green, GLuint blue);
	GLAPI PFNGLCOLOR3UIPROC _glColor3ui;
#define glColor3ui _glColor3ui
	typedef void (APIENTRYP PFNGLCOLOR3UIVPROC)(const GLuint *v);
	GLAPI PFNGLCOLOR3UIVPROC _glColor3uiv;
#define glColor3uiv _glColor3uiv
	typedef void (APIENTRYP PFNGLCOLOR3USPROC)(GLushort red, GLushort green, GLushort blue);
	GLAPI PFNGLCOLOR3USPROC _glColor3us;
#define glColor3us _glColor3us
	typedef void (APIENTRYP PFNGLCOLOR3USVPROC)(const GLushort *v);
	GLAPI PFNGLCOLOR3USVPROC _glColor3usv;
#define glColor3usv _glColor3usv
	typedef void (APIENTRYP PFNGLCOLOR4BPROC)(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
	GLAPI PFNGLCOLOR4BPROC _glColor4b;
#define glColor4b _glColor4b
	typedef void (APIENTRYP PFNGLCOLOR4BVPROC)(const GLbyte *v);
	GLAPI PFNGLCOLOR4BVPROC _glColor4bv;
#define glColor4bv _glColor4bv
	typedef void (APIENTRYP PFNGLCOLOR4DPROC)(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
	GLAPI PFNGLCOLOR4DPROC _glColor4d;
#define glColor4d _glColor4d
	typedef void (APIENTRYP PFNGLCOLOR4DVPROC)(const GLdouble *v);
	GLAPI PFNGLCOLOR4DVPROC _glColor4dv;
#define glColor4dv _glColor4dv
	typedef void (APIENTRYP PFNGLCOLOR4FPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	GLAPI PFNGLCOLOR4FPROC _glColor4f;
#define glColor4f _glColor4f
	typedef void (APIENTRYP PFNGLCOLOR4FVPROC)(const GLfloat *v);
	GLAPI PFNGLCOLOR4FVPROC _glColor4fv;
#define glColor4fv _glColor4fv
	typedef void (APIENTRYP PFNGLCOLOR4IPROC)(GLint red, GLint green, GLint blue, GLint alpha);
	GLAPI PFNGLCOLOR4IPROC _glColor4i;
#define glColor4i _glColor4i
	typedef void (APIENTRYP PFNGLCOLOR4IVPROC)(const GLint *v);
	GLAPI PFNGLCOLOR4IVPROC _glColor4iv;
#define glColor4iv _glColor4iv
	typedef void (APIENTRYP PFNGLCOLOR4SPROC)(GLshort red, GLshort green, GLshort blue, GLshort alpha);
	GLAPI PFNGLCOLOR4SPROC _glColor4s;
#define glColor4s _glColor4s
	typedef void (APIENTRYP PFNGLCOLOR4SVPROC)(const GLshort *v);
	GLAPI PFNGLCOLOR4SVPROC _glColor4sv;
#define glColor4sv _glColor4sv
	typedef void (APIENTRYP PFNGLCOLOR4UBPROC)(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
	GLAPI PFNGLCOLOR4UBPROC _glColor4ub;
#define glColor4ub _glColor4ub
	typedef void (APIENTRYP PFNGLCOLOR4UBVPROC)(const GLubyte *v);
	GLAPI PFNGLCOLOR4UBVPROC _glColor4ubv;
#define glColor4ubv _glColor4ubv
	typedef void (APIENTRYP PFNGLCOLOR4UIPROC)(GLuint red, GLuint green, GLuint blue, GLuint alpha);
	GLAPI PFNGLCOLOR4UIPROC _glColor4ui;
#define glColor4ui _glColor4ui
	typedef void (APIENTRYP PFNGLCOLOR4UIVPROC)(const GLuint *v);
	GLAPI PFNGLCOLOR4UIVPROC _glColor4uiv;
#define glColor4uiv _glColor4uiv
	typedef void (APIENTRYP PFNGLCOLOR4USPROC)(GLushort red, GLushort green, GLushort blue, GLushort alpha);
	GLAPI PFNGLCOLOR4USPROC _glColor4us;
#define glColor4us _glColor4us
	typedef void (APIENTRYP PFNGLCOLOR4USVPROC)(const GLushort *v);
	GLAPI PFNGLCOLOR4USVPROC _glColor4usv;
#define glColor4usv _glColor4usv
	typedef void (APIENTRYP PFNGLEDGEFLAGPROC)(GLboolean flag);
	GLAPI PFNGLEDGEFLAGPROC _glEdgeFlag;
#define glEdgeFlag _glEdgeFlag
	typedef void (APIENTRYP PFNGLEDGEFLAGVPROC)(const GLboolean *flag);
	GLAPI PFNGLEDGEFLAGVPROC _glEdgeFlagv;
#define glEdgeFlagv _glEdgeFlagv
	typedef void (APIENTRYP PFNGLENDPROC)(void);
	GLAPI PFNGLENDPROC _glEnd;
#define glEnd _glEnd
	typedef void (APIENTRYP PFNGLINDEXDPROC)(GLdouble c);
	GLAPI PFNGLINDEXDPROC _glIndexd;
#define glIndexd _glIndexd
	typedef void (APIENTRYP PFNGLINDEXDVPROC)(const GLdouble *c);
	GLAPI PFNGLINDEXDVPROC _glIndexdv;
#define glIndexdv _glIndexdv
	typedef void (APIENTRYP PFNGLINDEXFPROC)(GLfloat c);
	GLAPI PFNGLINDEXFPROC _glIndexf;
#define glIndexf _glIndexf
	typedef void (APIENTRYP PFNGLINDEXFVPROC)(const GLfloat *c);
	GLAPI PFNGLINDEXFVPROC _glIndexfv;
#define glIndexfv _glIndexfv
	typedef void (APIENTRYP PFNGLINDEXIPROC)(GLint c);
	GLAPI PFNGLINDEXIPROC _glIndexi;
#define glIndexi _glIndexi
	typedef void (APIENTRYP PFNGLINDEXIVPROC)(const GLint *c);
	GLAPI PFNGLINDEXIVPROC _glIndexiv;
#define glIndexiv _glIndexiv
	typedef void (APIENTRYP PFNGLINDEXSPROC)(GLshort c);
	GLAPI PFNGLINDEXSPROC _glIndexs;
#define glIndexs _glIndexs
	typedef void (APIENTRYP PFNGLINDEXSVPROC)(const GLshort *c);
	GLAPI PFNGLINDEXSVPROC _glIndexsv;
#define glIndexsv _glIndexsv
	typedef void (APIENTRYP PFNGLNORMAL3BPROC)(GLbyte nx, GLbyte ny, GLbyte nz);
	GLAPI PFNGLNORMAL3BPROC _glNormal3b;
#define glNormal3b _glNormal3b
	typedef void (APIENTRYP PFNGLNORMAL3BVPROC)(const GLbyte *v);
	GLAPI PFNGLNORMAL3BVPROC _glNormal3bv;
#define glNormal3bv _glNormal3bv
	typedef void (APIENTRYP PFNGLNORMAL3DPROC)(GLdouble nx, GLdouble ny, GLdouble nz);
	GLAPI PFNGLNORMAL3DPROC _glNormal3d;
#define glNormal3d _glNormal3d
	typedef void (APIENTRYP PFNGLNORMAL3DVPROC)(const GLdouble *v);
	GLAPI PFNGLNORMAL3DVPROC _glNormal3dv;
#define glNormal3dv _glNormal3dv
	typedef void (APIENTRYP PFNGLNORMAL3FPROC)(GLfloat nx, GLfloat ny, GLfloat nz);
	GLAPI PFNGLNORMAL3FPROC _glNormal3f;
#define glNormal3f _glNormal3f
	typedef void (APIENTRYP PFNGLNORMAL3FVPROC)(const GLfloat *v);
	GLAPI PFNGLNORMAL3FVPROC _glNormal3fv;
#define glNormal3fv _glNormal3fv
	typedef void (APIENTRYP PFNGLNORMAL3IPROC)(GLint nx, GLint ny, GLint nz);
	GLAPI PFNGLNORMAL3IPROC _glNormal3i;
#define glNormal3i _glNormal3i
	typedef void (APIENTRYP PFNGLNORMAL3IVPROC)(const GLint *v);
	GLAPI PFNGLNORMAL3IVPROC _glNormal3iv;
#define glNormal3iv _glNormal3iv
	typedef void (APIENTRYP PFNGLNORMAL3SPROC)(GLshort nx, GLshort ny, GLshort nz);
	GLAPI PFNGLNORMAL3SPROC _glNormal3s;
#define glNormal3s _glNormal3s
	typedef void (APIENTRYP PFNGLNORMAL3SVPROC)(const GLshort *v);
	GLAPI PFNGLNORMAL3SVPROC _glNormal3sv;
#define glNormal3sv _glNormal3sv
	typedef void (APIENTRYP PFNGLRASTERPOS2DPROC)(GLdouble x, GLdouble y);
	GLAPI PFNGLRASTERPOS2DPROC _glRasterPos2d;
#define glRasterPos2d _glRasterPos2d
	typedef void (APIENTRYP PFNGLRASTERPOS2DVPROC)(const GLdouble *v);
	GLAPI PFNGLRASTERPOS2DVPROC _glRasterPos2dv;
#define glRasterPos2dv _glRasterPos2dv
	typedef void (APIENTRYP PFNGLRASTERPOS2FPROC)(GLfloat x, GLfloat y);
	GLAPI PFNGLRASTERPOS2FPROC _glRasterPos2f;
#define glRasterPos2f _glRasterPos2f
	typedef void (APIENTRYP PFNGLRASTERPOS2FVPROC)(const GLfloat *v);
	GLAPI PFNGLRASTERPOS2FVPROC _glRasterPos2fv;
#define glRasterPos2fv _glRasterPos2fv
	typedef void (APIENTRYP PFNGLRASTERPOS2IPROC)(GLint x, GLint y);
	GLAPI PFNGLRASTERPOS2IPROC _glRasterPos2i;
#define glRasterPos2i _glRasterPos2i
	typedef void (APIENTRYP PFNGLRASTERPOS2IVPROC)(const GLint *v);
	GLAPI PFNGLRASTERPOS2IVPROC _glRasterPos2iv;
#define glRasterPos2iv _glRasterPos2iv
	typedef void (APIENTRYP PFNGLRASTERPOS2SPROC)(GLshort x, GLshort y);
	GLAPI PFNGLRASTERPOS2SPROC _glRasterPos2s;
#define glRasterPos2s _glRasterPos2s
	typedef void (APIENTRYP PFNGLRASTERPOS2SVPROC)(const GLshort *v);
	GLAPI PFNGLRASTERPOS2SVPROC _glRasterPos2sv;
#define glRasterPos2sv _glRasterPos2sv
	typedef void (APIENTRYP PFNGLRASTERPOS3DPROC)(GLdouble x, GLdouble y, GLdouble z);
	GLAPI PFNGLRASTERPOS3DPROC _glRasterPos3d;
#define glRasterPos3d _glRasterPos3d
	typedef void (APIENTRYP PFNGLRASTERPOS3DVPROC)(const GLdouble *v);
	GLAPI PFNGLRASTERPOS3DVPROC _glRasterPos3dv;
#define glRasterPos3dv _glRasterPos3dv
	typedef void (APIENTRYP PFNGLRASTERPOS3FPROC)(GLfloat x, GLfloat y, GLfloat z);
	GLAPI PFNGLRASTERPOS3FPROC _glRasterPos3f;
#define glRasterPos3f _glRasterPos3f
	typedef void (APIENTRYP PFNGLRASTERPOS3FVPROC)(const GLfloat *v);
	GLAPI PFNGLRASTERPOS3FVPROC _glRasterPos3fv;
#define glRasterPos3fv _glRasterPos3fv
	typedef void (APIENTRYP PFNGLRASTERPOS3IPROC)(GLint x, GLint y, GLint z);
	GLAPI PFNGLRASTERPOS3IPROC _glRasterPos3i;
#define glRasterPos3i _glRasterPos3i
	typedef void (APIENTRYP PFNGLRASTERPOS3IVPROC)(const GLint *v);
	GLAPI PFNGLRASTERPOS3IVPROC _glRasterPos3iv;
#define glRasterPos3iv _glRasterPos3iv
	typedef void (APIENTRYP PFNGLRASTERPOS3SPROC)(GLshort x, GLshort y, GLshort z);
	GLAPI PFNGLRASTERPOS3SPROC _glRasterPos3s;
#define glRasterPos3s _glRasterPos3s
	typedef void (APIENTRYP PFNGLRASTERPOS3SVPROC)(const GLshort *v);
	GLAPI PFNGLRASTERPOS3SVPROC _glRasterPos3sv;
#define glRasterPos3sv _glRasterPos3sv
	typedef void (APIENTRYP PFNGLRASTERPOS4DPROC)(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	GLAPI PFNGLRASTERPOS4DPROC _glRasterPos4d;
#define glRasterPos4d _glRasterPos4d
	typedef void (APIENTRYP PFNGLRASTERPOS4DVPROC)(const GLdouble *v);
	GLAPI PFNGLRASTERPOS4DVPROC _glRasterPos4dv;
#define glRasterPos4dv _glRasterPos4dv
	typedef void (APIENTRYP PFNGLRASTERPOS4FPROC)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	GLAPI PFNGLRASTERPOS4FPROC _glRasterPos4f;
#define glRasterPos4f _glRasterPos4f
	typedef void (APIENTRYP PFNGLRASTERPOS4FVPROC)(const GLfloat *v);
	GLAPI PFNGLRASTERPOS4FVPROC _glRasterPos4fv;
#define glRasterPos4fv _glRasterPos4fv
	typedef void (APIENTRYP PFNGLRASTERPOS4IPROC)(GLint x, GLint y, GLint z, GLint w);
	GLAPI PFNGLRASTERPOS4IPROC _glRasterPos4i;
#define glRasterPos4i _glRasterPos4i
	typedef void (APIENTRYP PFNGLRASTERPOS4IVPROC)(const GLint *v);
	GLAPI PFNGLRASTERPOS4IVPROC _glRasterPos4iv;
#define glRasterPos4iv _glRasterPos4iv
	typedef void (APIENTRYP PFNGLRASTERPOS4SPROC)(GLshort x, GLshort y, GLshort z, GLshort w);
	GLAPI PFNGLRASTERPOS4SPROC _glRasterPos4s;
#define glRasterPos4s _glRasterPos4s
	typedef void (APIENTRYP PFNGLRASTERPOS4SVPROC)(const GLshort *v);
	GLAPI PFNGLRASTERPOS4SVPROC _glRasterPos4sv;
#define glRasterPos4sv _glRasterPos4sv
	typedef void (APIENTRYP PFNGLRECTDPROC)(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
	GLAPI PFNGLRECTDPROC _glRectd;
#define glRectd _glRectd
	typedef void (APIENTRYP PFNGLRECTDVPROC)(const GLdouble *v1, const GLdouble *v2);
	GLAPI PFNGLRECTDVPROC _glRectdv;
#define glRectdv _glRectdv
	typedef void (APIENTRYP PFNGLRECTFPROC)(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
	GLAPI PFNGLRECTFPROC _glRectf;
#define glRectf _glRectf
	typedef void (APIENTRYP PFNGLRECTFVPROC)(const GLfloat *v1, const GLfloat *v2);
	GLAPI PFNGLRECTFVPROC _glRectfv;
#define glRectfv _glRectfv
	typedef void (APIENTRYP PFNGLRECTIPROC)(GLint x1, GLint y1, GLint x2, GLint y2);
	GLAPI PFNGLRECTIPROC _glRecti;
#define glRecti _glRecti
	typedef void (APIENTRYP PFNGLRECTIVPROC)(const GLint *v1, const GLint *v2);
	GLAPI PFNGLRECTIVPROC _glRectiv;
#define glRectiv _glRectiv
	typedef void (APIENTRYP PFNGLRECTSPROC)(GLshort x1, GLshort y1, GLshort x2, GLshort y2);
	GLAPI PFNGLRECTSPROC _glRects;
#define glRects _glRects
	typedef void (APIENTRYP PFNGLRECTSVPROC)(const GLshort *v1, const GLshort *v2);
	GLAPI PFNGLRECTSVPROC _glRectsv;
#define glRectsv _glRectsv
	typedef void (APIENTRYP PFNGLTEXCOORD1DPROC)(GLdouble s);
	GLAPI PFNGLTEXCOORD1DPROC _glTexCoord1d;
#define glTexCoord1d _glTexCoord1d
	typedef void (APIENTRYP PFNGLTEXCOORD1DVPROC)(const GLdouble *v);
	GLAPI PFNGLTEXCOORD1DVPROC _glTexCoord1dv;
#define glTexCoord1dv _glTexCoord1dv
	typedef void (APIENTRYP PFNGLTEXCOORD1FPROC)(GLfloat s);
	GLAPI PFNGLTEXCOORD1FPROC _glTexCoord1f;
#define glTexCoord1f _glTexCoord1f
	typedef void (APIENTRYP PFNGLTEXCOORD1FVPROC)(const GLfloat *v);
	GLAPI PFNGLTEXCOORD1FVPROC _glTexCoord1fv;
#define glTexCoord1fv _glTexCoord1fv
	typedef void (APIENTRYP PFNGLTEXCOORD1IPROC)(GLint s);
	GLAPI PFNGLTEXCOORD1IPROC _glTexCoord1i;
#define glTexCoord1i _glTexCoord1i
	typedef void (APIENTRYP PFNGLTEXCOORD1IVPROC)(const GLint *v);
	GLAPI PFNGLTEXCOORD1IVPROC _glTexCoord1iv;
#define glTexCoord1iv _glTexCoord1iv
	typedef void (APIENTRYP PFNGLTEXCOORD1SPROC)(GLshort s);
	GLAPI PFNGLTEXCOORD1SPROC _glTexCoord1s;
#define glTexCoord1s _glTexCoord1s
	typedef void (APIENTRYP PFNGLTEXCOORD1SVPROC)(const GLshort *v);
	GLAPI PFNGLTEXCOORD1SVPROC _glTexCoord1sv;
#define glTexCoord1sv _glTexCoord1sv
	typedef void (APIENTRYP PFNGLTEXCOORD2DPROC)(GLdouble s, GLdouble t);
	GLAPI PFNGLTEXCOORD2DPROC _glTexCoord2d;
#define glTexCoord2d _glTexCoord2d
	typedef void (APIENTRYP PFNGLTEXCOORD2DVPROC)(const GLdouble *v);
	GLAPI PFNGLTEXCOORD2DVPROC _glTexCoord2dv;
#define glTexCoord2dv _glTexCoord2dv
	typedef void (APIENTRYP PFNGLTEXCOORD2FPROC)(GLfloat s, GLfloat t);
	GLAPI PFNGLTEXCOORD2FPROC _glTexCoord2f;
#define glTexCoord2f _glTexCoord2f
	typedef void (APIENTRYP PFNGLTEXCOORD2FVPROC)(const GLfloat *v);
	GLAPI PFNGLTEXCOORD2FVPROC _glTexCoord2fv;
#define glTexCoord2fv _glTexCoord2fv
	typedef void (APIENTRYP PFNGLTEXCOORD2IPROC)(GLint s, GLint t);
	GLAPI PFNGLTEXCOORD2IPROC _glTexCoord2i;
#define glTexCoord2i _glTexCoord2i
	typedef void (APIENTRYP PFNGLTEXCOORD2IVPROC)(const GLint *v);
	GLAPI PFNGLTEXCOORD2IVPROC _glTexCoord2iv;
#define glTexCoord2iv _glTexCoord2iv
	typedef void (APIENTRYP PFNGLTEXCOORD2SPROC)(GLshort s, GLshort t);
	GLAPI PFNGLTEXCOORD2SPROC _glTexCoord2s;
#define glTexCoord2s _glTexCoord2s
	typedef void (APIENTRYP PFNGLTEXCOORD2SVPROC)(const GLshort *v);
	GLAPI PFNGLTEXCOORD2SVPROC _glTexCoord2sv;
#define glTexCoord2sv _glTexCoord2sv
	typedef void (APIENTRYP PFNGLTEXCOORD3DPROC)(GLdouble s, GLdouble t, GLdouble r);
	GLAPI PFNGLTEXCOORD3DPROC _glTexCoord3d;
#define glTexCoord3d _glTexCoord3d
	typedef void (APIENTRYP PFNGLTEXCOORD3DVPROC)(const GLdouble *v);
	GLAPI PFNGLTEXCOORD3DVPROC _glTexCoord3dv;
#define glTexCoord3dv _glTexCoord3dv
	typedef void (APIENTRYP PFNGLTEXCOORD3FPROC)(GLfloat s, GLfloat t, GLfloat r);
	GLAPI PFNGLTEXCOORD3FPROC _glTexCoord3f;
#define glTexCoord3f _glTexCoord3f
	typedef void (APIENTRYP PFNGLTEXCOORD3FVPROC)(const GLfloat *v);
	GLAPI PFNGLTEXCOORD3FVPROC _glTexCoord3fv;
#define glTexCoord3fv _glTexCoord3fv
	typedef void (APIENTRYP PFNGLTEXCOORD3IPROC)(GLint s, GLint t, GLint r);
	GLAPI PFNGLTEXCOORD3IPROC _glTexCoord3i;
#define glTexCoord3i _glTexCoord3i
	typedef void (APIENTRYP PFNGLTEXCOORD3IVPROC)(const GLint *v);
	GLAPI PFNGLTEXCOORD3IVPROC _glTexCoord3iv;
#define glTexCoord3iv _glTexCoord3iv
	typedef void (APIENTRYP PFNGLTEXCOORD3SPROC)(GLshort s, GLshort t, GLshort r);
	GLAPI PFNGLTEXCOORD3SPROC _glTexCoord3s;
#define glTexCoord3s _glTexCoord3s
	typedef void (APIENTRYP PFNGLTEXCOORD3SVPROC)(const GLshort *v);
	GLAPI PFNGLTEXCOORD3SVPROC _glTexCoord3sv;
#define glTexCoord3sv _glTexCoord3sv
	typedef void (APIENTRYP PFNGLTEXCOORD4DPROC)(GLdouble s, GLdouble t, GLdouble r, GLdouble q);
	GLAPI PFNGLTEXCOORD4DPROC _glTexCoord4d;
#define glTexCoord4d _glTexCoord4d
	typedef void (APIENTRYP PFNGLTEXCOORD4DVPROC)(const GLdouble *v);
	GLAPI PFNGLTEXCOORD4DVPROC _glTexCoord4dv;
#define glTexCoord4dv _glTexCoord4dv
	typedef void (APIENTRYP PFNGLTEXCOORD4FPROC)(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
	GLAPI PFNGLTEXCOORD4FPROC _glTexCoord4f;
#define glTexCoord4f _glTexCoord4f
	typedef void (APIENTRYP PFNGLTEXCOORD4FVPROC)(const GLfloat *v);
	GLAPI PFNGLTEXCOORD4FVPROC _glTexCoord4fv;
#define glTexCoord4fv _glTexCoord4fv
	typedef void (APIENTRYP PFNGLTEXCOORD4IPROC)(GLint s, GLint t, GLint r, GLint q);
	GLAPI PFNGLTEXCOORD4IPROC _glTexCoord4i;
#define glTexCoord4i _glTexCoord4i
	typedef void (APIENTRYP PFNGLTEXCOORD4IVPROC)(const GLint *v);
	GLAPI PFNGLTEXCOORD4IVPROC _glTexCoord4iv;
#define glTexCoord4iv _glTexCoord4iv
	typedef void (APIENTRYP PFNGLTEXCOORD4SPROC)(GLshort s, GLshort t, GLshort r, GLshort q);
	GLAPI PFNGLTEXCOORD4SPROC _glTexCoord4s;
#define glTexCoord4s _glTexCoord4s
	typedef void (APIENTRYP PFNGLTEXCOORD4SVPROC)(const GLshort *v);
	GLAPI PFNGLTEXCOORD4SVPROC _glTexCoord4sv;
#define glTexCoord4sv _glTexCoord4sv
	typedef void (APIENTRYP PFNGLVERTEX2DPROC)(GLdouble x, GLdouble y);
	GLAPI PFNGLVERTEX2DPROC _glVertex2d;
#define glVertex2d _glVertex2d
	typedef void (APIENTRYP PFNGLVERTEX2DVPROC)(const GLdouble *v);
	GLAPI PFNGLVERTEX2DVPROC _glVertex2dv;
#define glVertex2dv _glVertex2dv
	typedef void (APIENTRYP PFNGLVERTEX2FPROC)(GLfloat x, GLfloat y);
	GLAPI PFNGLVERTEX2FPROC _glVertex2f;
#define glVertex2f _glVertex2f
	typedef void (APIENTRYP PFNGLVERTEX2FVPROC)(const GLfloat *v);
	GLAPI PFNGLVERTEX2FVPROC _glVertex2fv;
#define glVertex2fv _glVertex2fv
	typedef void (APIENTRYP PFNGLVERTEX2IPROC)(GLint x, GLint y);
	GLAPI PFNGLVERTEX2IPROC _glVertex2i;
#define glVertex2i _glVertex2i
	typedef void (APIENTRYP PFNGLVERTEX2IVPROC)(const GLint *v);
	GLAPI PFNGLVERTEX2IVPROC _glVertex2iv;
#define glVertex2iv _glVertex2iv
	typedef void (APIENTRYP PFNGLVERTEX2SPROC)(GLshort x, GLshort y);
	GLAPI PFNGLVERTEX2SPROC _glVertex2s;
#define glVertex2s _glVertex2s
	typedef void (APIENTRYP PFNGLVERTEX2SVPROC)(const GLshort *v);
	GLAPI PFNGLVERTEX2SVPROC _glVertex2sv;
#define glVertex2sv _glVertex2sv
	typedef void (APIENTRYP PFNGLVERTEX3DPROC)(GLdouble x, GLdouble y, GLdouble z);
	GLAPI PFNGLVERTEX3DPROC _glVertex3d;
#define glVertex3d _glVertex3d
	typedef void (APIENTRYP PFNGLVERTEX3DVPROC)(const GLdouble *v);
	GLAPI PFNGLVERTEX3DVPROC _glVertex3dv;
#define glVertex3dv _glVertex3dv
	typedef void (APIENTRYP PFNGLVERTEX3FPROC)(GLfloat x, GLfloat y, GLfloat z);
	GLAPI PFNGLVERTEX3FPROC _glVertex3f;
#define glVertex3f _glVertex3f
	typedef void (APIENTRYP PFNGLVERTEX3FVPROC)(const GLfloat *v);
	GLAPI PFNGLVERTEX3FVPROC _glVertex3fv;
#define glVertex3fv _glVertex3fv
	typedef void (APIENTRYP PFNGLVERTEX3IPROC)(GLint x, GLint y, GLint z);
	GLAPI PFNGLVERTEX3IPROC _glVertex3i;
#define glVertex3i _glVertex3i
	typedef void (APIENTRYP PFNGLVERTEX3IVPROC)(const GLint *v);
	GLAPI PFNGLVERTEX3IVPROC _glVertex3iv;
#define glVertex3iv _glVertex3iv
	typedef void (APIENTRYP PFNGLVERTEX3SPROC)(GLshort x, GLshort y, GLshort z);
	GLAPI PFNGLVERTEX3SPROC _glVertex3s;
#define glVertex3s _glVertex3s
	typedef void (APIENTRYP PFNGLVERTEX3SVPROC)(const GLshort *v);
	GLAPI PFNGLVERTEX3SVPROC _glVertex3sv;
#define glVertex3sv _glVertex3sv
	typedef void (APIENTRYP PFNGLVERTEX4DPROC)(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	GLAPI PFNGLVERTEX4DPROC _glVertex4d;
#define glVertex4d _glVertex4d
	typedef void (APIENTRYP PFNGLVERTEX4DVPROC)(const GLdouble *v);
	GLAPI PFNGLVERTEX4DVPROC _glVertex4dv;
#define glVertex4dv _glVertex4dv
	typedef void (APIENTRYP PFNGLVERTEX4FPROC)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	GLAPI PFNGLVERTEX4FPROC _glVertex4f;
#define glVertex4f _glVertex4f
	typedef void (APIENTRYP PFNGLVERTEX4FVPROC)(const GLfloat *v);
	GLAPI PFNGLVERTEX4FVPROC _glVertex4fv;
#define glVertex4fv _glVertex4fv
	typedef void (APIENTRYP PFNGLVERTEX4IPROC)(GLint x, GLint y, GLint z, GLint w);
	GLAPI PFNGLVERTEX4IPROC _glVertex4i;
#define glVertex4i _glVertex4i
	typedef void (APIENTRYP PFNGLVERTEX4IVPROC)(const GLint *v);
	GLAPI PFNGLVERTEX4IVPROC _glVertex4iv;
#define glVertex4iv _glVertex4iv
	typedef void (APIENTRYP PFNGLVERTEX4SPROC)(GLshort x, GLshort y, GLshort z, GLshort w);
	GLAPI PFNGLVERTEX4SPROC _glVertex4s;
#define glVertex4s _glVertex4s
	typedef void (APIENTRYP PFNGLVERTEX4SVPROC)(const GLshort *v);
	GLAPI PFNGLVERTEX4SVPROC _glVertex4sv;
#define glVertex4sv _glVertex4sv
	typedef void (APIENTRYP PFNGLCLIPPLANEPROC)(GLenum plane, const GLdouble *equation);
	GLAPI PFNGLCLIPPLANEPROC _glClipPlane;
#define glClipPlane _glClipPlane
	typedef void (APIENTRYP PFNGLCOLORMATERIALPROC)(GLenum face, GLenum mode);
	GLAPI PFNGLCOLORMATERIALPROC _glColorMaterial;
#define glColorMaterial _glColorMaterial
	typedef void (APIENTRYP PFNGLFOGFPROC)(GLenum pname, GLfloat param);
	GLAPI PFNGLFOGFPROC _glFogf;
#define glFogf _glFogf
	typedef void (APIENTRYP PFNGLFOGFVPROC)(GLenum pname, const GLfloat *params);
	GLAPI PFNGLFOGFVPROC _glFogfv;
#define glFogfv _glFogfv
	typedef void (APIENTRYP PFNGLFOGIPROC)(GLenum pname, GLint param);
	GLAPI PFNGLFOGIPROC _glFogi;
#define glFogi _glFogi
	typedef void (APIENTRYP PFNGLFOGIVPROC)(GLenum pname, const GLint *params);
	GLAPI PFNGLFOGIVPROC _glFogiv;
#define glFogiv _glFogiv
	typedef void (APIENTRYP PFNGLLIGHTFPROC)(GLenum light, GLenum pname, GLfloat param);
	GLAPI PFNGLLIGHTFPROC _glLightf;
#define glLightf _glLightf
	typedef void (APIENTRYP PFNGLLIGHTFVPROC)(GLenum light, GLenum pname, const GLfloat *params);
	GLAPI PFNGLLIGHTFVPROC _glLightfv;
#define glLightfv _glLightfv
	typedef void (APIENTRYP PFNGLLIGHTIPROC)(GLenum light, GLenum pname, GLint param);
	GLAPI PFNGLLIGHTIPROC _glLighti;
#define glLighti _glLighti
	typedef void (APIENTRYP PFNGLLIGHTIVPROC)(GLenum light, GLenum pname, const GLint *params);
	GLAPI PFNGLLIGHTIVPROC _glLightiv;
#define glLightiv _glLightiv
	typedef void (APIENTRYP PFNGLLIGHTMODELFPROC)(GLenum pname, GLfloat param);
	GLAPI PFNGLLIGHTMODELFPROC _glLightModelf;
#define glLightModelf _glLightModelf
	typedef void (APIENTRYP PFNGLLIGHTMODELFVPROC)(GLenum pname, const GLfloat *params);
	GLAPI PFNGLLIGHTMODELFVPROC _glLightModelfv;
#define glLightModelfv _glLightModelfv
	typedef void (APIENTRYP PFNGLLIGHTMODELIPROC)(GLenum pname, GLint param);
	GLAPI PFNGLLIGHTMODELIPROC _glLightModeli;
#define glLightModeli _glLightModeli
	typedef void (APIENTRYP PFNGLLIGHTMODELIVPROC)(GLenum pname, const GLint *params);
	GLAPI PFNGLLIGHTMODELIVPROC _glLightModeliv;
#define glLightModeliv _glLightModeliv
	typedef void (APIENTRYP PFNGLLINESTIPPLEPROC)(GLint factor, GLushort pattern);
	GLAPI PFNGLLINESTIPPLEPROC _glLineStipple;
#define glLineStipple _glLineStipple
	typedef void (APIENTRYP PFNGLMATERIALFPROC)(GLenum face, GLenum pname, GLfloat param);
	GLAPI PFNGLMATERIALFPROC _glMaterialf;
#define glMaterialf _glMaterialf
	typedef void (APIENTRYP PFNGLMATERIALFVPROC)(GLenum face, GLenum pname, const GLfloat *params);
	GLAPI PFNGLMATERIALFVPROC _glMaterialfv;
#define glMaterialfv _glMaterialfv
	typedef void (APIENTRYP PFNGLMATERIALIPROC)(GLenum face, GLenum pname, GLint param);
	GLAPI PFNGLMATERIALIPROC _glMateriali;
#define glMateriali _glMateriali
	typedef void (APIENTRYP PFNGLMATERIALIVPROC)(GLenum face, GLenum pname, const GLint *params);
	GLAPI PFNGLMATERIALIVPROC _glMaterialiv;
#define glMaterialiv _glMaterialiv
	typedef void (APIENTRYP PFNGLPOLYGONSTIPPLEPROC)(const GLubyte *mask);
	GLAPI PFNGLPOLYGONSTIPPLEPROC _glPolygonStipple;
#define glPolygonStipple _glPolygonStipple
	typedef void (APIENTRYP PFNGLSHADEMODELPROC)(GLenum mode);
	GLAPI PFNGLSHADEMODELPROC _glShadeModel;
#define glShadeModel _glShadeModel
	typedef void (APIENTRYP PFNGLTEXENVFPROC)(GLenum target, GLenum pname, GLfloat param);
	GLAPI PFNGLTEXENVFPROC _glTexEnvf;
#define glTexEnvf _glTexEnvf
	typedef void (APIENTRYP PFNGLTEXENVFVPROC)(GLenum target, GLenum pname, const GLfloat *params);
	GLAPI PFNGLTEXENVFVPROC _glTexEnvfv;
#define glTexEnvfv _glTexEnvfv
	typedef void (APIENTRYP PFNGLTEXENVIPROC)(GLenum target, GLenum pname, GLint param);
	GLAPI PFNGLTEXENVIPROC _glTexEnvi;
#define glTexEnvi _glTexEnvi
	typedef void (APIENTRYP PFNGLTEXENVIVPROC)(GLenum target, GLenum pname, const GLint *params);
	GLAPI PFNGLTEXENVIVPROC _glTexEnviv;
#define glTexEnviv _glTexEnviv
	typedef void (APIENTRYP PFNGLTEXGENDPROC)(GLenum coord, GLenum pname, GLdouble param);
	GLAPI PFNGLTEXGENDPROC _glTexGend;
#define glTexGend _glTexGend
	typedef void (APIENTRYP PFNGLTEXGENDVPROC)(GLenum coord, GLenum pname, const GLdouble *params);
	GLAPI PFNGLTEXGENDVPROC _glTexGendv;
#define glTexGendv _glTexGendv
	typedef void (APIENTRYP PFNGLTEXGENFPROC)(GLenum coord, GLenum pname, GLfloat param);
	GLAPI PFNGLTEXGENFPROC _glTexGenf;
#define glTexGenf _glTexGenf
	typedef void (APIENTRYP PFNGLTEXGENFVPROC)(GLenum coord, GLenum pname, const GLfloat *params);
	GLAPI PFNGLTEXGENFVPROC _glTexGenfv;
#define glTexGenfv _glTexGenfv
	typedef void (APIENTRYP PFNGLTEXGENIPROC)(GLenum coord, GLenum pname, GLint param);
	GLAPI PFNGLTEXGENIPROC _glTexGeni;
#define glTexGeni _glTexGeni
	typedef void (APIENTRYP PFNGLTEXGENIVPROC)(GLenum coord, GLenum pname, const GLint *params);
	GLAPI PFNGLTEXGENIVPROC _glTexGeniv;
#define glTexGeniv _glTexGeniv
	typedef void (APIENTRYP PFNGLFEEDBACKBUFFERPROC)(GLsizei size, GLenum type, GLfloat *buffer);
	GLAPI PFNGLFEEDBACKBUFFERPROC _glFeedbackBuffer;
#define glFeedbackBuffer _glFeedbackBuffer
	typedef void (APIENTRYP PFNGLSELECTBUFFERPROC)(GLsizei size, GLuint *buffer);
	GLAPI PFNGLSELECTBUFFERPROC _glSelectBuffer;
#define glSelectBuffer _glSelectBuffer
	typedef GLint(APIENTRYP PFNGLRENDERMODEPROC)(GLenum mode);
	GLAPI PFNGLRENDERMODEPROC _glRenderMode;
#define glRenderMode _glRenderMode
	typedef void (APIENTRYP PFNGLINITNAMESPROC)(void);
	GLAPI PFNGLINITNAMESPROC _glInitNames;
#define glInitNames _glInitNames
	typedef void (APIENTRYP PFNGLLOADNAMEPROC)(GLuint name);
	GLAPI PFNGLLOADNAMEPROC _glLoadName;
#define glLoadName _glLoadName
	typedef void (APIENTRYP PFNGLPASSTHROUGHPROC)(GLfloat token);
	GLAPI PFNGLPASSTHROUGHPROC _glPassThrough;
#define glPassThrough _glPassThrough
	typedef void (APIENTRYP PFNGLPOPNAMEPROC)(void);
	GLAPI PFNGLPOPNAMEPROC _glPopName;
#define glPopName _glPopName
	typedef void (APIENTRYP PFNGLPUSHNAMEPROC)(GLuint name);
	GLAPI PFNGLPUSHNAMEPROC _glPushName;
#define glPushName _glPushName
	typedef void (APIENTRYP PFNGLCLEARACCUMPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	GLAPI PFNGLCLEARACCUMPROC _glClearAccum;
#define glClearAccum _glClearAccum
	typedef void (APIENTRYP PFNGLCLEARINDEXPROC)(GLfloat c);
	GLAPI PFNGLCLEARINDEXPROC _glClearIndex;
#define glClearIndex _glClearIndex
	typedef void (APIENTRYP PFNGLINDEXMASKPROC)(GLuint mask);
	GLAPI PFNGLINDEXMASKPROC _glIndexMask;
#define glIndexMask _glIndexMask
	typedef void (APIENTRYP PFNGLACCUMPROC)(GLenum op, GLfloat value);
	GLAPI PFNGLACCUMPROC _glAccum;
#define glAccum _glAccum
	typedef void (APIENTRYP PFNGLPOPATTRIBPROC)(void);
	GLAPI PFNGLPOPATTRIBPROC _glPopAttrib;
#define glPopAttrib _glPopAttrib
	typedef void (APIENTRYP PFNGLPUSHATTRIBPROC)(GLbitfield mask);
	GLAPI PFNGLPUSHATTRIBPROC _glPushAttrib;
#define glPushAttrib _glPushAttrib
	typedef void (APIENTRYP PFNGLMAP1DPROC)(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
	GLAPI PFNGLMAP1DPROC _glMap1d;
#define glMap1d _glMap1d
	typedef void (APIENTRYP PFNGLMAP1FPROC)(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
	GLAPI PFNGLMAP1FPROC _glMap1f;
#define glMap1f _glMap1f
	typedef void (APIENTRYP PFNGLMAP2DPROC)(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
	GLAPI PFNGLMAP2DPROC _glMap2d;
#define glMap2d _glMap2d
	typedef void (APIENTRYP PFNGLMAP2FPROC)(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
	GLAPI PFNGLMAP2FPROC _glMap2f;
#define glMap2f _glMap2f
	typedef void (APIENTRYP PFNGLMAPGRID1DPROC)(GLint un, GLdouble u1, GLdouble u2);
	GLAPI PFNGLMAPGRID1DPROC _glMapGrid1d;
#define glMapGrid1d _glMapGrid1d
	typedef void (APIENTRYP PFNGLMAPGRID1FPROC)(GLint un, GLfloat u1, GLfloat u2);
	GLAPI PFNGLMAPGRID1FPROC _glMapGrid1f;
#define glMapGrid1f _glMapGrid1f
	typedef void (APIENTRYP PFNGLMAPGRID2DPROC)(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
	GLAPI PFNGLMAPGRID2DPROC _glMapGrid2d;
#define glMapGrid2d _glMapGrid2d
	typedef void (APIENTRYP PFNGLMAPGRID2FPROC)(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
	GLAPI PFNGLMAPGRID2FPROC _glMapGrid2f;
#define glMapGrid2f _glMapGrid2f
	typedef void (APIENTRYP PFNGLEVALCOORD1DPROC)(GLdouble u);
	GLAPI PFNGLEVALCOORD1DPROC _glEvalCoord1d;
#define glEvalCoord1d _glEvalCoord1d
	typedef void (APIENTRYP PFNGLEVALCOORD1DVPROC)(const GLdouble *u);
	GLAPI PFNGLEVALCOORD1DVPROC _glEvalCoord1dv;
#define glEvalCoord1dv _glEvalCoord1dv
	typedef void (APIENTRYP PFNGLEVALCOORD1FPROC)(GLfloat u);
	GLAPI PFNGLEVALCOORD1FPROC _glEvalCoord1f;
#define glEvalCoord1f _glEvalCoord1f
	typedef void (APIENTRYP PFNGLEVALCOORD1FVPROC)(const GLfloat *u);
	GLAPI PFNGLEVALCOORD1FVPROC _glEvalCoord1fv;
#define glEvalCoord1fv _glEvalCoord1fv
	typedef void (APIENTRYP PFNGLEVALCOORD2DPROC)(GLdouble u, GLdouble v);
	GLAPI PFNGLEVALCOORD2DPROC _glEvalCoord2d;
#define glEvalCoord2d _glEvalCoord2d
	typedef void (APIENTRYP PFNGLEVALCOORD2DVPROC)(const GLdouble *u);
	GLAPI PFNGLEVALCOORD2DVPROC _glEvalCoord2dv;
#define glEvalCoord2dv _glEvalCoord2dv
	typedef void (APIENTRYP PFNGLEVALCOORD2FPROC)(GLfloat u, GLfloat v);
	GLAPI PFNGLEVALCOORD2FPROC _glEvalCoord2f;
#define glEvalCoord2f _glEvalCoord2f
	typedef void (APIENTRYP PFNGLEVALCOORD2FVPROC)(const GLfloat *u);
	GLAPI PFNGLEVALCOORD2FVPROC _glEvalCoord2fv;
#define glEvalCoord2fv _glEvalCoord2fv
	typedef void (APIENTRYP PFNGLEVALMESH1PROC)(GLenum mode, GLint i1, GLint i2);
	GLAPI PFNGLEVALMESH1PROC _glEvalMesh1;
#define glEvalMesh1 _glEvalMesh1
	typedef void (APIENTRYP PFNGLEVALPOINT1PROC)(GLint i);
	GLAPI PFNGLEVALPOINT1PROC _glEvalPoint1;
#define glEvalPoint1 _glEvalPoint1
	typedef void (APIENTRYP PFNGLEVALMESH2PROC)(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
	GLAPI PFNGLEVALMESH2PROC _glEvalMesh2;
#define glEvalMesh2 _glEvalMesh2
	typedef void (APIENTRYP PFNGLEVALPOINT2PROC)(GLint i, GLint j);
	GLAPI PFNGLEVALPOINT2PROC _glEvalPoint2;
#define glEvalPoint2 _glEvalPoint2
	typedef void (APIENTRYP PFNGLALPHAFUNCPROC)(GLenum func, GLfloat ref);
	GLAPI PFNGLALPHAFUNCPROC _glAlphaFunc;
#define glAlphaFunc _glAlphaFunc
	typedef void (APIENTRYP PFNGLPIXELZOOMPROC)(GLfloat xfactor, GLfloat yfactor);
	GLAPI PFNGLPIXELZOOMPROC _glPixelZoom;
#define glPixelZoom _glPixelZoom
	typedef void (APIENTRYP PFNGLPIXELTRANSFERFPROC)(GLenum pname, GLfloat param);
	GLAPI PFNGLPIXELTRANSFERFPROC _glPixelTransferf;
#define glPixelTransferf _glPixelTransferf
	typedef void (APIENTRYP PFNGLPIXELTRANSFERIPROC)(GLenum pname, GLint param);
	GLAPI PFNGLPIXELTRANSFERIPROC _glPixelTransferi;
#define glPixelTransferi _glPixelTransferi
	typedef void (APIENTRYP PFNGLPIXELMAPFVPROC)(GLenum map, GLsizei mapsize, const GLfloat *values);
	GLAPI PFNGLPIXELMAPFVPROC _glPixelMapfv;
#define glPixelMapfv _glPixelMapfv
	typedef void (APIENTRYP PFNGLPIXELMAPUIVPROC)(GLenum map, GLsizei mapsize, const GLuint *values);
	GLAPI PFNGLPIXELMAPUIVPROC _glPixelMapuiv;
#define glPixelMapuiv _glPixelMapuiv
	typedef void (APIENTRYP PFNGLPIXELMAPUSVPROC)(GLenum map, GLsizei mapsize, const GLushort *values);
	GLAPI PFNGLPIXELMAPUSVPROC _glPixelMapusv;
#define glPixelMapusv _glPixelMapusv
	typedef void (APIENTRYP PFNGLCOPYPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
	GLAPI PFNGLCOPYPIXELSPROC _glCopyPixels;
#define glCopyPixels _glCopyPixels
	typedef void (APIENTRYP PFNGLDRAWPIXELSPROC)(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
	GLAPI PFNGLDRAWPIXELSPROC _glDrawPixels;
#define glDrawPixels _glDrawPixels
	typedef void (APIENTRYP PFNGLGETCLIPPLANEPROC)(GLenum plane, GLdouble *equation);
	GLAPI PFNGLGETCLIPPLANEPROC _glGetClipPlane;
#define glGetClipPlane _glGetClipPlane
	typedef void (APIENTRYP PFNGLGETLIGHTFVPROC)(GLenum light, GLenum pname, GLfloat *params);
	GLAPI PFNGLGETLIGHTFVPROC _glGetLightfv;
#define glGetLightfv _glGetLightfv
	typedef void (APIENTRYP PFNGLGETLIGHTIVPROC)(GLenum light, GLenum pname, GLint *params);
	GLAPI PFNGLGETLIGHTIVPROC _glGetLightiv;
#define glGetLightiv _glGetLightiv
	typedef void (APIENTRYP PFNGLGETMAPDVPROC)(GLenum target, GLenum query, GLdouble *v);
	GLAPI PFNGLGETMAPDVPROC _glGetMapdv;
#define glGetMapdv _glGetMapdv
	typedef void (APIENTRYP PFNGLGETMAPFVPROC)(GLenum target, GLenum query, GLfloat *v);
	GLAPI PFNGLGETMAPFVPROC _glGetMapfv;
#define glGetMapfv _glGetMapfv
	typedef void (APIENTRYP PFNGLGETMAPIVPROC)(GLenum target, GLenum query, GLint *v);
	GLAPI PFNGLGETMAPIVPROC _glGetMapiv;
#define glGetMapiv _glGetMapiv
	typedef void (APIENTRYP PFNGLGETMATERIALFVPROC)(GLenum face, GLenum pname, GLfloat *params);
	GLAPI PFNGLGETMATERIALFVPROC _glGetMaterialfv;
#define glGetMaterialfv _glGetMaterialfv
	typedef void (APIENTRYP PFNGLGETMATERIALIVPROC)(GLenum face, GLenum pname, GLint *params);
	GLAPI PFNGLGETMATERIALIVPROC _glGetMaterialiv;
#define glGetMaterialiv _glGetMaterialiv
	typedef void (APIENTRYP PFNGLGETPIXELMAPFVPROC)(GLenum map, GLfloat *values);
	GLAPI PFNGLGETPIXELMAPFVPROC _glGetPixelMapfv;
#define glGetPixelMapfv _glGetPixelMapfv
	typedef void (APIENTRYP PFNGLGETPIXELMAPUIVPROC)(GLenum map, GLuint *values);
	GLAPI PFNGLGETPIXELMAPUIVPROC _glGetPixelMapuiv;
#define glGetPixelMapuiv _glGetPixelMapuiv
	typedef void (APIENTRYP PFNGLGETPIXELMAPUSVPROC)(GLenum map, GLushort *values);
	GLAPI PFNGLGETPIXELMAPUSVPROC _glGetPixelMapusv;
#define glGetPixelMapusv _glGetPixelMapusv
	typedef void (APIENTRYP PFNGLGETPOLYGONSTIPPLEPROC)(GLubyte *mask);
	GLAPI PFNGLGETPOLYGONSTIPPLEPROC _glGetPolygonStipple;
#define glGetPolygonStipple _glGetPolygonStipple
	typedef void (APIENTRYP PFNGLGETTEXENVFVPROC)(GLenum target, GLenum pname, GLfloat *params);
	GLAPI PFNGLGETTEXENVFVPROC _glGetTexEnvfv;
#define glGetTexEnvfv _glGetTexEnvfv
	typedef void (APIENTRYP PFNGLGETTEXENVIVPROC)(GLenum target, GLenum pname, GLint *params);
	GLAPI PFNGLGETTEXENVIVPROC _glGetTexEnviv;
#define glGetTexEnviv _glGetTexEnviv
	typedef void (APIENTRYP PFNGLGETTEXGENDVPROC)(GLenum coord, GLenum pname, GLdouble *params);
	GLAPI PFNGLGETTEXGENDVPROC _glGetTexGendv;
#define glGetTexGendv _glGetTexGendv
	typedef void (APIENTRYP PFNGLGETTEXGENFVPROC)(GLenum coord, GLenum pname, GLfloat *params);
	GLAPI PFNGLGETTEXGENFVPROC _glGetTexGenfv;
#define glGetTexGenfv _glGetTexGenfv
	typedef void (APIENTRYP PFNGLGETTEXGENIVPROC)(GLenum coord, GLenum pname, GLint *params);
	GLAPI PFNGLGETTEXGENIVPROC _glGetTexGeniv;
#define glGetTexGeniv _glGetTexGeniv
	typedef GLboolean(APIENTRYP PFNGLISLISTPROC)(GLuint list);
	GLAPI PFNGLISLISTPROC _glIsList;
#define glIsList _glIsList
	typedef void (APIENTRYP PFNGLFRUSTUMPROC)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
	GLAPI PFNGLFRUSTUMPROC _glFrustum;
#define glFrustum _glFrustum
	typedef void (APIENTRYP PFNGLLOADIDENTITYPROC)(void);
	GLAPI PFNGLLOADIDENTITYPROC _glLoadIdentity;
#define glLoadIdentity _glLoadIdentity
	typedef void (APIENTRYP PFNGLLOADMATRIXFPROC)(const GLfloat *m);
	GLAPI PFNGLLOADMATRIXFPROC _glLoadMatrixf;
#define glLoadMatrixf _glLoadMatrixf
	typedef void (APIENTRYP PFNGLLOADMATRIXDPROC)(const GLdouble *m);
	GLAPI PFNGLLOADMATRIXDPROC _glLoadMatrixd;
#define glLoadMatrixd _glLoadMatrixd
	typedef void (APIENTRYP PFNGLMATRIXMODEPROC)(GLenum mode);
	GLAPI PFNGLMATRIXMODEPROC _glMatrixMode;
#define glMatrixMode _glMatrixMode
	typedef void (APIENTRYP PFNGLMULTMATRIXFPROC)(const GLfloat *m);
	GLAPI PFNGLMULTMATRIXFPROC _glMultMatrixf;
#define glMultMatrixf _glMultMatrixf
	typedef void (APIENTRYP PFNGLMULTMATRIXDPROC)(const GLdouble *m);
	GLAPI PFNGLMULTMATRIXDPROC _glMultMatrixd;
#define glMultMatrixd _glMultMatrixd
	typedef void (APIENTRYP PFNGLORTHOPROC)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
	GLAPI PFNGLORTHOPROC _glOrtho;
#define glOrtho _glOrtho
	typedef void (APIENTRYP PFNGLPOPMATRIXPROC)(void);
	GLAPI PFNGLPOPMATRIXPROC _glPopMatrix;
#define glPopMatrix _glPopMatrix
	typedef void (APIENTRYP PFNGLPUSHMATRIXPROC)(void);
	GLAPI PFNGLPUSHMATRIXPROC _glPushMatrix;
#define glPushMatrix _glPushMatrix
	typedef void (APIENTRYP PFNGLROTATEDPROC)(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
	GLAPI PFNGLROTATEDPROC _glRotated;
#define glRotated _glRotated
	typedef void (APIENTRYP PFNGLROTATEFPROC)(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
	GLAPI PFNGLROTATEFPROC _glRotatef;
#define glRotatef _glRotatef
	typedef void (APIENTRYP PFNGLSCALEDPROC)(GLdouble x, GLdouble y, GLdouble z);
	GLAPI PFNGLSCALEDPROC _glScaled;
#define glScaled _glScaled
	typedef void (APIENTRYP PFNGLSCALEFPROC)(GLfloat x, GLfloat y, GLfloat z);
	GLAPI PFNGLSCALEFPROC _glScalef;
#define glScalef _glScalef
	typedef void (APIENTRYP PFNGLTRANSLATEDPROC)(GLdouble x, GLdouble y, GLdouble z);
	GLAPI PFNGLTRANSLATEDPROC _glTranslated;
#define glTranslated _glTranslated
	typedef void (APIENTRYP PFNGLTRANSLATEFPROC)(GLfloat x, GLfloat y, GLfloat z);
	GLAPI PFNGLTRANSLATEFPROC _glTranslatef;
#define glTranslatef _glTranslatef
#endif
#ifndef GL_VERSION_1_1
#define GL_VERSION_1_1 1
	GLAPI int _GL_VERSION_1_1;
	typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
	GLAPI PFNGLDRAWARRAYSPROC _glDrawArrays;
#define glDrawArrays _glDrawArrays
	typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
	GLAPI PFNGLDRAWELEMENTSPROC _glDrawElements;
#define glDrawElements _glDrawElements
	typedef void (APIENTRYP PFNGLGETPOINTERVPROC)(GLenum pname, void **params);
	GLAPI PFNGLGETPOINTERVPROC _glGetPointerv;
#define glGetPointerv _glGetPointerv
	typedef void (APIENTRYP PFNGLPOLYGONOFFSETPROC)(GLfloat factor, GLfloat units);
	GLAPI PFNGLPOLYGONOFFSETPROC _glPolygonOffset;
#define glPolygonOffset _glPolygonOffset
	typedef void (APIENTRYP PFNGLCOPYTEXIMAGE1DPROC)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
	GLAPI PFNGLCOPYTEXIMAGE1DPROC _glCopyTexImage1D;
#define glCopyTexImage1D _glCopyTexImage1D
	typedef void (APIENTRYP PFNGLCOPYTEXIMAGE2DPROC)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
	GLAPI PFNGLCOPYTEXIMAGE2DPROC _glCopyTexImage2D;
#define glCopyTexImage2D _glCopyTexImage2D
	typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE1DPROC)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
	GLAPI PFNGLCOPYTEXSUBIMAGE1DPROC _glCopyTexSubImage1D;
#define glCopyTexSubImage1D _glCopyTexSubImage1D
	typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	GLAPI PFNGLCOPYTEXSUBIMAGE2DPROC _glCopyTexSubImage2D;
#define glCopyTexSubImage2D _glCopyTexSubImage2D
	typedef void (APIENTRYP PFNGLTEXSUBIMAGE1DPROC)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
	GLAPI PFNGLTEXSUBIMAGE1DPROC _glTexSubImage1D;
#define glTexSubImage1D _glTexSubImage1D
	typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
	GLAPI PFNGLTEXSUBIMAGE2DPROC _glTexSubImage2D;
#define glTexSubImage2D _glTexSubImage2D
	typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
	GLAPI PFNGLBINDTEXTUREPROC _glBindTexture;
#define glBindTexture _glBindTexture
	typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);
	GLAPI PFNGLDELETETEXTURESPROC _glDeleteTextures;
#define glDeleteTextures _glDeleteTextures
	typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
	GLAPI PFNGLGENTEXTURESPROC _glGenTextures;
#define glGenTextures _glGenTextures
	typedef GLboolean(APIENTRYP PFNGLISTEXTUREPROC)(GLuint texture);
	GLAPI PFNGLISTEXTUREPROC _glIsTexture;
#define glIsTexture _glIsTexture
	typedef void (APIENTRYP PFNGLARRAYELEMENTPROC)(GLint i);
	GLAPI PFNGLARRAYELEMENTPROC _glArrayElement;
#define glArrayElement _glArrayElement
	typedef void (APIENTRYP PFNGLCOLORPOINTERPROC)(GLint size, GLenum type, GLsizei stride, const void *pointer);
	GLAPI PFNGLCOLORPOINTERPROC _glColorPointer;
#define glColorPointer _glColorPointer
	typedef void (APIENTRYP PFNGLDISABLECLIENTSTATEPROC)(GLenum array);
	GLAPI PFNGLDISABLECLIENTSTATEPROC _glDisableClientState;
#define glDisableClientState _glDisableClientState
	typedef void (APIENTRYP PFNGLEDGEFLAGPOINTERPROC)(GLsizei stride, const void *pointer);
	GLAPI PFNGLEDGEFLAGPOINTERPROC _glEdgeFlagPointer;
#define glEdgeFlagPointer _glEdgeFlagPointer
	typedef void (APIENTRYP PFNGLENABLECLIENTSTATEPROC)(GLenum array);
	GLAPI PFNGLENABLECLIENTSTATEPROC _glEnableClientState;
#define glEnableClientState _glEnableClientState
	typedef void (APIENTRYP PFNGLINDEXPOINTERPROC)(GLenum type, GLsizei stride, const void *pointer);
	GLAPI PFNGLINDEXPOINTERPROC _glIndexPointer;
#define glIndexPointer _glIndexPointer
	typedef void (APIENTRYP PFNGLINTERLEAVEDARRAYSPROC)(GLenum format, GLsizei stride, const void *pointer);
	GLAPI PFNGLINTERLEAVEDARRAYSPROC _glInterleavedArrays;
#define glInterleavedArrays _glInterleavedArrays
	typedef void (APIENTRYP PFNGLNORMALPOINTERPROC)(GLenum type, GLsizei stride, const void *pointer);
	GLAPI PFNGLNORMALPOINTERPROC _glNormalPointer;
#define glNormalPointer _glNormalPointer
	typedef void (APIENTRYP PFNGLTEXCOORDPOINTERPROC)(GLint size, GLenum type, GLsizei stride, const void *pointer);
	GLAPI PFNGLTEXCOORDPOINTERPROC _glTexCoordPointer;
#define glTexCoordPointer _glTexCoordPointer
	typedef void (APIENTRYP PFNGLVERTEXPOINTERPROC)(GLint size, GLenum type, GLsizei stride, const void *pointer);
	GLAPI PFNGLVERTEXPOINTERPROC _glVertexPointer;
#define glVertexPointer _glVertexPointer
	typedef GLboolean(APIENTRYP PFNGLARETEXTURESRESIDENTPROC)(GLsizei n, const GLuint *textures, GLboolean *residences);
	GLAPI PFNGLARETEXTURESRESIDENTPROC _glAreTexturesResident;
#define glAreTexturesResident _glAreTexturesResident
	typedef void (APIENTRYP PFNGLPRIORITIZETEXTURESPROC)(GLsizei n, const GLuint *textures, const GLfloat *priorities);
	GLAPI PFNGLPRIORITIZETEXTURESPROC _glPrioritizeTextures;
#define glPrioritizeTextures _glPrioritizeTextures
	typedef void (APIENTRYP PFNGLINDEXUBPROC)(GLubyte c);
	GLAPI PFNGLINDEXUBPROC _glIndexub;
#define glIndexub _glIndexub
	typedef void (APIENTRYP PFNGLINDEXUBVPROC)(const GLubyte *c);
	GLAPI PFNGLINDEXUBVPROC _glIndexubv;
#define glIndexubv _glIndexubv
	typedef void (APIENTRYP PFNGLPOPCLIENTATTRIBPROC)(void);
	GLAPI PFNGLPOPCLIENTATTRIBPROC _glPopClientAttrib;
#define glPopClientAttrib _glPopClientAttrib
	typedef void (APIENTRYP PFNGLPUSHCLIENTATTRIBPROC)(GLbitfield mask);
	GLAPI PFNGLPUSHCLIENTATTRIBPROC _glPushClientAttrib;
#define glPushClientAttrib _glPushClientAttrib
#endif
#ifndef GL_VERSION_1_2
#define GL_VERSION_1_2 1
	GLAPI int _GL_VERSION_1_2;
	typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSPROC)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
	GLAPI PFNGLDRAWRANGEELEMENTSPROC _glDrawRangeElements;
#define glDrawRangeElements _glDrawRangeElements
	typedef void (APIENTRYP PFNGLTEXIMAGE3DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
	GLAPI PFNGLTEXIMAGE3DPROC _glTexImage3D;
#define glTexImage3D _glTexImage3D
	typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
	GLAPI PFNGLTEXSUBIMAGE3DPROC _glTexSubImage3D;
#define glTexSubImage3D _glTexSubImage3D
	typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	GLAPI PFNGLCOPYTEXSUBIMAGE3DPROC _glCopyTexSubImage3D;
#define glCopyTexSubImage3D _glCopyTexSubImage3D
#endif
#ifndef GL_VERSION_1_3
#define GL_VERSION_1_3 1
	GLAPI int _GL_VERSION_1_3;
	typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
	GLAPI PFNGLACTIVETEXTUREPROC _glActiveTexture;
#define glActiveTexture _glActiveTexture
	typedef void (APIENTRYP PFNGLSAMPLECOVERAGEPROC)(GLfloat value, GLboolean invert);
	GLAPI PFNGLSAMPLECOVERAGEPROC _glSampleCoverage;
#define glSampleCoverage _glSampleCoverage
	typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
	GLAPI PFNGLCOMPRESSEDTEXIMAGE3DPROC _glCompressedTexImage3D;
#define glCompressedTexImage3D _glCompressedTexImage3D
	typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
	GLAPI PFNGLCOMPRESSEDTEXIMAGE2DPROC _glCompressedTexImage2D;
#define glCompressedTexImage2D _glCompressedTexImage2D
	typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE1DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
	GLAPI PFNGLCOMPRESSEDTEXIMAGE1DPROC _glCompressedTexImage1D;
#define glCompressedTexImage1D _glCompressedTexImage1D
	typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
	GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC _glCompressedTexSubImage3D;
#define glCompressedTexSubImage3D _glCompressedTexSubImage3D
	typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
	GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC _glCompressedTexSubImage2D;
#define glCompressedTexSubImage2D _glCompressedTexSubImage2D
	typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
	GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC _glCompressedTexSubImage1D;
#define glCompressedTexSubImage1D _glCompressedTexSubImage1D
	typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXIMAGEPROC)(GLenum target, GLint level, void *img);
	GLAPI PFNGLGETCOMPRESSEDTEXIMAGEPROC _glGetCompressedTexImage;
#define glGetCompressedTexImage _glGetCompressedTexImage
	typedef void (APIENTRYP PFNGLCLIENTACTIVETEXTUREPROC)(GLenum texture);
	GLAPI PFNGLCLIENTACTIVETEXTUREPROC _glClientActiveTexture;
#define glClientActiveTexture _glClientActiveTexture
	typedef void (APIENTRYP PFNGLMULTITEXCOORD1DPROC)(GLenum target, GLdouble s);
	GLAPI PFNGLMULTITEXCOORD1DPROC _glMultiTexCoord1d;
#define glMultiTexCoord1d _glMultiTexCoord1d
	typedef void (APIENTRYP PFNGLMULTITEXCOORD1DVPROC)(GLenum target, const GLdouble *v);
	GLAPI PFNGLMULTITEXCOORD1DVPROC _glMultiTexCoord1dv;
#define glMultiTexCoord1dv _glMultiTexCoord1dv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD1FPROC)(GLenum target, GLfloat s);
	GLAPI PFNGLMULTITEXCOORD1FPROC _glMultiTexCoord1f;
#define glMultiTexCoord1f _glMultiTexCoord1f
	typedef void (APIENTRYP PFNGLMULTITEXCOORD1FVPROC)(GLenum target, const GLfloat *v);
	GLAPI PFNGLMULTITEXCOORD1FVPROC _glMultiTexCoord1fv;
#define glMultiTexCoord1fv _glMultiTexCoord1fv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD1IPROC)(GLenum target, GLint s);
	GLAPI PFNGLMULTITEXCOORD1IPROC _glMultiTexCoord1i;
#define glMultiTexCoord1i _glMultiTexCoord1i
	typedef void (APIENTRYP PFNGLMULTITEXCOORD1IVPROC)(GLenum target, const GLint *v);
	GLAPI PFNGLMULTITEXCOORD1IVPROC _glMultiTexCoord1iv;
#define glMultiTexCoord1iv _glMultiTexCoord1iv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD1SPROC)(GLenum target, GLshort s);
	GLAPI PFNGLMULTITEXCOORD1SPROC _glMultiTexCoord1s;
#define glMultiTexCoord1s _glMultiTexCoord1s
	typedef void (APIENTRYP PFNGLMULTITEXCOORD1SVPROC)(GLenum target, const GLshort *v);
	GLAPI PFNGLMULTITEXCOORD1SVPROC _glMultiTexCoord1sv;
#define glMultiTexCoord1sv _glMultiTexCoord1sv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD2DPROC)(GLenum target, GLdouble s, GLdouble t);
	GLAPI PFNGLMULTITEXCOORD2DPROC _glMultiTexCoord2d;
#define glMultiTexCoord2d _glMultiTexCoord2d
	typedef void (APIENTRYP PFNGLMULTITEXCOORD2DVPROC)(GLenum target, const GLdouble *v);
	GLAPI PFNGLMULTITEXCOORD2DVPROC _glMultiTexCoord2dv;
#define glMultiTexCoord2dv _glMultiTexCoord2dv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD2FPROC)(GLenum target, GLfloat s, GLfloat t);
	GLAPI PFNGLMULTITEXCOORD2FPROC _glMultiTexCoord2f;
#define glMultiTexCoord2f _glMultiTexCoord2f
	typedef void (APIENTRYP PFNGLMULTITEXCOORD2FVPROC)(GLenum target, const GLfloat *v);
	GLAPI PFNGLMULTITEXCOORD2FVPROC _glMultiTexCoord2fv;
#define glMultiTexCoord2fv _glMultiTexCoord2fv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD2IPROC)(GLenum target, GLint s, GLint t);
	GLAPI PFNGLMULTITEXCOORD2IPROC _glMultiTexCoord2i;
#define glMultiTexCoord2i _glMultiTexCoord2i
	typedef void (APIENTRYP PFNGLMULTITEXCOORD2IVPROC)(GLenum target, const GLint *v);
	GLAPI PFNGLMULTITEXCOORD2IVPROC _glMultiTexCoord2iv;
#define glMultiTexCoord2iv _glMultiTexCoord2iv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD2SPROC)(GLenum target, GLshort s, GLshort t);
	GLAPI PFNGLMULTITEXCOORD2SPROC _glMultiTexCoord2s;
#define glMultiTexCoord2s _glMultiTexCoord2s
	typedef void (APIENTRYP PFNGLMULTITEXCOORD2SVPROC)(GLenum target, const GLshort *v);
	GLAPI PFNGLMULTITEXCOORD2SVPROC _glMultiTexCoord2sv;
#define glMultiTexCoord2sv _glMultiTexCoord2sv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD3DPROC)(GLenum target, GLdouble s, GLdouble t, GLdouble r);
	GLAPI PFNGLMULTITEXCOORD3DPROC _glMultiTexCoord3d;
#define glMultiTexCoord3d _glMultiTexCoord3d
	typedef void (APIENTRYP PFNGLMULTITEXCOORD3DVPROC)(GLenum target, const GLdouble *v);
	GLAPI PFNGLMULTITEXCOORD3DVPROC _glMultiTexCoord3dv;
#define glMultiTexCoord3dv _glMultiTexCoord3dv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD3FPROC)(GLenum target, GLfloat s, GLfloat t, GLfloat r);
	GLAPI PFNGLMULTITEXCOORD3FPROC _glMultiTexCoord3f;
#define glMultiTexCoord3f _glMultiTexCoord3f
	typedef void (APIENTRYP PFNGLMULTITEXCOORD3FVPROC)(GLenum target, const GLfloat *v);
	GLAPI PFNGLMULTITEXCOORD3FVPROC _glMultiTexCoord3fv;
#define glMultiTexCoord3fv _glMultiTexCoord3fv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD3IPROC)(GLenum target, GLint s, GLint t, GLint r);
	GLAPI PFNGLMULTITEXCOORD3IPROC _glMultiTexCoord3i;
#define glMultiTexCoord3i _glMultiTexCoord3i
	typedef void (APIENTRYP PFNGLMULTITEXCOORD3IVPROC)(GLenum target, const GLint *v);
	GLAPI PFNGLMULTITEXCOORD3IVPROC _glMultiTexCoord3iv;
#define glMultiTexCoord3iv _glMultiTexCoord3iv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD3SPROC)(GLenum target, GLshort s, GLshort t, GLshort r);
	GLAPI PFNGLMULTITEXCOORD3SPROC _glMultiTexCoord3s;
#define glMultiTexCoord3s _glMultiTexCoord3s
	typedef void (APIENTRYP PFNGLMULTITEXCOORD3SVPROC)(GLenum target, const GLshort *v);
	GLAPI PFNGLMULTITEXCOORD3SVPROC _glMultiTexCoord3sv;
#define glMultiTexCoord3sv _glMultiTexCoord3sv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD4DPROC)(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
	GLAPI PFNGLMULTITEXCOORD4DPROC _glMultiTexCoord4d;
#define glMultiTexCoord4d _glMultiTexCoord4d
	typedef void (APIENTRYP PFNGLMULTITEXCOORD4DVPROC)(GLenum target, const GLdouble *v);
	GLAPI PFNGLMULTITEXCOORD4DVPROC _glMultiTexCoord4dv;
#define glMultiTexCoord4dv _glMultiTexCoord4dv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD4FPROC)(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
	GLAPI PFNGLMULTITEXCOORD4FPROC _glMultiTexCoord4f;
#define glMultiTexCoord4f _glMultiTexCoord4f
	typedef void (APIENTRYP PFNGLMULTITEXCOORD4FVPROC)(GLenum target, const GLfloat *v);
	GLAPI PFNGLMULTITEXCOORD4FVPROC _glMultiTexCoord4fv;
#define glMultiTexCoord4fv _glMultiTexCoord4fv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD4IPROC)(GLenum target, GLint s, GLint t, GLint r, GLint q);
	GLAPI PFNGLMULTITEXCOORD4IPROC _glMultiTexCoord4i;
#define glMultiTexCoord4i _glMultiTexCoord4i
	typedef void (APIENTRYP PFNGLMULTITEXCOORD4IVPROC)(GLenum target, const GLint *v);
	GLAPI PFNGLMULTITEXCOORD4IVPROC _glMultiTexCoord4iv;
#define glMultiTexCoord4iv _glMultiTexCoord4iv
	typedef void (APIENTRYP PFNGLMULTITEXCOORD4SPROC)(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
	GLAPI PFNGLMULTITEXCOORD4SPROC _glMultiTexCoord4s;
#define glMultiTexCoord4s _glMultiTexCoord4s
	typedef void (APIENTRYP PFNGLMULTITEXCOORD4SVPROC)(GLenum target, const GLshort *v);
	GLAPI PFNGLMULTITEXCOORD4SVPROC _glMultiTexCoord4sv;
#define glMultiTexCoord4sv _glMultiTexCoord4sv
	typedef void (APIENTRYP PFNGLLOADTRANSPOSEMATRIXFPROC)(const GLfloat *m);
	GLAPI PFNGLLOADTRANSPOSEMATRIXFPROC _glLoadTransposeMatrixf;
#define glLoadTransposeMatrixf _glLoadTransposeMatrixf
	typedef void (APIENTRYP PFNGLLOADTRANSPOSEMATRIXDPROC)(const GLdouble *m);
	GLAPI PFNGLLOADTRANSPOSEMATRIXDPROC _glLoadTransposeMatrixd;
#define glLoadTransposeMatrixd _glLoadTransposeMatrixd
	typedef void (APIENTRYP PFNGLMULTTRANSPOSEMATRIXFPROC)(const GLfloat *m);
	GLAPI PFNGLMULTTRANSPOSEMATRIXFPROC _glMultTransposeMatrixf;
#define glMultTransposeMatrixf _glMultTransposeMatrixf
	typedef void (APIENTRYP PFNGLMULTTRANSPOSEMATRIXDPROC)(const GLdouble *m);
	GLAPI PFNGLMULTTRANSPOSEMATRIXDPROC _glMultTransposeMatrixd;
#define glMultTransposeMatrixd _glMultTransposeMatrixd
#endif
#ifndef GL_VERSION_1_4
#define GL_VERSION_1_4 1
	GLAPI int _GL_VERSION_1_4;
	typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEPROC)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
	GLAPI PFNGLBLENDFUNCSEPARATEPROC _glBlendFuncSeparate;
#define glBlendFuncSeparate _glBlendFuncSeparate
	typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSPROC)(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
	GLAPI PFNGLMULTIDRAWARRAYSPROC _glMultiDrawArrays;
#define glMultiDrawArrays _glMultiDrawArrays
	typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSPROC)(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount);
	GLAPI PFNGLMULTIDRAWELEMENTSPROC _glMultiDrawElements;
#define glMultiDrawElements _glMultiDrawElements
	typedef void (APIENTRYP PFNGLPOINTPARAMETERFPROC)(GLenum pname, GLfloat param);
	GLAPI PFNGLPOINTPARAMETERFPROC _glPointParameterf;
#define glPointParameterf _glPointParameterf
	typedef void (APIENTRYP PFNGLPOINTPARAMETERFVPROC)(GLenum pname, const GLfloat *params);
	GLAPI PFNGLPOINTPARAMETERFVPROC _glPointParameterfv;
#define glPointParameterfv _glPointParameterfv
	typedef void (APIENTRYP PFNGLPOINTPARAMETERIPROC)(GLenum pname, GLint param);
	GLAPI PFNGLPOINTPARAMETERIPROC _glPointParameteri;
#define glPointParameteri _glPointParameteri
	typedef void (APIENTRYP PFNGLPOINTPARAMETERIVPROC)(GLenum pname, const GLint *params);
	GLAPI PFNGLPOINTPARAMETERIVPROC _glPointParameteriv;
#define glPointParameteriv _glPointParameteriv
	typedef void (APIENTRYP PFNGLFOGCOORDFPROC)(GLfloat coord);
	GLAPI PFNGLFOGCOORDFPROC _glFogCoordf;
#define glFogCoordf _glFogCoordf
	typedef void (APIENTRYP PFNGLFOGCOORDFVPROC)(const GLfloat *coord);
	GLAPI PFNGLFOGCOORDFVPROC _glFogCoordfv;
#define glFogCoordfv _glFogCoordfv
	typedef void (APIENTRYP PFNGLFOGCOORDDPROC)(GLdouble coord);
	GLAPI PFNGLFOGCOORDDPROC _glFogCoordd;
#define glFogCoordd _glFogCoordd
	typedef void (APIENTRYP PFNGLFOGCOORDDVPROC)(const GLdouble *coord);
	GLAPI PFNGLFOGCOORDDVPROC _glFogCoorddv;
#define glFogCoorddv _glFogCoorddv
	typedef void (APIENTRYP PFNGLFOGCOORDPOINTERPROC)(GLenum type, GLsizei stride, const void *pointer);
	GLAPI PFNGLFOGCOORDPOINTERPROC _glFogCoordPointer;
#define glFogCoordPointer _glFogCoordPointer
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3BPROC)(GLbyte red, GLbyte green, GLbyte blue);
	GLAPI PFNGLSECONDARYCOLOR3BPROC _glSecondaryColor3b;
#define glSecondaryColor3b _glSecondaryColor3b
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3BVPROC)(const GLbyte *v);
	GLAPI PFNGLSECONDARYCOLOR3BVPROC _glSecondaryColor3bv;
#define glSecondaryColor3bv _glSecondaryColor3bv
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3DPROC)(GLdouble red, GLdouble green, GLdouble blue);
	GLAPI PFNGLSECONDARYCOLOR3DPROC _glSecondaryColor3d;
#define glSecondaryColor3d _glSecondaryColor3d
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3DVPROC)(const GLdouble *v);
	GLAPI PFNGLSECONDARYCOLOR3DVPROC _glSecondaryColor3dv;
#define glSecondaryColor3dv _glSecondaryColor3dv
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3FPROC)(GLfloat red, GLfloat green, GLfloat blue);
	GLAPI PFNGLSECONDARYCOLOR3FPROC _glSecondaryColor3f;
#define glSecondaryColor3f _glSecondaryColor3f
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3FVPROC)(const GLfloat *v);
	GLAPI PFNGLSECONDARYCOLOR3FVPROC _glSecondaryColor3fv;
#define glSecondaryColor3fv _glSecondaryColor3fv
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3IPROC)(GLint red, GLint green, GLint blue);
	GLAPI PFNGLSECONDARYCOLOR3IPROC _glSecondaryColor3i;
#define glSecondaryColor3i _glSecondaryColor3i
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3IVPROC)(const GLint *v);
	GLAPI PFNGLSECONDARYCOLOR3IVPROC _glSecondaryColor3iv;
#define glSecondaryColor3iv _glSecondaryColor3iv
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3SPROC)(GLshort red, GLshort green, GLshort blue);
	GLAPI PFNGLSECONDARYCOLOR3SPROC _glSecondaryColor3s;
#define glSecondaryColor3s _glSecondaryColor3s
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3SVPROC)(const GLshort *v);
	GLAPI PFNGLSECONDARYCOLOR3SVPROC _glSecondaryColor3sv;
#define glSecondaryColor3sv _glSecondaryColor3sv
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UBPROC)(GLubyte red, GLubyte green, GLubyte blue);
	GLAPI PFNGLSECONDARYCOLOR3UBPROC _glSecondaryColor3ub;
#define glSecondaryColor3ub _glSecondaryColor3ub
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UBVPROC)(const GLubyte *v);
	GLAPI PFNGLSECONDARYCOLOR3UBVPROC _glSecondaryColor3ubv;
#define glSecondaryColor3ubv _glSecondaryColor3ubv
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UIPROC)(GLuint red, GLuint green, GLuint blue);
	GLAPI PFNGLSECONDARYCOLOR3UIPROC _glSecondaryColor3ui;
#define glSecondaryColor3ui _glSecondaryColor3ui
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UIVPROC)(const GLuint *v);
	GLAPI PFNGLSECONDARYCOLOR3UIVPROC _glSecondaryColor3uiv;
#define glSecondaryColor3uiv _glSecondaryColor3uiv
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3USPROC)(GLushort red, GLushort green, GLushort blue);
	GLAPI PFNGLSECONDARYCOLOR3USPROC _glSecondaryColor3us;
#define glSecondaryColor3us _glSecondaryColor3us
	typedef void (APIENTRYP PFNGLSECONDARYCOLOR3USVPROC)(const GLushort *v);
	GLAPI PFNGLSECONDARYCOLOR3USVPROC _glSecondaryColor3usv;
#define glSecondaryColor3usv _glSecondaryColor3usv
	typedef void (APIENTRYP PFNGLSECONDARYCOLORPOINTERPROC)(GLint size, GLenum type, GLsizei stride, const void *pointer);
	GLAPI PFNGLSECONDARYCOLORPOINTERPROC _glSecondaryColorPointer;
#define glSecondaryColorPointer _glSecondaryColorPointer
	typedef void (APIENTRYP PFNGLWINDOWPOS2DPROC)(GLdouble x, GLdouble y);
	GLAPI PFNGLWINDOWPOS2DPROC _glWindowPos2d;
#define glWindowPos2d _glWindowPos2d
	typedef void (APIENTRYP PFNGLWINDOWPOS2DVPROC)(const GLdouble *v);
	GLAPI PFNGLWINDOWPOS2DVPROC _glWindowPos2dv;
#define glWindowPos2dv _glWindowPos2dv
	typedef void (APIENTRYP PFNGLWINDOWPOS2FPROC)(GLfloat x, GLfloat y);
	GLAPI PFNGLWINDOWPOS2FPROC _glWindowPos2f;
#define glWindowPos2f _glWindowPos2f
	typedef void (APIENTRYP PFNGLWINDOWPOS2FVPROC)(const GLfloat *v);
	GLAPI PFNGLWINDOWPOS2FVPROC _glWindowPos2fv;
#define glWindowPos2fv _glWindowPos2fv
	typedef void (APIENTRYP PFNGLWINDOWPOS2IPROC)(GLint x, GLint y);
	GLAPI PFNGLWINDOWPOS2IPROC _glWindowPos2i;
#define glWindowPos2i _glWindowPos2i
	typedef void (APIENTRYP PFNGLWINDOWPOS2IVPROC)(const GLint *v);
	GLAPI PFNGLWINDOWPOS2IVPROC _glWindowPos2iv;
#define glWindowPos2iv _glWindowPos2iv
	typedef void (APIENTRYP PFNGLWINDOWPOS2SPROC)(GLshort x, GLshort y);
	GLAPI PFNGLWINDOWPOS2SPROC _glWindowPos2s;
#define glWindowPos2s _glWindowPos2s
	typedef void (APIENTRYP PFNGLWINDOWPOS2SVPROC)(const GLshort *v);
	GLAPI PFNGLWINDOWPOS2SVPROC _glWindowPos2sv;
#define glWindowPos2sv _glWindowPos2sv
	typedef void (APIENTRYP PFNGLWINDOWPOS3DPROC)(GLdouble x, GLdouble y, GLdouble z);
	GLAPI PFNGLWINDOWPOS3DPROC _glWindowPos3d;
#define glWindowPos3d _glWindowPos3d
	typedef void (APIENTRYP PFNGLWINDOWPOS3DVPROC)(const GLdouble *v);
	GLAPI PFNGLWINDOWPOS3DVPROC _glWindowPos3dv;
#define glWindowPos3dv _glWindowPos3dv
	typedef void (APIENTRYP PFNGLWINDOWPOS3FPROC)(GLfloat x, GLfloat y, GLfloat z);
	GLAPI PFNGLWINDOWPOS3FPROC _glWindowPos3f;
#define glWindowPos3f _glWindowPos3f
	typedef void (APIENTRYP PFNGLWINDOWPOS3FVPROC)(const GLfloat *v);
	GLAPI PFNGLWINDOWPOS3FVPROC _glWindowPos3fv;
#define glWindowPos3fv _glWindowPos3fv
	typedef void (APIENTRYP PFNGLWINDOWPOS3IPROC)(GLint x, GLint y, GLint z);
	GLAPI PFNGLWINDOWPOS3IPROC _glWindowPos3i;
#define glWindowPos3i _glWindowPos3i
	typedef void (APIENTRYP PFNGLWINDOWPOS3IVPROC)(const GLint *v);
	GLAPI PFNGLWINDOWPOS3IVPROC _glWindowPos3iv;
#define glWindowPos3iv _glWindowPos3iv
	typedef void (APIENTRYP PFNGLWINDOWPOS3SPROC)(GLshort x, GLshort y, GLshort z);
	GLAPI PFNGLWINDOWPOS3SPROC _glWindowPos3s;
#define glWindowPos3s _glWindowPos3s
	typedef void (APIENTRYP PFNGLWINDOWPOS3SVPROC)(const GLshort *v);
	GLAPI PFNGLWINDOWPOS3SVPROC _glWindowPos3sv;
#define glWindowPos3sv _glWindowPos3sv
	typedef void (APIENTRYP PFNGLBLENDCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	GLAPI PFNGLBLENDCOLORPROC _glBlendColor;
#define glBlendColor _glBlendColor
	typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC)(GLenum mode);
	GLAPI PFNGLBLENDEQUATIONPROC _glBlendEquation;
#define glBlendEquation _glBlendEquation
#endif
#ifndef GL_VERSION_1_5
#define GL_VERSION_1_5 1
	GLAPI int _GL_VERSION_1_5;
	typedef void (APIENTRYP PFNGLGENQUERIESPROC)(GLsizei n, GLuint *ids);
	GLAPI PFNGLGENQUERIESPROC _glGenQueries;
#define glGenQueries _glGenQueries
	typedef void (APIENTRYP PFNGLDELETEQUERIESPROC)(GLsizei n, const GLuint *ids);
	GLAPI PFNGLDELETEQUERIESPROC _glDeleteQueries;
#define glDeleteQueries _glDeleteQueries
	typedef GLboolean(APIENTRYP PFNGLISQUERYPROC)(GLuint id);
	GLAPI PFNGLISQUERYPROC _glIsQuery;
#define glIsQuery _glIsQuery
	typedef void (APIENTRYP PFNGLBEGINQUERYPROC)(GLenum target, GLuint id);
	GLAPI PFNGLBEGINQUERYPROC _glBeginQuery;
#define glBeginQuery _glBeginQuery
	typedef void (APIENTRYP PFNGLENDQUERYPROC)(GLenum target);
	GLAPI PFNGLENDQUERYPROC _glEndQuery;
#define glEndQuery _glEndQuery
	typedef void (APIENTRYP PFNGLGETQUERYIVPROC)(GLenum target, GLenum pname, GLint *params);
	GLAPI PFNGLGETQUERYIVPROC _glGetQueryiv;
#define glGetQueryiv _glGetQueryiv
	typedef void (APIENTRYP PFNGLGETQUERYOBJECTIVPROC)(GLuint id, GLenum pname, GLint *params);
	GLAPI PFNGLGETQUERYOBJECTIVPROC _glGetQueryObjectiv;
#define glGetQueryObjectiv _glGetQueryObjectiv
	typedef void (APIENTRYP PFNGLGETQUERYOBJECTUIVPROC)(GLuint id, GLenum pname, GLuint *params);
	GLAPI PFNGLGETQUERYOBJECTUIVPROC _glGetQueryObjectuiv;
#define glGetQueryObjectuiv _glGetQueryObjectuiv
	typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
	GLAPI PFNGLBINDBUFFERPROC _glBindBuffer;
#define glBindBuffer _glBindBuffer
	typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
	GLAPI PFNGLDELETEBUFFERSPROC _glDeleteBuffers;
#define glDeleteBuffers _glDeleteBuffers
	typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
	GLAPI PFNGLGENBUFFERSPROC _glGenBuffers;
#define glGenBuffers _glGenBuffers
	typedef GLboolean(APIENTRYP PFNGLISBUFFERPROC)(GLuint buffer);
	GLAPI PFNGLISBUFFERPROC _glIsBuffer;
#define glIsBuffer _glIsBuffer
	typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
	GLAPI PFNGLBUFFERDATAPROC _glBufferData;
#define glBufferData _glBufferData
	typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
	GLAPI PFNGLBUFFERSUBDATAPROC _glBufferSubData;
#define glBufferSubData _glBufferSubData
	typedef void (APIENTRYP PFNGLGETBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, void *data);
	GLAPI PFNGLGETBUFFERSUBDATAPROC _glGetBufferSubData;
#define glGetBufferSubData _glGetBufferSubData
	typedef void * (APIENTRYP PFNGLMAPBUFFERPROC)(GLenum target, GLenum access);
	GLAPI PFNGLMAPBUFFERPROC _glMapBuffer;
#define glMapBuffer _glMapBuffer
	typedef GLboolean(APIENTRYP PFNGLUNMAPBUFFERPROC)(GLenum target);
	GLAPI PFNGLUNMAPBUFFERPROC _glUnmapBuffer;
#define glUnmapBuffer _glUnmapBuffer
	typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
	GLAPI PFNGLGETBUFFERPARAMETERIVPROC _glGetBufferParameteriv;
#define glGetBufferParameteriv _glGetBufferParameteriv
	typedef void (APIENTRYP PFNGLGETBUFFERPOINTERVPROC)(GLenum target, GLenum pname, void **params);
	GLAPI PFNGLGETBUFFERPOINTERVPROC _glGetBufferPointerv;
#define glGetBufferPointerv _glGetBufferPointerv
#endif
#ifndef GL_VERSION_2_0
#define GL_VERSION_2_0 1
	GLAPI int _GL_VERSION_2_0;
	typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEPROC)(GLenum modeRGB, GLenum modeAlpha);
	GLAPI PFNGLBLENDEQUATIONSEPARATEPROC _glBlendEquationSeparate;
#define glBlendEquationSeparate _glBlendEquationSeparate
	typedef void (APIENTRYP PFNGLDRAWBUFFERSPROC)(GLsizei n, const GLenum *bufs);
	GLAPI PFNGLDRAWBUFFERSPROC _glDrawBuffers;
#define glDrawBuffers _glDrawBuffers
	typedef void (APIENTRYP PFNGLSTENCILOPSEPARATEPROC)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
	GLAPI PFNGLSTENCILOPSEPARATEPROC _glStencilOpSeparate;
#define glStencilOpSeparate _glStencilOpSeparate
	typedef void (APIENTRYP PFNGLSTENCILFUNCSEPARATEPROC)(GLenum face, GLenum func, GLint ref, GLuint mask);
	GLAPI PFNGLSTENCILFUNCSEPARATEPROC _glStencilFuncSeparate;
#define glStencilFuncSeparate _glStencilFuncSeparate
	typedef void (APIENTRYP PFNGLSTENCILMASKSEPARATEPROC)(GLenum face, GLuint mask);
	GLAPI PFNGLSTENCILMASKSEPARATEPROC _glStencilMaskSeparate;
#define glStencilMaskSeparate _glStencilMaskSeparate
	typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
	GLAPI PFNGLATTACHSHADERPROC _glAttachShader;
#define glAttachShader _glAttachShader
	typedef void (APIENTRYP PFNGLBINDATTRIBLOCATIONPROC)(GLuint program, GLuint index, const GLchar *name);
	GLAPI PFNGLBINDATTRIBLOCATIONPROC _glBindAttribLocation;
#define glBindAttribLocation _glBindAttribLocation
	typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint shader);
	GLAPI PFNGLCOMPILESHADERPROC _glCompileShader;
#define glCompileShader _glCompileShader
	typedef GLuint(APIENTRYP PFNGLCREATEPROGRAMPROC)(void);
	GLAPI PFNGLCREATEPROGRAMPROC _glCreateProgram;
#define glCreateProgram _glCreateProgram
	typedef GLuint(APIENTRYP PFNGLCREATESHADERPROC)(GLenum type);
	GLAPI PFNGLCREATESHADERPROC _glCreateShader;
#define glCreateShader _glCreateShader
	typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint program);
	GLAPI PFNGLDELETEPROGRAMPROC _glDeleteProgram;
#define glDeleteProgram _glDeleteProgram
	typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint shader);
	GLAPI PFNGLDELETESHADERPROC _glDeleteShader;
#define glDeleteShader _glDeleteShader
	typedef void (APIENTRYP PFNGLDETACHSHADERPROC)(GLuint program, GLuint shader);
	GLAPI PFNGLDETACHSHADERPROC _glDetachShader;
#define glDetachShader _glDetachShader
	typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
	GLAPI PFNGLDISABLEVERTEXATTRIBARRAYPROC _glDisableVertexAttribArray;
#define glDisableVertexAttribArray _glDisableVertexAttribArray
	typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
	GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC _glEnableVertexAttribArray;
#define glEnableVertexAttribArray _glEnableVertexAttribArray
	typedef void (APIENTRYP PFNGLGETACTIVEATTRIBPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
	GLAPI PFNGLGETACTIVEATTRIBPROC _glGetActiveAttrib;
#define glGetActiveAttrib _glGetActiveAttrib
	typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
	GLAPI PFNGLGETACTIVEUNIFORMPROC _glGetActiveUniform;
#define glGetActiveUniform _glGetActiveUniform
	typedef void (APIENTRYP PFNGLGETATTACHEDSHADERSPROC)(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
	GLAPI PFNGLGETATTACHEDSHADERSPROC _glGetAttachedShaders;
#define glGetAttachedShaders _glGetAttachedShaders
	typedef GLint(APIENTRYP PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const GLchar *name);
	GLAPI PFNGLGETATTRIBLOCATIONPROC _glGetAttribLocation;
#define glGetAttribLocation _glGetAttribLocation
	typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
	GLAPI PFNGLGETPROGRAMIVPROC _glGetProgramiv;
#define glGetProgramiv _glGetProgramiv
	typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
	GLAPI PFNGLGETPROGRAMINFOLOGPROC _glGetProgramInfoLog;
#define glGetProgramInfoLog _glGetProgramInfoLog
	typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
	GLAPI PFNGLGETSHADERIVPROC _glGetShaderiv;
#define glGetShaderiv _glGetShaderiv
	typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
	GLAPI PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog;
#define glGetShaderInfoLog _glGetShaderInfoLog
	typedef void (APIENTRYP PFNGLGETSHADERSOURCEPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
	GLAPI PFNGLGETSHADERSOURCEPROC _glGetShaderSource;
#define glGetShaderSource _glGetShaderSource
	typedef GLint(APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
	GLAPI PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation;
#define glGetUniformLocation _glGetUniformLocation
	typedef void (APIENTRYP PFNGLGETUNIFORMFVPROC)(GLuint program, GLint location, GLfloat *params);
	GLAPI PFNGLGETUNIFORMFVPROC _glGetUniformfv;
#define glGetUniformfv _glGetUniformfv
	typedef void (APIENTRYP PFNGLGETUNIFORMIVPROC)(GLuint program, GLint location, GLint *params);
	GLAPI PFNGLGETUNIFORMIVPROC _glGetUniformiv;
#define glGetUniformiv _glGetUniformiv
	typedef void (APIENTRYP PFNGLGETVERTEXATTRIBDVPROC)(GLuint index, GLenum pname, GLdouble *params);
	GLAPI PFNGLGETVERTEXATTRIBDVPROC _glGetVertexAttribdv;
#define glGetVertexAttribdv _glGetVertexAttribdv
	typedef void (APIENTRYP PFNGLGETVERTEXATTRIBFVPROC)(GLuint index, GLenum pname, GLfloat *params);
	GLAPI PFNGLGETVERTEXATTRIBFVPROC _glGetVertexAttribfv;
#define glGetVertexAttribfv _glGetVertexAttribfv
	typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVPROC)(GLuint index, GLenum pname, GLint *params);
	GLAPI PFNGLGETVERTEXATTRIBIVPROC _glGetVertexAttribiv;
#define glGetVertexAttribiv _glGetVertexAttribiv
	typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVPROC)(GLuint index, GLenum pname, void **pointer);
	GLAPI PFNGLGETVERTEXATTRIBPOINTERVPROC _glGetVertexAttribPointerv;
#define glGetVertexAttribPointerv _glGetVertexAttribPointerv
	typedef GLboolean(APIENTRYP PFNGLISPROGRAMPROC)(GLuint program);
	GLAPI PFNGLISPROGRAMPROC _glIsProgram;
#define glIsProgram _glIsProgram
	typedef GLboolean(APIENTRYP PFNGLISSHADERPROC)(GLuint shader);
	GLAPI PFNGLISSHADERPROC _glIsShader;
#define glIsShader _glIsShader
	typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint program);
	GLAPI PFNGLLINKPROGRAMPROC _glLinkProgram;
#define glLinkProgram _glLinkProgram
	typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
	GLAPI PFNGLSHADERSOURCEPROC _glShaderSource;
#define glShaderSource _glShaderSource
	typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint program);
	GLAPI PFNGLUSEPROGRAMPROC _glUseProgram;
#define glUseProgram _glUseProgram
	typedef void (APIENTRYP PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
	GLAPI PFNGLUNIFORM1FPROC _glUniform1f;
#define glUniform1f _glUniform1f
	typedef void (APIENTRYP PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
	GLAPI PFNGLUNIFORM2FPROC _glUniform2f;
#define glUniform2f _glUniform2f
	typedef void (APIENTRYP PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	GLAPI PFNGLUNIFORM3FPROC _glUniform3f;
#define glUniform3f _glUniform3f
	typedef void (APIENTRYP PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	GLAPI PFNGLUNIFORM4FPROC _glUniform4f;
#define glUniform4f _glUniform4f
	typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
	GLAPI PFNGLUNIFORM1IPROC _glUniform1i;
#define glUniform1i _glUniform1i
	typedef void (APIENTRYP PFNGLUNIFORM2IPROC)(GLint location, GLint v0, GLint v1);
	GLAPI PFNGLUNIFORM2IPROC _glUniform2i;
#define glUniform2i _glUniform2i
	typedef void (APIENTRYP PFNGLUNIFORM3IPROC)(GLint location, GLint v0, GLint v1, GLint v2);
	GLAPI PFNGLUNIFORM3IPROC _glUniform3i;
#define glUniform3i _glUniform3i
	typedef void (APIENTRYP PFNGLUNIFORM4IPROC)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
	GLAPI PFNGLUNIFORM4IPROC _glUniform4i;
#define glUniform4i _glUniform4i
	typedef void (APIENTRYP PFNGLUNIFORM1FVPROC)(GLint location, GLsizei count, const GLfloat *value);
	GLAPI PFNGLUNIFORM1FVPROC _glUniform1fv;
#define glUniform1fv _glUniform1fv
	typedef void (APIENTRYP PFNGLUNIFORM2FVPROC)(GLint location, GLsizei count, const GLfloat *value);
	GLAPI PFNGLUNIFORM2FVPROC _glUniform2fv;
#define glUniform2fv _glUniform2fv
	typedef void (APIENTRYP PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat *value);
	GLAPI PFNGLUNIFORM3FVPROC _glUniform3fv;
#define glUniform3fv _glUniform3fv
	typedef void (APIENTRYP PFNGLUNIFORM4FVPROC)(GLint location, GLsizei count, const GLfloat *value);
	GLAPI PFNGLUNIFORM4FVPROC _glUniform4fv;
#define glUniform4fv _glUniform4fv
	typedef void (APIENTRYP PFNGLUNIFORM1IVPROC)(GLint location, GLsizei count, const GLint *value);
	GLAPI PFNGLUNIFORM1IVPROC _glUniform1iv;
#define glUniform1iv _glUniform1iv
	typedef void (APIENTRYP PFNGLUNIFORM2IVPROC)(GLint location, GLsizei count, const GLint *value);
	GLAPI PFNGLUNIFORM2IVPROC _glUniform2iv;
#define glUniform2iv _glUniform2iv
	typedef void (APIENTRYP PFNGLUNIFORM3IVPROC)(GLint location, GLsizei count, const GLint *value);
	GLAPI PFNGLUNIFORM3IVPROC _glUniform3iv;
#define glUniform3iv _glUniform3iv
	typedef void (APIENTRYP PFNGLUNIFORM4IVPROC)(GLint location, GLsizei count, const GLint *value);
	GLAPI PFNGLUNIFORM4IVPROC _glUniform4iv;
#define glUniform4iv _glUniform4iv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLUNIFORMMATRIX2FVPROC _glUniformMatrix2fv;
#define glUniformMatrix2fv _glUniformMatrix2fv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLUNIFORMMATRIX3FVPROC _glUniformMatrix3fv;
#define glUniformMatrix3fv _glUniformMatrix3fv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLUNIFORMMATRIX4FVPROC _glUniformMatrix4fv;
#define glUniformMatrix4fv _glUniformMatrix4fv
	typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC)(GLuint program);
	GLAPI PFNGLVALIDATEPROGRAMPROC _glValidateProgram;
#define glValidateProgram _glValidateProgram
	typedef void (APIENTRYP PFNGLVERTEXATTRIB1DPROC)(GLuint index, GLdouble x);
	GLAPI PFNGLVERTEXATTRIB1DPROC _glVertexAttrib1d;
#define glVertexAttrib1d _glVertexAttrib1d
	typedef void (APIENTRYP PFNGLVERTEXATTRIB1DVPROC)(GLuint index, const GLdouble *v);
	GLAPI PFNGLVERTEXATTRIB1DVPROC _glVertexAttrib1dv;
#define glVertexAttrib1dv _glVertexAttrib1dv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB1FPROC)(GLuint index, GLfloat x);
	GLAPI PFNGLVERTEXATTRIB1FPROC _glVertexAttrib1f;
#define glVertexAttrib1f _glVertexAttrib1f
	typedef void (APIENTRYP PFNGLVERTEXATTRIB1FVPROC)(GLuint index, const GLfloat *v);
	GLAPI PFNGLVERTEXATTRIB1FVPROC _glVertexAttrib1fv;
#define glVertexAttrib1fv _glVertexAttrib1fv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB1SPROC)(GLuint index, GLshort x);
	GLAPI PFNGLVERTEXATTRIB1SPROC _glVertexAttrib1s;
#define glVertexAttrib1s _glVertexAttrib1s
	typedef void (APIENTRYP PFNGLVERTEXATTRIB1SVPROC)(GLuint index, const GLshort *v);
	GLAPI PFNGLVERTEXATTRIB1SVPROC _glVertexAttrib1sv;
#define glVertexAttrib1sv _glVertexAttrib1sv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB2DPROC)(GLuint index, GLdouble x, GLdouble y);
	GLAPI PFNGLVERTEXATTRIB2DPROC _glVertexAttrib2d;
#define glVertexAttrib2d _glVertexAttrib2d
	typedef void (APIENTRYP PFNGLVERTEXATTRIB2DVPROC)(GLuint index, const GLdouble *v);
	GLAPI PFNGLVERTEXATTRIB2DVPROC _glVertexAttrib2dv;
#define glVertexAttrib2dv _glVertexAttrib2dv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB2FPROC)(GLuint index, GLfloat x, GLfloat y);
	GLAPI PFNGLVERTEXATTRIB2FPROC _glVertexAttrib2f;
#define glVertexAttrib2f _glVertexAttrib2f
	typedef void (APIENTRYP PFNGLVERTEXATTRIB2FVPROC)(GLuint index, const GLfloat *v);
	GLAPI PFNGLVERTEXATTRIB2FVPROC _glVertexAttrib2fv;
#define glVertexAttrib2fv _glVertexAttrib2fv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB2SPROC)(GLuint index, GLshort x, GLshort y);
	GLAPI PFNGLVERTEXATTRIB2SPROC _glVertexAttrib2s;
#define glVertexAttrib2s _glVertexAttrib2s
	typedef void (APIENTRYP PFNGLVERTEXATTRIB2SVPROC)(GLuint index, const GLshort *v);
	GLAPI PFNGLVERTEXATTRIB2SVPROC _glVertexAttrib2sv;
#define glVertexAttrib2sv _glVertexAttrib2sv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB3DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
	GLAPI PFNGLVERTEXATTRIB3DPROC _glVertexAttrib3d;
#define glVertexAttrib3d _glVertexAttrib3d
	typedef void (APIENTRYP PFNGLVERTEXATTRIB3DVPROC)(GLuint index, const GLdouble *v);
	GLAPI PFNGLVERTEXATTRIB3DVPROC _glVertexAttrib3dv;
#define glVertexAttrib3dv _glVertexAttrib3dv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB3FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
	GLAPI PFNGLVERTEXATTRIB3FPROC _glVertexAttrib3f;
#define glVertexAttrib3f _glVertexAttrib3f
	typedef void (APIENTRYP PFNGLVERTEXATTRIB3FVPROC)(GLuint index, const GLfloat *v);
	GLAPI PFNGLVERTEXATTRIB3FVPROC _glVertexAttrib3fv;
#define glVertexAttrib3fv _glVertexAttrib3fv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB3SPROC)(GLuint index, GLshort x, GLshort y, GLshort z);
	GLAPI PFNGLVERTEXATTRIB3SPROC _glVertexAttrib3s;
#define glVertexAttrib3s _glVertexAttrib3s
	typedef void (APIENTRYP PFNGLVERTEXATTRIB3SVPROC)(GLuint index, const GLshort *v);
	GLAPI PFNGLVERTEXATTRIB3SVPROC _glVertexAttrib3sv;
#define glVertexAttrib3sv _glVertexAttrib3sv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4NBVPROC)(GLuint index, const GLbyte *v);
	GLAPI PFNGLVERTEXATTRIB4NBVPROC _glVertexAttrib4Nbv;
#define glVertexAttrib4Nbv _glVertexAttrib4Nbv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4NIVPROC)(GLuint index, const GLint *v);
	GLAPI PFNGLVERTEXATTRIB4NIVPROC _glVertexAttrib4Niv;
#define glVertexAttrib4Niv _glVertexAttrib4Niv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4NSVPROC)(GLuint index, const GLshort *v);
	GLAPI PFNGLVERTEXATTRIB4NSVPROC _glVertexAttrib4Nsv;
#define glVertexAttrib4Nsv _glVertexAttrib4Nsv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBPROC)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
	GLAPI PFNGLVERTEXATTRIB4NUBPROC _glVertexAttrib4Nub;
#define glVertexAttrib4Nub _glVertexAttrib4Nub
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBVPROC)(GLuint index, const GLubyte *v);
	GLAPI PFNGLVERTEXATTRIB4NUBVPROC _glVertexAttrib4Nubv;
#define glVertexAttrib4Nubv _glVertexAttrib4Nubv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUIVPROC)(GLuint index, const GLuint *v);
	GLAPI PFNGLVERTEXATTRIB4NUIVPROC _glVertexAttrib4Nuiv;
#define glVertexAttrib4Nuiv _glVertexAttrib4Nuiv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUSVPROC)(GLuint index, const GLushort *v);
	GLAPI PFNGLVERTEXATTRIB4NUSVPROC _glVertexAttrib4Nusv;
#define glVertexAttrib4Nusv _glVertexAttrib4Nusv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4BVPROC)(GLuint index, const GLbyte *v);
	GLAPI PFNGLVERTEXATTRIB4BVPROC _glVertexAttrib4bv;
#define glVertexAttrib4bv _glVertexAttrib4bv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	GLAPI PFNGLVERTEXATTRIB4DPROC _glVertexAttrib4d;
#define glVertexAttrib4d _glVertexAttrib4d
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4DVPROC)(GLuint index, const GLdouble *v);
	GLAPI PFNGLVERTEXATTRIB4DVPROC _glVertexAttrib4dv;
#define glVertexAttrib4dv _glVertexAttrib4dv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	GLAPI PFNGLVERTEXATTRIB4FPROC _glVertexAttrib4f;
#define glVertexAttrib4f _glVertexAttrib4f
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVPROC)(GLuint index, const GLfloat *v);
	GLAPI PFNGLVERTEXATTRIB4FVPROC _glVertexAttrib4fv;
#define glVertexAttrib4fv _glVertexAttrib4fv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4IVPROC)(GLuint index, const GLint *v);
	GLAPI PFNGLVERTEXATTRIB4IVPROC _glVertexAttrib4iv;
#define glVertexAttrib4iv _glVertexAttrib4iv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4SPROC)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
	GLAPI PFNGLVERTEXATTRIB4SPROC _glVertexAttrib4s;
#define glVertexAttrib4s _glVertexAttrib4s
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4SVPROC)(GLuint index, const GLshort *v);
	GLAPI PFNGLVERTEXATTRIB4SVPROC _glVertexAttrib4sv;
#define glVertexAttrib4sv _glVertexAttrib4sv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBVPROC)(GLuint index, const GLubyte *v);
	GLAPI PFNGLVERTEXATTRIB4UBVPROC _glVertexAttrib4ubv;
#define glVertexAttrib4ubv _glVertexAttrib4ubv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4UIVPROC)(GLuint index, const GLuint *v);
	GLAPI PFNGLVERTEXATTRIB4UIVPROC _glVertexAttrib4uiv;
#define glVertexAttrib4uiv _glVertexAttrib4uiv
	typedef void (APIENTRYP PFNGLVERTEXATTRIB4USVPROC)(GLuint index, const GLushort *v);
	GLAPI PFNGLVERTEXATTRIB4USVPROC _glVertexAttrib4usv;
#define glVertexAttrib4usv _glVertexAttrib4usv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
	GLAPI PFNGLVERTEXATTRIBPOINTERPROC _glVertexAttribPointer;
#define glVertexAttribPointer _glVertexAttribPointer
#endif
#ifndef GL_VERSION_2_1
#define GL_VERSION_2_1 1
	GLAPI int _GL_VERSION_2_1;
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLUNIFORMMATRIX2X3FVPROC _glUniformMatrix2x3fv;
#define glUniformMatrix2x3fv _glUniformMatrix2x3fv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLUNIFORMMATRIX3X2FVPROC _glUniformMatrix3x2fv;
#define glUniformMatrix3x2fv _glUniformMatrix3x2fv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLUNIFORMMATRIX2X4FVPROC _glUniformMatrix2x4fv;
#define glUniformMatrix2x4fv _glUniformMatrix2x4fv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLUNIFORMMATRIX4X2FVPROC _glUniformMatrix4x2fv;
#define glUniformMatrix4x2fv _glUniformMatrix4x2fv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLUNIFORMMATRIX3X4FVPROC _glUniformMatrix3x4fv;
#define glUniformMatrix3x4fv _glUniformMatrix3x4fv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLUNIFORMMATRIX4X3FVPROC _glUniformMatrix4x3fv;
#define glUniformMatrix4x3fv _glUniformMatrix4x3fv
#endif
#ifndef GL_VERSION_3_0
#define GL_VERSION_3_0 1
	GLAPI int _GL_VERSION_3_0;
	typedef void (APIENTRYP PFNGLCOLORMASKIPROC)(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
	GLAPI PFNGLCOLORMASKIPROC _glColorMaski;
#define glColorMaski _glColorMaski
	typedef void (APIENTRYP PFNGLGETBOOLEANI_VPROC)(GLenum target, GLuint index, GLboolean *data);
	GLAPI PFNGLGETBOOLEANI_VPROC _glGetBooleani_v;
#define glGetBooleani_v _glGetBooleani_v
	typedef void (APIENTRYP PFNGLGETINTEGERI_VPROC)(GLenum target, GLuint index, GLint *data);
	GLAPI PFNGLGETINTEGERI_VPROC _glGetIntegeri_v;
#define glGetIntegeri_v _glGetIntegeri_v
	typedef void (APIENTRYP PFNGLENABLEIPROC)(GLenum target, GLuint index);
	GLAPI PFNGLENABLEIPROC _glEnablei;
#define glEnablei _glEnablei
	typedef void (APIENTRYP PFNGLDISABLEIPROC)(GLenum target, GLuint index);
	GLAPI PFNGLDISABLEIPROC _glDisablei;
#define glDisablei _glDisablei
	typedef GLboolean(APIENTRYP PFNGLISENABLEDIPROC)(GLenum target, GLuint index);
	GLAPI PFNGLISENABLEDIPROC _glIsEnabledi;
#define glIsEnabledi _glIsEnabledi
	typedef void (APIENTRYP PFNGLBEGINTRANSFORMFEEDBACKPROC)(GLenum primitiveMode);
	GLAPI PFNGLBEGINTRANSFORMFEEDBACKPROC _glBeginTransformFeedback;
#define glBeginTransformFeedback _glBeginTransformFeedback
	typedef void (APIENTRYP PFNGLENDTRANSFORMFEEDBACKPROC)(void);
	GLAPI PFNGLENDTRANSFORMFEEDBACKPROC _glEndTransformFeedback;
#define glEndTransformFeedback _glEndTransformFeedback
	typedef void (APIENTRYP PFNGLBINDBUFFERRANGEPROC)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
	GLAPI PFNGLBINDBUFFERRANGEPROC _glBindBufferRange;
#define glBindBufferRange _glBindBufferRange
	typedef void (APIENTRYP PFNGLBINDBUFFERBASEPROC)(GLenum target, GLuint index, GLuint buffer);
	GLAPI PFNGLBINDBUFFERBASEPROC _glBindBufferBase;
#define glBindBufferBase _glBindBufferBase
	typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKVARYINGSPROC)(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode);
	GLAPI PFNGLTRANSFORMFEEDBACKVARYINGSPROC _glTransformFeedbackVaryings;
#define glTransformFeedbackVaryings _glTransformFeedbackVaryings
	typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
	GLAPI PFNGLGETTRANSFORMFEEDBACKVARYINGPROC _glGetTransformFeedbackVarying;
#define glGetTransformFeedbackVarying _glGetTransformFeedbackVarying
	typedef void (APIENTRYP PFNGLCLAMPCOLORPROC)(GLenum target, GLenum clamp);
	GLAPI PFNGLCLAMPCOLORPROC _glClampColor;
#define glClampColor _glClampColor
	typedef void (APIENTRYP PFNGLBEGINCONDITIONALRENDERPROC)(GLuint id, GLenum mode);
	GLAPI PFNGLBEGINCONDITIONALRENDERPROC _glBeginConditionalRender;
#define glBeginConditionalRender _glBeginConditionalRender
	typedef void (APIENTRYP PFNGLENDCONDITIONALRENDERPROC)(void);
	GLAPI PFNGLENDCONDITIONALRENDERPROC _glEndConditionalRender;
#define glEndConditionalRender _glEndConditionalRender
	typedef void (APIENTRYP PFNGLVERTEXATTRIBIPOINTERPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
	GLAPI PFNGLVERTEXATTRIBIPOINTERPROC _glVertexAttribIPointer;
#define glVertexAttribIPointer _glVertexAttribIPointer
	typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIIVPROC)(GLuint index, GLenum pname, GLint *params);
	GLAPI PFNGLGETVERTEXATTRIBIIVPROC _glGetVertexAttribIiv;
#define glGetVertexAttribIiv _glGetVertexAttribIiv
	typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIUIVPROC)(GLuint index, GLenum pname, GLuint *params);
	GLAPI PFNGLGETVERTEXATTRIBIUIVPROC _glGetVertexAttribIuiv;
#define glGetVertexAttribIuiv _glGetVertexAttribIuiv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IPROC)(GLuint index, GLint x);
	GLAPI PFNGLVERTEXATTRIBI1IPROC _glVertexAttribI1i;
#define glVertexAttribI1i _glVertexAttribI1i
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IPROC)(GLuint index, GLint x, GLint y);
	GLAPI PFNGLVERTEXATTRIBI2IPROC _glVertexAttribI2i;
#define glVertexAttribI2i _glVertexAttribI2i
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IPROC)(GLuint index, GLint x, GLint y, GLint z);
	GLAPI PFNGLVERTEXATTRIBI3IPROC _glVertexAttribI3i;
#define glVertexAttribI3i _glVertexAttribI3i
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IPROC)(GLuint index, GLint x, GLint y, GLint z, GLint w);
	GLAPI PFNGLVERTEXATTRIBI4IPROC _glVertexAttribI4i;
#define glVertexAttribI4i _glVertexAttribI4i
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIPROC)(GLuint index, GLuint x);
	GLAPI PFNGLVERTEXATTRIBI1UIPROC _glVertexAttribI1ui;
#define glVertexAttribI1ui _glVertexAttribI1ui
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIPROC)(GLuint index, GLuint x, GLuint y);
	GLAPI PFNGLVERTEXATTRIBI2UIPROC _glVertexAttribI2ui;
#define glVertexAttribI2ui _glVertexAttribI2ui
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIPROC)(GLuint index, GLuint x, GLuint y, GLuint z);
	GLAPI PFNGLVERTEXATTRIBI3UIPROC _glVertexAttribI3ui;
#define glVertexAttribI3ui _glVertexAttribI3ui
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIPROC)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
	GLAPI PFNGLVERTEXATTRIBI4UIPROC _glVertexAttribI4ui;
#define glVertexAttribI4ui _glVertexAttribI4ui
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IVPROC)(GLuint index, const GLint *v);
	GLAPI PFNGLVERTEXATTRIBI1IVPROC _glVertexAttribI1iv;
#define glVertexAttribI1iv _glVertexAttribI1iv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IVPROC)(GLuint index, const GLint *v);
	GLAPI PFNGLVERTEXATTRIBI2IVPROC _glVertexAttribI2iv;
#define glVertexAttribI2iv _glVertexAttribI2iv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IVPROC)(GLuint index, const GLint *v);
	GLAPI PFNGLVERTEXATTRIBI3IVPROC _glVertexAttribI3iv;
#define glVertexAttribI3iv _glVertexAttribI3iv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IVPROC)(GLuint index, const GLint *v);
	GLAPI PFNGLVERTEXATTRIBI4IVPROC _glVertexAttribI4iv;
#define glVertexAttribI4iv _glVertexAttribI4iv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIVPROC)(GLuint index, const GLuint *v);
	GLAPI PFNGLVERTEXATTRIBI1UIVPROC _glVertexAttribI1uiv;
#define glVertexAttribI1uiv _glVertexAttribI1uiv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIVPROC)(GLuint index, const GLuint *v);
	GLAPI PFNGLVERTEXATTRIBI2UIVPROC _glVertexAttribI2uiv;
#define glVertexAttribI2uiv _glVertexAttribI2uiv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIVPROC)(GLuint index, const GLuint *v);
	GLAPI PFNGLVERTEXATTRIBI3UIVPROC _glVertexAttribI3uiv;
#define glVertexAttribI3uiv _glVertexAttribI3uiv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIVPROC)(GLuint index, const GLuint *v);
	GLAPI PFNGLVERTEXATTRIBI4UIVPROC _glVertexAttribI4uiv;
#define glVertexAttribI4uiv _glVertexAttribI4uiv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI4BVPROC)(GLuint index, const GLbyte *v);
	GLAPI PFNGLVERTEXATTRIBI4BVPROC _glVertexAttribI4bv;
#define glVertexAttribI4bv _glVertexAttribI4bv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI4SVPROC)(GLuint index, const GLshort *v);
	GLAPI PFNGLVERTEXATTRIBI4SVPROC _glVertexAttribI4sv;
#define glVertexAttribI4sv _glVertexAttribI4sv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UBVPROC)(GLuint index, const GLubyte *v);
	GLAPI PFNGLVERTEXATTRIBI4UBVPROC _glVertexAttribI4ubv;
#define glVertexAttribI4ubv _glVertexAttribI4ubv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBI4USVPROC)(GLuint index, const GLushort *v);
	GLAPI PFNGLVERTEXATTRIBI4USVPROC _glVertexAttribI4usv;
#define glVertexAttribI4usv _glVertexAttribI4usv
	typedef void (APIENTRYP PFNGLGETUNIFORMUIVPROC)(GLuint program, GLint location, GLuint *params);
	GLAPI PFNGLGETUNIFORMUIVPROC _glGetUniformuiv;
#define glGetUniformuiv _glGetUniformuiv
	typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONPROC)(GLuint program, GLuint color, const GLchar *name);
	GLAPI PFNGLBINDFRAGDATALOCATIONPROC _glBindFragDataLocation;
#define glBindFragDataLocation _glBindFragDataLocation
	typedef GLint(APIENTRYP PFNGLGETFRAGDATALOCATIONPROC)(GLuint program, const GLchar *name);
	GLAPI PFNGLGETFRAGDATALOCATIONPROC _glGetFragDataLocation;
#define glGetFragDataLocation _glGetFragDataLocation
	typedef void (APIENTRYP PFNGLUNIFORM1UIPROC)(GLint location, GLuint v0);
	GLAPI PFNGLUNIFORM1UIPROC _glUniform1ui;
#define glUniform1ui _glUniform1ui
	typedef void (APIENTRYP PFNGLUNIFORM2UIPROC)(GLint location, GLuint v0, GLuint v1);
	GLAPI PFNGLUNIFORM2UIPROC _glUniform2ui;
#define glUniform2ui _glUniform2ui
	typedef void (APIENTRYP PFNGLUNIFORM3UIPROC)(GLint location, GLuint v0, GLuint v1, GLuint v2);
	GLAPI PFNGLUNIFORM3UIPROC _glUniform3ui;
#define glUniform3ui _glUniform3ui
	typedef void (APIENTRYP PFNGLUNIFORM4UIPROC)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
	GLAPI PFNGLUNIFORM4UIPROC _glUniform4ui;
#define glUniform4ui _glUniform4ui
	typedef void (APIENTRYP PFNGLUNIFORM1UIVPROC)(GLint location, GLsizei count, const GLuint *value);
	GLAPI PFNGLUNIFORM1UIVPROC _glUniform1uiv;
#define glUniform1uiv _glUniform1uiv
	typedef void (APIENTRYP PFNGLUNIFORM2UIVPROC)(GLint location, GLsizei count, const GLuint *value);
	GLAPI PFNGLUNIFORM2UIVPROC _glUniform2uiv;
#define glUniform2uiv _glUniform2uiv
	typedef void (APIENTRYP PFNGLUNIFORM3UIVPROC)(GLint location, GLsizei count, const GLuint *value);
	GLAPI PFNGLUNIFORM3UIVPROC _glUniform3uiv;
#define glUniform3uiv _glUniform3uiv
	typedef void (APIENTRYP PFNGLUNIFORM4UIVPROC)(GLint location, GLsizei count, const GLuint *value);
	GLAPI PFNGLUNIFORM4UIVPROC _glUniform4uiv;
#define glUniform4uiv _glUniform4uiv
	typedef void (APIENTRYP PFNGLTEXPARAMETERIIVPROC)(GLenum target, GLenum pname, const GLint *params);
	GLAPI PFNGLTEXPARAMETERIIVPROC _glTexParameterIiv;
#define glTexParameterIiv _glTexParameterIiv
	typedef void (APIENTRYP PFNGLTEXPARAMETERIUIVPROC)(GLenum target, GLenum pname, const GLuint *params);
	GLAPI PFNGLTEXPARAMETERIUIVPROC _glTexParameterIuiv;
#define glTexParameterIuiv _glTexParameterIuiv
	typedef void (APIENTRYP PFNGLGETTEXPARAMETERIIVPROC)(GLenum target, GLenum pname, GLint *params);
	GLAPI PFNGLGETTEXPARAMETERIIVPROC _glGetTexParameterIiv;
#define glGetTexParameterIiv _glGetTexParameterIiv
	typedef void (APIENTRYP PFNGLGETTEXPARAMETERIUIVPROC)(GLenum target, GLenum pname, GLuint *params);
	GLAPI PFNGLGETTEXPARAMETERIUIVPROC _glGetTexParameterIuiv;
#define glGetTexParameterIuiv _glGetTexParameterIuiv
	typedef void (APIENTRYP PFNGLCLEARBUFFERIVPROC)(GLenum buffer, GLint drawbuffer, const GLint *value);
	GLAPI PFNGLCLEARBUFFERIVPROC _glClearBufferiv;
#define glClearBufferiv _glClearBufferiv
	typedef void (APIENTRYP PFNGLCLEARBUFFERUIVPROC)(GLenum buffer, GLint drawbuffer, const GLuint *value);
	GLAPI PFNGLCLEARBUFFERUIVPROC _glClearBufferuiv;
#define glClearBufferuiv _glClearBufferuiv
	typedef void (APIENTRYP PFNGLCLEARBUFFERFVPROC)(GLenum buffer, GLint drawbuffer, const GLfloat *value);
	GLAPI PFNGLCLEARBUFFERFVPROC _glClearBufferfv;
#define glClearBufferfv _glClearBufferfv
	typedef void (APIENTRYP PFNGLCLEARBUFFERFIPROC)(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
	GLAPI PFNGLCLEARBUFFERFIPROC _glClearBufferfi;
#define glClearBufferfi _glClearBufferfi
	typedef const GLubyte * (APIENTRYP PFNGLGETSTRINGIPROC)(GLenum name, GLuint index);
	GLAPI PFNGLGETSTRINGIPROC _glGetStringi;
#define glGetStringi _glGetStringi
	typedef GLboolean(APIENTRYP PFNGLISRENDERBUFFERPROC)(GLuint renderbuffer);
	GLAPI PFNGLISRENDERBUFFERPROC _glIsRenderbuffer;
#define glIsRenderbuffer _glIsRenderbuffer
	typedef void (APIENTRYP PFNGLBINDRENDERBUFFERPROC)(GLenum target, GLuint renderbuffer);
	GLAPI PFNGLBINDRENDERBUFFERPROC _glBindRenderbuffer;
#define glBindRenderbuffer _glBindRenderbuffer
	typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSPROC)(GLsizei n, const GLuint *renderbuffers);
	GLAPI PFNGLDELETERENDERBUFFERSPROC _glDeleteRenderbuffers;
#define glDeleteRenderbuffers _glDeleteRenderbuffers
	typedef void (APIENTRYP PFNGLGENRENDERBUFFERSPROC)(GLsizei n, GLuint *renderbuffers);
	GLAPI PFNGLGENRENDERBUFFERSPROC _glGenRenderbuffers;
#define glGenRenderbuffers _glGenRenderbuffers
	typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	GLAPI PFNGLRENDERBUFFERSTORAGEPROC _glRenderbufferStorage;
#define glRenderbufferStorage _glRenderbufferStorage
	typedef void (APIENTRYP PFNGLGETRENDERBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
	GLAPI PFNGLGETRENDERBUFFERPARAMETERIVPROC _glGetRenderbufferParameteriv;
#define glGetRenderbufferParameteriv _glGetRenderbufferParameteriv
	typedef GLboolean(APIENTRYP PFNGLISFRAMEBUFFERPROC)(GLuint framebuffer);
	GLAPI PFNGLISFRAMEBUFFERPROC _glIsFramebuffer;
#define glIsFramebuffer _glIsFramebuffer
	typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
	GLAPI PFNGLBINDFRAMEBUFFERPROC _glBindFramebuffer;
#define glBindFramebuffer _glBindFramebuffer
	typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint *framebuffers);
	GLAPI PFNGLDELETEFRAMEBUFFERSPROC _glDeleteFramebuffers;
#define glDeleteFramebuffers _glDeleteFramebuffers
	typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint *framebuffers);
	GLAPI PFNGLGENFRAMEBUFFERSPROC _glGenFramebuffers;
#define glGenFramebuffers _glGenFramebuffers
	typedef GLenum(APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
	GLAPI PFNGLCHECKFRAMEBUFFERSTATUSPROC _glCheckFramebufferStatus;
#define glCheckFramebufferStatus _glCheckFramebufferStatus
	typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE1DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	GLAPI PFNGLFRAMEBUFFERTEXTURE1DPROC _glFramebufferTexture1D;
#define glFramebufferTexture1D _glFramebufferTexture1D
	typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	GLAPI PFNGLFRAMEBUFFERTEXTURE2DPROC _glFramebufferTexture2D;
#define glFramebufferTexture2D _glFramebufferTexture2D
	typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
	GLAPI PFNGLFRAMEBUFFERTEXTURE3DPROC _glFramebufferTexture3D;
#define glFramebufferTexture3D _glFramebufferTexture3D
	typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	GLAPI PFNGLFRAMEBUFFERRENDERBUFFERPROC _glFramebufferRenderbuffer;
#define glFramebufferRenderbuffer _glFramebufferRenderbuffer
	typedef void (APIENTRYP PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)(GLenum target, GLenum attachment, GLenum pname, GLint *params);
	GLAPI PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC _glGetFramebufferAttachmentParameteriv;
#define glGetFramebufferAttachmentParameteriv _glGetFramebufferAttachmentParameteriv
	typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC)(GLenum target);
	GLAPI PFNGLGENERATEMIPMAPPROC _glGenerateMipmap;
#define glGenerateMipmap _glGenerateMipmap
	typedef void (APIENTRYP PFNGLBLITFRAMEBUFFERPROC)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	GLAPI PFNGLBLITFRAMEBUFFERPROC _glBlitFramebuffer;
#define glBlitFramebuffer _glBlitFramebuffer
	typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	GLAPI PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC _glRenderbufferStorageMultisample;
#define glRenderbufferStorageMultisample _glRenderbufferStorageMultisample
	typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
	GLAPI PFNGLFRAMEBUFFERTEXTURELAYERPROC _glFramebufferTextureLayer;
#define glFramebufferTextureLayer _glFramebufferTextureLayer
	typedef void * (APIENTRYP PFNGLMAPBUFFERRANGEPROC)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
	GLAPI PFNGLMAPBUFFERRANGEPROC _glMapBufferRange;
#define glMapBufferRange _glMapBufferRange
	typedef void (APIENTRYP PFNGLFLUSHMAPPEDBUFFERRANGEPROC)(GLenum target, GLintptr offset, GLsizeiptr length);
	GLAPI PFNGLFLUSHMAPPEDBUFFERRANGEPROC _glFlushMappedBufferRange;
#define glFlushMappedBufferRange _glFlushMappedBufferRange
	typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint array);
	GLAPI PFNGLBINDVERTEXARRAYPROC _glBindVertexArray;
#define glBindVertexArray _glBindVertexArray
	typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
	GLAPI PFNGLDELETEVERTEXARRAYSPROC _glDeleteVertexArrays;
#define glDeleteVertexArrays _glDeleteVertexArrays
	typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
	GLAPI PFNGLGENVERTEXARRAYSPROC _glGenVertexArrays;
#define glGenVertexArrays _glGenVertexArrays
	typedef GLboolean(APIENTRYP PFNGLISVERTEXARRAYPROC)(GLuint array);
	GLAPI PFNGLISVERTEXARRAYPROC _glIsVertexArray;
#define glIsVertexArray _glIsVertexArray
#endif
#ifndef GL_VERSION_3_1
#define GL_VERSION_3_1 1
	GLAPI int _GL_VERSION_3_1;
	typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
	GLAPI PFNGLDRAWARRAYSINSTANCEDPROC _glDrawArraysInstanced;
#define glDrawArraysInstanced _glDrawArraysInstanced
	typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
	GLAPI PFNGLDRAWELEMENTSINSTANCEDPROC _glDrawElementsInstanced;
#define glDrawElementsInstanced _glDrawElementsInstanced
	typedef void (APIENTRYP PFNGLTEXBUFFERPROC)(GLenum target, GLenum internalformat, GLuint buffer);
	GLAPI PFNGLTEXBUFFERPROC _glTexBuffer;
#define glTexBuffer _glTexBuffer
	typedef void (APIENTRYP PFNGLPRIMITIVERESTARTINDEXPROC)(GLuint index);
	GLAPI PFNGLPRIMITIVERESTARTINDEXPROC _glPrimitiveRestartIndex;
#define glPrimitiveRestartIndex _glPrimitiveRestartIndex
	typedef void (APIENTRYP PFNGLCOPYBUFFERSUBDATAPROC)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
	GLAPI PFNGLCOPYBUFFERSUBDATAPROC _glCopyBufferSubData;
#define glCopyBufferSubData _glCopyBufferSubData
	typedef void (APIENTRYP PFNGLGETUNIFORMINDICESPROC)(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);
	GLAPI PFNGLGETUNIFORMINDICESPROC _glGetUniformIndices;
#define glGetUniformIndices _glGetUniformIndices
	typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMSIVPROC)(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
	GLAPI PFNGLGETACTIVEUNIFORMSIVPROC _glGetActiveUniformsiv;
#define glGetActiveUniformsiv _glGetActiveUniformsiv
	typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMNAMEPROC)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
	GLAPI PFNGLGETACTIVEUNIFORMNAMEPROC _glGetActiveUniformName;
#define glGetActiveUniformName _glGetActiveUniformName
	typedef GLuint(APIENTRYP PFNGLGETUNIFORMBLOCKINDEXPROC)(GLuint program, const GLchar *uniformBlockName);
	GLAPI PFNGLGETUNIFORMBLOCKINDEXPROC _glGetUniformBlockIndex;
#define glGetUniformBlockIndex _glGetUniformBlockIndex
	typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKIVPROC)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
	GLAPI PFNGLGETACTIVEUNIFORMBLOCKIVPROC _glGetActiveUniformBlockiv;
#define glGetActiveUniformBlockiv _glGetActiveUniformBlockiv
	typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
	GLAPI PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC _glGetActiveUniformBlockName;
#define glGetActiveUniformBlockName _glGetActiveUniformBlockName
	typedef void (APIENTRYP PFNGLUNIFORMBLOCKBINDINGPROC)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
	GLAPI PFNGLUNIFORMBLOCKBINDINGPROC _glUniformBlockBinding;
#define glUniformBlockBinding _glUniformBlockBinding
#endif
#ifndef GL_VERSION_3_2
#define GL_VERSION_3_2 1
	GLAPI int _GL_VERSION_3_2;
	typedef void (APIENTRYP PFNGLDRAWELEMENTSBASEVERTEXPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
	GLAPI PFNGLDRAWELEMENTSBASEVERTEXPROC _glDrawElementsBaseVertex;
#define glDrawElementsBaseVertex _glDrawElementsBaseVertex
	typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
	GLAPI PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC _glDrawRangeElementsBaseVertex;
#define glDrawRangeElementsBaseVertex _glDrawRangeElementsBaseVertex
	typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
	GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC _glDrawElementsInstancedBaseVertex;
#define glDrawElementsInstancedBaseVertex _glDrawElementsInstancedBaseVertex
	typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount, const GLint *basevertex);
	GLAPI PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC _glMultiDrawElementsBaseVertex;
#define glMultiDrawElementsBaseVertex _glMultiDrawElementsBaseVertex
	typedef void (APIENTRYP PFNGLPROVOKINGVERTEXPROC)(GLenum mode);
	GLAPI PFNGLPROVOKINGVERTEXPROC _glProvokingVertex;
#define glProvokingVertex _glProvokingVertex
	typedef GLsync(APIENTRYP PFNGLFENCESYNCPROC)(GLenum condition, GLbitfield flags);
	GLAPI PFNGLFENCESYNCPROC _glFenceSync;
#define glFenceSync _glFenceSync
	typedef GLboolean(APIENTRYP PFNGLISSYNCPROC)(GLsync sync);
	GLAPI PFNGLISSYNCPROC _glIsSync;
#define glIsSync _glIsSync
	typedef void (APIENTRYP PFNGLDELETESYNCPROC)(GLsync sync);
	GLAPI PFNGLDELETESYNCPROC _glDeleteSync;
#define glDeleteSync _glDeleteSync
	typedef GLenum(APIENTRYP PFNGLCLIENTWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
	GLAPI PFNGLCLIENTWAITSYNCPROC _glClientWaitSync;
#define glClientWaitSync _glClientWaitSync
	typedef void (APIENTRYP PFNGLWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
	GLAPI PFNGLWAITSYNCPROC _glWaitSync;
#define glWaitSync _glWaitSync
	typedef void (APIENTRYP PFNGLGETINTEGER64VPROC)(GLenum pname, GLint64 *data);
	GLAPI PFNGLGETINTEGER64VPROC _glGetInteger64v;
#define glGetInteger64v _glGetInteger64v
	typedef void (APIENTRYP PFNGLGETSYNCIVPROC)(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);
	GLAPI PFNGLGETSYNCIVPROC _glGetSynciv;
#define glGetSynciv _glGetSynciv
	typedef void (APIENTRYP PFNGLGETINTEGER64I_VPROC)(GLenum target, GLuint index, GLint64 *data);
	GLAPI PFNGLGETINTEGER64I_VPROC _glGetInteger64i_v;
#define glGetInteger64i_v _glGetInteger64i_v
	typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERI64VPROC)(GLenum target, GLenum pname, GLint64 *params);
	GLAPI PFNGLGETBUFFERPARAMETERI64VPROC _glGetBufferParameteri64v;
#define glGetBufferParameteri64v _glGetBufferParameteri64v
	typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level);
	GLAPI PFNGLFRAMEBUFFERTEXTUREPROC _glFramebufferTexture;
#define glFramebufferTexture _glFramebufferTexture
	typedef void (APIENTRYP PFNGLTEXIMAGE2DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
	GLAPI PFNGLTEXIMAGE2DMULTISAMPLEPROC _glTexImage2DMultisample;
#define glTexImage2DMultisample _glTexImage2DMultisample
	typedef void (APIENTRYP PFNGLTEXIMAGE3DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
	GLAPI PFNGLTEXIMAGE3DMULTISAMPLEPROC _glTexImage3DMultisample;
#define glTexImage3DMultisample _glTexImage3DMultisample
	typedef void (APIENTRYP PFNGLGETMULTISAMPLEFVPROC)(GLenum pname, GLuint index, GLfloat *val);
	GLAPI PFNGLGETMULTISAMPLEFVPROC _glGetMultisamplefv;
#define glGetMultisamplefv _glGetMultisamplefv
	typedef void (APIENTRYP PFNGLSAMPLEMASKIPROC)(GLuint maskNumber, GLbitfield mask);
	GLAPI PFNGLSAMPLEMASKIPROC _glSampleMaski;
#define glSampleMaski _glSampleMaski
#endif
#ifndef GL_VERSION_3_3
#define GL_VERSION_3_3 1
	GLAPI int _GL_VERSION_3_3;
	typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)(GLuint program, GLuint colorNumber, GLuint index, const GLchar *name);
	GLAPI PFNGLBINDFRAGDATALOCATIONINDEXEDPROC _glBindFragDataLocationIndexed;
#define glBindFragDataLocationIndexed _glBindFragDataLocationIndexed
	typedef GLint(APIENTRYP PFNGLGETFRAGDATAINDEXPROC)(GLuint program, const GLchar *name);
	GLAPI PFNGLGETFRAGDATAINDEXPROC _glGetFragDataIndex;
#define glGetFragDataIndex _glGetFragDataIndex
	typedef void (APIENTRYP PFNGLGENSAMPLERSPROC)(GLsizei count, GLuint *samplers);
	GLAPI PFNGLGENSAMPLERSPROC _glGenSamplers;
#define glGenSamplers _glGenSamplers
	typedef void (APIENTRYP PFNGLDELETESAMPLERSPROC)(GLsizei count, const GLuint *samplers);
	GLAPI PFNGLDELETESAMPLERSPROC _glDeleteSamplers;
#define glDeleteSamplers _glDeleteSamplers
	typedef GLboolean(APIENTRYP PFNGLISSAMPLERPROC)(GLuint sampler);
	GLAPI PFNGLISSAMPLERPROC _glIsSampler;
#define glIsSampler _glIsSampler
	typedef void (APIENTRYP PFNGLBINDSAMPLERPROC)(GLuint unit, GLuint sampler);
	GLAPI PFNGLBINDSAMPLERPROC _glBindSampler;
#define glBindSampler _glBindSampler
	typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIPROC)(GLuint sampler, GLenum pname, GLint param);
	GLAPI PFNGLSAMPLERPARAMETERIPROC _glSamplerParameteri;
#define glSamplerParameteri _glSamplerParameteri
	typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIVPROC)(GLuint sampler, GLenum pname, const GLint *param);
	GLAPI PFNGLSAMPLERPARAMETERIVPROC _glSamplerParameteriv;
#define glSamplerParameteriv _glSamplerParameteriv
	typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFPROC)(GLuint sampler, GLenum pname, GLfloat param);
	GLAPI PFNGLSAMPLERPARAMETERFPROC _glSamplerParameterf;
#define glSamplerParameterf _glSamplerParameterf
	typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFVPROC)(GLuint sampler, GLenum pname, const GLfloat *param);
	GLAPI PFNGLSAMPLERPARAMETERFVPROC _glSamplerParameterfv;
#define glSamplerParameterfv _glSamplerParameterfv
	typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIIVPROC)(GLuint sampler, GLenum pname, const GLint *param);
	GLAPI PFNGLSAMPLERPARAMETERIIVPROC _glSamplerParameterIiv;
#define glSamplerParameterIiv _glSamplerParameterIiv
	typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIUIVPROC)(GLuint sampler, GLenum pname, const GLuint *param);
	GLAPI PFNGLSAMPLERPARAMETERIUIVPROC _glSamplerParameterIuiv;
#define glSamplerParameterIuiv _glSamplerParameterIuiv
	typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIVPROC)(GLuint sampler, GLenum pname, GLint *params);
	GLAPI PFNGLGETSAMPLERPARAMETERIVPROC _glGetSamplerParameteriv;
#define glGetSamplerParameteriv _glGetSamplerParameteriv
	typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIIVPROC)(GLuint sampler, GLenum pname, GLint *params);
	GLAPI PFNGLGETSAMPLERPARAMETERIIVPROC _glGetSamplerParameterIiv;
#define glGetSamplerParameterIiv _glGetSamplerParameterIiv
	typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERFVPROC)(GLuint sampler, GLenum pname, GLfloat *params);
	GLAPI PFNGLGETSAMPLERPARAMETERFVPROC _glGetSamplerParameterfv;
#define glGetSamplerParameterfv _glGetSamplerParameterfv
	typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVPROC)(GLuint sampler, GLenum pname, GLuint *params);
	GLAPI PFNGLGETSAMPLERPARAMETERIUIVPROC _glGetSamplerParameterIuiv;
#define glGetSamplerParameterIuiv _glGetSamplerParameterIuiv
	typedef void (APIENTRYP PFNGLQUERYCOUNTERPROC)(GLuint id, GLenum target);
	GLAPI PFNGLQUERYCOUNTERPROC _glQueryCounter;
#define glQueryCounter _glQueryCounter
	typedef void (APIENTRYP PFNGLGETQUERYOBJECTI64VPROC)(GLuint id, GLenum pname, GLint64 *params);
	GLAPI PFNGLGETQUERYOBJECTI64VPROC _glGetQueryObjecti64v;
#define glGetQueryObjecti64v _glGetQueryObjecti64v
	typedef void (APIENTRYP PFNGLGETQUERYOBJECTUI64VPROC)(GLuint id, GLenum pname, GLuint64 *params);
	GLAPI PFNGLGETQUERYOBJECTUI64VPROC _glGetQueryObjectui64v;
#define glGetQueryObjectui64v _glGetQueryObjectui64v
	typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORPROC)(GLuint index, GLuint divisor);
	GLAPI PFNGLVERTEXATTRIBDIVISORPROC _glVertexAttribDivisor;
#define glVertexAttribDivisor _glVertexAttribDivisor
	typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
	GLAPI PFNGLVERTEXATTRIBP1UIPROC _glVertexAttribP1ui;
#define glVertexAttribP1ui _glVertexAttribP1ui
	typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
	GLAPI PFNGLVERTEXATTRIBP1UIVPROC _glVertexAttribP1uiv;
#define glVertexAttribP1uiv _glVertexAttribP1uiv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
	GLAPI PFNGLVERTEXATTRIBP2UIPROC _glVertexAttribP2ui;
#define glVertexAttribP2ui _glVertexAttribP2ui
	typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
	GLAPI PFNGLVERTEXATTRIBP2UIVPROC _glVertexAttribP2uiv;
#define glVertexAttribP2uiv _glVertexAttribP2uiv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
	GLAPI PFNGLVERTEXATTRIBP3UIPROC _glVertexAttribP3ui;
#define glVertexAttribP3ui _glVertexAttribP3ui
	typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
	GLAPI PFNGLVERTEXATTRIBP3UIVPROC _glVertexAttribP3uiv;
#define glVertexAttribP3uiv _glVertexAttribP3uiv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
	GLAPI PFNGLVERTEXATTRIBP4UIPROC _glVertexAttribP4ui;
#define glVertexAttribP4ui _glVertexAttribP4ui
	typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
	GLAPI PFNGLVERTEXATTRIBP4UIVPROC _glVertexAttribP4uiv;
#define glVertexAttribP4uiv _glVertexAttribP4uiv
	typedef void (APIENTRYP PFNGLVERTEXP2UIPROC)(GLenum type, GLuint value);
	GLAPI PFNGLVERTEXP2UIPROC _glVertexP2ui;
#define glVertexP2ui _glVertexP2ui
	typedef void (APIENTRYP PFNGLVERTEXP2UIVPROC)(GLenum type, const GLuint *value);
	GLAPI PFNGLVERTEXP2UIVPROC _glVertexP2uiv;
#define glVertexP2uiv _glVertexP2uiv
	typedef void (APIENTRYP PFNGLVERTEXP3UIPROC)(GLenum type, GLuint value);
	GLAPI PFNGLVERTEXP3UIPROC _glVertexP3ui;
#define glVertexP3ui _glVertexP3ui
	typedef void (APIENTRYP PFNGLVERTEXP3UIVPROC)(GLenum type, const GLuint *value);
	GLAPI PFNGLVERTEXP3UIVPROC _glVertexP3uiv;
#define glVertexP3uiv _glVertexP3uiv
	typedef void (APIENTRYP PFNGLVERTEXP4UIPROC)(GLenum type, GLuint value);
	GLAPI PFNGLVERTEXP4UIPROC _glVertexP4ui;
#define glVertexP4ui _glVertexP4ui
	typedef void (APIENTRYP PFNGLVERTEXP4UIVPROC)(GLenum type, const GLuint *value);
	GLAPI PFNGLVERTEXP4UIVPROC _glVertexP4uiv;
#define glVertexP4uiv _glVertexP4uiv
	typedef void (APIENTRYP PFNGLTEXCOORDP1UIPROC)(GLenum type, GLuint coords);
	GLAPI PFNGLTEXCOORDP1UIPROC _glTexCoordP1ui;
#define glTexCoordP1ui _glTexCoordP1ui
	typedef void (APIENTRYP PFNGLTEXCOORDP1UIVPROC)(GLenum type, const GLuint *coords);
	GLAPI PFNGLTEXCOORDP1UIVPROC _glTexCoordP1uiv;
#define glTexCoordP1uiv _glTexCoordP1uiv
	typedef void (APIENTRYP PFNGLTEXCOORDP2UIPROC)(GLenum type, GLuint coords);
	GLAPI PFNGLTEXCOORDP2UIPROC _glTexCoordP2ui;
#define glTexCoordP2ui _glTexCoordP2ui
	typedef void (APIENTRYP PFNGLTEXCOORDP2UIVPROC)(GLenum type, const GLuint *coords);
	GLAPI PFNGLTEXCOORDP2UIVPROC _glTexCoordP2uiv;
#define glTexCoordP2uiv _glTexCoordP2uiv
	typedef void (APIENTRYP PFNGLTEXCOORDP3UIPROC)(GLenum type, GLuint coords);
	GLAPI PFNGLTEXCOORDP3UIPROC _glTexCoordP3ui;
#define glTexCoordP3ui _glTexCoordP3ui
	typedef void (APIENTRYP PFNGLTEXCOORDP3UIVPROC)(GLenum type, const GLuint *coords);
	GLAPI PFNGLTEXCOORDP3UIVPROC _glTexCoordP3uiv;
#define glTexCoordP3uiv _glTexCoordP3uiv
	typedef void (APIENTRYP PFNGLTEXCOORDP4UIPROC)(GLenum type, GLuint coords);
	GLAPI PFNGLTEXCOORDP4UIPROC _glTexCoordP4ui;
#define glTexCoordP4ui _glTexCoordP4ui
	typedef void (APIENTRYP PFNGLTEXCOORDP4UIVPROC)(GLenum type, const GLuint *coords);
	GLAPI PFNGLTEXCOORDP4UIVPROC _glTexCoordP4uiv;
#define glTexCoordP4uiv _glTexCoordP4uiv
	typedef void (APIENTRYP PFNGLMULTITEXCOORDP1UIPROC)(GLenum texture, GLenum type, GLuint coords);
	GLAPI PFNGLMULTITEXCOORDP1UIPROC _glMultiTexCoordP1ui;
#define glMultiTexCoordP1ui _glMultiTexCoordP1ui
	typedef void (APIENTRYP PFNGLMULTITEXCOORDP1UIVPROC)(GLenum texture, GLenum type, const GLuint *coords);
	GLAPI PFNGLMULTITEXCOORDP1UIVPROC _glMultiTexCoordP1uiv;
#define glMultiTexCoordP1uiv _glMultiTexCoordP1uiv
	typedef void (APIENTRYP PFNGLMULTITEXCOORDP2UIPROC)(GLenum texture, GLenum type, GLuint coords);
	GLAPI PFNGLMULTITEXCOORDP2UIPROC _glMultiTexCoordP2ui;
#define glMultiTexCoordP2ui _glMultiTexCoordP2ui
	typedef void (APIENTRYP PFNGLMULTITEXCOORDP2UIVPROC)(GLenum texture, GLenum type, const GLuint *coords);
	GLAPI PFNGLMULTITEXCOORDP2UIVPROC _glMultiTexCoordP2uiv;
#define glMultiTexCoordP2uiv _glMultiTexCoordP2uiv
	typedef void (APIENTRYP PFNGLMULTITEXCOORDP3UIPROC)(GLenum texture, GLenum type, GLuint coords);
	GLAPI PFNGLMULTITEXCOORDP3UIPROC _glMultiTexCoordP3ui;
#define glMultiTexCoordP3ui _glMultiTexCoordP3ui
	typedef void (APIENTRYP PFNGLMULTITEXCOORDP3UIVPROC)(GLenum texture, GLenum type, const GLuint *coords);
	GLAPI PFNGLMULTITEXCOORDP3UIVPROC _glMultiTexCoordP3uiv;
#define glMultiTexCoordP3uiv _glMultiTexCoordP3uiv
	typedef void (APIENTRYP PFNGLMULTITEXCOORDP4UIPROC)(GLenum texture, GLenum type, GLuint coords);
	GLAPI PFNGLMULTITEXCOORDP4UIPROC _glMultiTexCoordP4ui;
#define glMultiTexCoordP4ui _glMultiTexCoordP4ui
	typedef void (APIENTRYP PFNGLMULTITEXCOORDP4UIVPROC)(GLenum texture, GLenum type, const GLuint *coords);
	GLAPI PFNGLMULTITEXCOORDP4UIVPROC _glMultiTexCoordP4uiv;
#define glMultiTexCoordP4uiv _glMultiTexCoordP4uiv
	typedef void (APIENTRYP PFNGLNORMALP3UIPROC)(GLenum type, GLuint coords);
	GLAPI PFNGLNORMALP3UIPROC _glNormalP3ui;
#define glNormalP3ui _glNormalP3ui
	typedef void (APIENTRYP PFNGLNORMALP3UIVPROC)(GLenum type, const GLuint *coords);
	GLAPI PFNGLNORMALP3UIVPROC _glNormalP3uiv;
#define glNormalP3uiv _glNormalP3uiv
	typedef void (APIENTRYP PFNGLCOLORP3UIPROC)(GLenum type, GLuint color);
	GLAPI PFNGLCOLORP3UIPROC _glColorP3ui;
#define glColorP3ui _glColorP3ui
	typedef void (APIENTRYP PFNGLCOLORP3UIVPROC)(GLenum type, const GLuint *color);
	GLAPI PFNGLCOLORP3UIVPROC _glColorP3uiv;
#define glColorP3uiv _glColorP3uiv
	typedef void (APIENTRYP PFNGLCOLORP4UIPROC)(GLenum type, GLuint color);
	GLAPI PFNGLCOLORP4UIPROC _glColorP4ui;
#define glColorP4ui _glColorP4ui
	typedef void (APIENTRYP PFNGLCOLORP4UIVPROC)(GLenum type, const GLuint *color);
	GLAPI PFNGLCOLORP4UIVPROC _glColorP4uiv;
#define glColorP4uiv _glColorP4uiv
	typedef void (APIENTRYP PFNGLSECONDARYCOLORP3UIPROC)(GLenum type, GLuint color);
	GLAPI PFNGLSECONDARYCOLORP3UIPROC _glSecondaryColorP3ui;
#define glSecondaryColorP3ui _glSecondaryColorP3ui
	typedef void (APIENTRYP PFNGLSECONDARYCOLORP3UIVPROC)(GLenum type, const GLuint *color);
	GLAPI PFNGLSECONDARYCOLORP3UIVPROC _glSecondaryColorP3uiv;
#define glSecondaryColorP3uiv _glSecondaryColorP3uiv
#endif
#ifndef GL_VERSION_4_0
#define GL_VERSION_4_0 1
	GLAPI int _GL_VERSION_4_0;
	typedef void (APIENTRYP PFNGLMINSAMPLESHADINGPROC)(GLfloat value);
	GLAPI PFNGLMINSAMPLESHADINGPROC _glMinSampleShading;
#define glMinSampleShading _glMinSampleShading
	typedef void (APIENTRYP PFNGLBLENDEQUATIONIPROC)(GLuint buf, GLenum mode);
	GLAPI PFNGLBLENDEQUATIONIPROC _glBlendEquationi;
#define glBlendEquationi _glBlendEquationi
	typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEIPROC)(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
	GLAPI PFNGLBLENDEQUATIONSEPARATEIPROC _glBlendEquationSeparatei;
#define glBlendEquationSeparatei _glBlendEquationSeparatei
	typedef void (APIENTRYP PFNGLBLENDFUNCIPROC)(GLuint buf, GLenum src, GLenum dst);
	GLAPI PFNGLBLENDFUNCIPROC _glBlendFunci;
#define glBlendFunci _glBlendFunci
	typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEIPROC)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
	GLAPI PFNGLBLENDFUNCSEPARATEIPROC _glBlendFuncSeparatei;
#define glBlendFuncSeparatei _glBlendFuncSeparatei
	typedef void (APIENTRYP PFNGLDRAWARRAYSINDIRECTPROC)(GLenum mode, const void *indirect);
	GLAPI PFNGLDRAWARRAYSINDIRECTPROC _glDrawArraysIndirect;
#define glDrawArraysIndirect _glDrawArraysIndirect
	typedef void (APIENTRYP PFNGLDRAWELEMENTSINDIRECTPROC)(GLenum mode, GLenum type, const void *indirect);
	GLAPI PFNGLDRAWELEMENTSINDIRECTPROC _glDrawElementsIndirect;
#define glDrawElementsIndirect _glDrawElementsIndirect
	typedef void (APIENTRYP PFNGLUNIFORM1DPROC)(GLint location, GLdouble x);
	GLAPI PFNGLUNIFORM1DPROC _glUniform1d;
#define glUniform1d _glUniform1d
	typedef void (APIENTRYP PFNGLUNIFORM2DPROC)(GLint location, GLdouble x, GLdouble y);
	GLAPI PFNGLUNIFORM2DPROC _glUniform2d;
#define glUniform2d _glUniform2d
	typedef void (APIENTRYP PFNGLUNIFORM3DPROC)(GLint location, GLdouble x, GLdouble y, GLdouble z);
	GLAPI PFNGLUNIFORM3DPROC _glUniform3d;
#define glUniform3d _glUniform3d
	typedef void (APIENTRYP PFNGLUNIFORM4DPROC)(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	GLAPI PFNGLUNIFORM4DPROC _glUniform4d;
#define glUniform4d _glUniform4d
	typedef void (APIENTRYP PFNGLUNIFORM1DVPROC)(GLint location, GLsizei count, const GLdouble *value);
	GLAPI PFNGLUNIFORM1DVPROC _glUniform1dv;
#define glUniform1dv _glUniform1dv
	typedef void (APIENTRYP PFNGLUNIFORM2DVPROC)(GLint location, GLsizei count, const GLdouble *value);
	GLAPI PFNGLUNIFORM2DVPROC _glUniform2dv;
#define glUniform2dv _glUniform2dv
	typedef void (APIENTRYP PFNGLUNIFORM3DVPROC)(GLint location, GLsizei count, const GLdouble *value);
	GLAPI PFNGLUNIFORM3DVPROC _glUniform3dv;
#define glUniform3dv _glUniform3dv
	typedef void (APIENTRYP PFNGLUNIFORM4DVPROC)(GLint location, GLsizei count, const GLdouble *value);
	GLAPI PFNGLUNIFORM4DVPROC _glUniform4dv;
#define glUniform4dv _glUniform4dv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLUNIFORMMATRIX2DVPROC _glUniformMatrix2dv;
#define glUniformMatrix2dv _glUniformMatrix2dv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLUNIFORMMATRIX3DVPROC _glUniformMatrix3dv;
#define glUniformMatrix3dv _glUniformMatrix3dv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLUNIFORMMATRIX4DVPROC _glUniformMatrix4dv;
#define glUniformMatrix4dv _glUniformMatrix4dv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLUNIFORMMATRIX2X3DVPROC _glUniformMatrix2x3dv;
#define glUniformMatrix2x3dv _glUniformMatrix2x3dv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLUNIFORMMATRIX2X4DVPROC _glUniformMatrix2x4dv;
#define glUniformMatrix2x4dv _glUniformMatrix2x4dv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLUNIFORMMATRIX3X2DVPROC _glUniformMatrix3x2dv;
#define glUniformMatrix3x2dv _glUniformMatrix3x2dv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLUNIFORMMATRIX3X4DVPROC _glUniformMatrix3x4dv;
#define glUniformMatrix3x4dv _glUniformMatrix3x4dv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLUNIFORMMATRIX4X2DVPROC _glUniformMatrix4x2dv;
#define glUniformMatrix4x2dv _glUniformMatrix4x2dv
	typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLUNIFORMMATRIX4X3DVPROC _glUniformMatrix4x3dv;
#define glUniformMatrix4x3dv _glUniformMatrix4x3dv
	typedef void (APIENTRYP PFNGLGETUNIFORMDVPROC)(GLuint program, GLint location, GLdouble *params);
	GLAPI PFNGLGETUNIFORMDVPROC _glGetUniformdv;
#define glGetUniformdv _glGetUniformdv
	typedef GLint(APIENTRYP PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)(GLuint program, GLenum shadertype, const GLchar *name);
	GLAPI PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC _glGetSubroutineUniformLocation;
#define glGetSubroutineUniformLocation _glGetSubroutineUniformLocation
	typedef GLuint(APIENTRYP PFNGLGETSUBROUTINEINDEXPROC)(GLuint program, GLenum shadertype, const GLchar *name);
	GLAPI PFNGLGETSUBROUTINEINDEXPROC _glGetSubroutineIndex;
#define glGetSubroutineIndex _glGetSubroutineIndex
	typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint *values);
	GLAPI PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC _glGetActiveSubroutineUniformiv;
#define glGetActiveSubroutineUniformiv _glGetActiveSubroutineUniformiv
	typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei *length, GLchar *name);
	GLAPI PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC _glGetActiveSubroutineUniformName;
#define glGetActiveSubroutineUniformName _glGetActiveSubroutineUniformName
	typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINENAMEPROC)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei *length, GLchar *name);
	GLAPI PFNGLGETACTIVESUBROUTINENAMEPROC _glGetActiveSubroutineName;
#define glGetActiveSubroutineName _glGetActiveSubroutineName
	typedef void (APIENTRYP PFNGLUNIFORMSUBROUTINESUIVPROC)(GLenum shadertype, GLsizei count, const GLuint *indices);
	GLAPI PFNGLUNIFORMSUBROUTINESUIVPROC _glUniformSubroutinesuiv;
#define glUniformSubroutinesuiv _glUniformSubroutinesuiv
	typedef void (APIENTRYP PFNGLGETUNIFORMSUBROUTINEUIVPROC)(GLenum shadertype, GLint location, GLuint *params);
	GLAPI PFNGLGETUNIFORMSUBROUTINEUIVPROC _glGetUniformSubroutineuiv;
#define glGetUniformSubroutineuiv _glGetUniformSubroutineuiv
	typedef void (APIENTRYP PFNGLGETPROGRAMSTAGEIVPROC)(GLuint program, GLenum shadertype, GLenum pname, GLint *values);
	GLAPI PFNGLGETPROGRAMSTAGEIVPROC _glGetProgramStageiv;
#define glGetProgramStageiv _glGetProgramStageiv
	typedef void (APIENTRYP PFNGLPATCHPARAMETERIPROC)(GLenum pname, GLint value);
	GLAPI PFNGLPATCHPARAMETERIPROC _glPatchParameteri;
#define glPatchParameteri _glPatchParameteri
	typedef void (APIENTRYP PFNGLPATCHPARAMETERFVPROC)(GLenum pname, const GLfloat *values);
	GLAPI PFNGLPATCHPARAMETERFVPROC _glPatchParameterfv;
#define glPatchParameterfv _glPatchParameterfv
	typedef void (APIENTRYP PFNGLBINDTRANSFORMFEEDBACKPROC)(GLenum target, GLuint id);
	GLAPI PFNGLBINDTRANSFORMFEEDBACKPROC _glBindTransformFeedback;
#define glBindTransformFeedback _glBindTransformFeedback
	typedef void (APIENTRYP PFNGLDELETETRANSFORMFEEDBACKSPROC)(GLsizei n, const GLuint *ids);
	GLAPI PFNGLDELETETRANSFORMFEEDBACKSPROC _glDeleteTransformFeedbacks;
#define glDeleteTransformFeedbacks _glDeleteTransformFeedbacks
	typedef void (APIENTRYP PFNGLGENTRANSFORMFEEDBACKSPROC)(GLsizei n, GLuint *ids);
	GLAPI PFNGLGENTRANSFORMFEEDBACKSPROC _glGenTransformFeedbacks;
#define glGenTransformFeedbacks _glGenTransformFeedbacks
	typedef GLboolean(APIENTRYP PFNGLISTRANSFORMFEEDBACKPROC)(GLuint id);
	GLAPI PFNGLISTRANSFORMFEEDBACKPROC _glIsTransformFeedback;
#define glIsTransformFeedback _glIsTransformFeedback
	typedef void (APIENTRYP PFNGLPAUSETRANSFORMFEEDBACKPROC)(void);
	GLAPI PFNGLPAUSETRANSFORMFEEDBACKPROC _glPauseTransformFeedback;
#define glPauseTransformFeedback _glPauseTransformFeedback
	typedef void (APIENTRYP PFNGLRESUMETRANSFORMFEEDBACKPROC)(void);
	GLAPI PFNGLRESUMETRANSFORMFEEDBACKPROC _glResumeTransformFeedback;
#define glResumeTransformFeedback _glResumeTransformFeedback
	typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKPROC)(GLenum mode, GLuint id);
	GLAPI PFNGLDRAWTRANSFORMFEEDBACKPROC _glDrawTransformFeedback;
#define glDrawTransformFeedback _glDrawTransformFeedback
	typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)(GLenum mode, GLuint id, GLuint stream);
	GLAPI PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC _glDrawTransformFeedbackStream;
#define glDrawTransformFeedbackStream _glDrawTransformFeedbackStream
	typedef void (APIENTRYP PFNGLBEGINQUERYINDEXEDPROC)(GLenum target, GLuint index, GLuint id);
	GLAPI PFNGLBEGINQUERYINDEXEDPROC _glBeginQueryIndexed;
#define glBeginQueryIndexed _glBeginQueryIndexed
	typedef void (APIENTRYP PFNGLENDQUERYINDEXEDPROC)(GLenum target, GLuint index);
	GLAPI PFNGLENDQUERYINDEXEDPROC _glEndQueryIndexed;
#define glEndQueryIndexed _glEndQueryIndexed
	typedef void (APIENTRYP PFNGLGETQUERYINDEXEDIVPROC)(GLenum target, GLuint index, GLenum pname, GLint *params);
	GLAPI PFNGLGETQUERYINDEXEDIVPROC _glGetQueryIndexediv;
#define glGetQueryIndexediv _glGetQueryIndexediv
#endif
#ifndef GL_VERSION_4_1
#define GL_VERSION_4_1 1
	GLAPI int _GL_VERSION_4_1;
	typedef void (APIENTRYP PFNGLRELEASESHADERCOMPILERPROC)(void);
	GLAPI PFNGLRELEASESHADERCOMPILERPROC _glReleaseShaderCompiler;
#define glReleaseShaderCompiler _glReleaseShaderCompiler
	typedef void (APIENTRYP PFNGLSHADERBINARYPROC)(GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length);
	GLAPI PFNGLSHADERBINARYPROC _glShaderBinary;
#define glShaderBinary _glShaderBinary
	typedef void (APIENTRYP PFNGLGETSHADERPRECISIONFORMATPROC)(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);
	GLAPI PFNGLGETSHADERPRECISIONFORMATPROC _glGetShaderPrecisionFormat;
#define glGetShaderPrecisionFormat _glGetShaderPrecisionFormat
	typedef void (APIENTRYP PFNGLDEPTHRANGEFPROC)(GLfloat n, GLfloat f);
	GLAPI PFNGLDEPTHRANGEFPROC _glDepthRangef;
#define glDepthRangef _glDepthRangef
	typedef void (APIENTRYP PFNGLCLEARDEPTHFPROC)(GLfloat d);
	GLAPI PFNGLCLEARDEPTHFPROC _glClearDepthf;
#define glClearDepthf _glClearDepthf
	typedef void (APIENTRYP PFNGLGETPROGRAMBINARYPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
	GLAPI PFNGLGETPROGRAMBINARYPROC _glGetProgramBinary;
#define glGetProgramBinary _glGetProgramBinary
	typedef void (APIENTRYP PFNGLPROGRAMBINARYPROC)(GLuint program, GLenum binaryFormat, const void *binary, GLsizei length);
	GLAPI PFNGLPROGRAMBINARYPROC _glProgramBinary;
#define glProgramBinary _glProgramBinary
	typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIPROC)(GLuint program, GLenum pname, GLint value);
	GLAPI PFNGLPROGRAMPARAMETERIPROC _glProgramParameteri;
#define glProgramParameteri _glProgramParameteri
	typedef void (APIENTRYP PFNGLUSEPROGRAMSTAGESPROC)(GLuint pipeline, GLbitfield stages, GLuint program);
	GLAPI PFNGLUSEPROGRAMSTAGESPROC _glUseProgramStages;
#define glUseProgramStages _glUseProgramStages
	typedef void (APIENTRYP PFNGLACTIVESHADERPROGRAMPROC)(GLuint pipeline, GLuint program);
	GLAPI PFNGLACTIVESHADERPROGRAMPROC _glActiveShaderProgram;
#define glActiveShaderProgram _glActiveShaderProgram
	typedef GLuint(APIENTRYP PFNGLCREATESHADERPROGRAMVPROC)(GLenum type, GLsizei count, const GLchar *const*strings);
	GLAPI PFNGLCREATESHADERPROGRAMVPROC _glCreateShaderProgramv;
#define glCreateShaderProgramv _glCreateShaderProgramv
	typedef void (APIENTRYP PFNGLBINDPROGRAMPIPELINEPROC)(GLuint pipeline);
	GLAPI PFNGLBINDPROGRAMPIPELINEPROC _glBindProgramPipeline;
#define glBindProgramPipeline _glBindProgramPipeline
	typedef void (APIENTRYP PFNGLDELETEPROGRAMPIPELINESPROC)(GLsizei n, const GLuint *pipelines);
	GLAPI PFNGLDELETEPROGRAMPIPELINESPROC _glDeleteProgramPipelines;
#define glDeleteProgramPipelines _glDeleteProgramPipelines
	typedef void (APIENTRYP PFNGLGENPROGRAMPIPELINESPROC)(GLsizei n, GLuint *pipelines);
	GLAPI PFNGLGENPROGRAMPIPELINESPROC _glGenProgramPipelines;
#define glGenProgramPipelines _glGenProgramPipelines
	typedef GLboolean(APIENTRYP PFNGLISPROGRAMPIPELINEPROC)(GLuint pipeline);
	GLAPI PFNGLISPROGRAMPIPELINEPROC _glIsProgramPipeline;
#define glIsProgramPipeline _glIsProgramPipeline
	typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEIVPROC)(GLuint pipeline, GLenum pname, GLint *params);
	GLAPI PFNGLGETPROGRAMPIPELINEIVPROC _glGetProgramPipelineiv;
#define glGetProgramPipelineiv _glGetProgramPipelineiv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IPROC)(GLuint program, GLint location, GLint v0);
	GLAPI PFNGLPROGRAMUNIFORM1IPROC _glProgramUniform1i;
#define glProgramUniform1i _glProgramUniform1i
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
	GLAPI PFNGLPROGRAMUNIFORM1IVPROC _glProgramUniform1iv;
#define glProgramUniform1iv _glProgramUniform1iv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FPROC)(GLuint program, GLint location, GLfloat v0);
	GLAPI PFNGLPROGRAMUNIFORM1FPROC _glProgramUniform1f;
#define glProgramUniform1f _glProgramUniform1f
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORM1FVPROC _glProgramUniform1fv;
#define glProgramUniform1fv _glProgramUniform1fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DPROC)(GLuint program, GLint location, GLdouble v0);
	GLAPI PFNGLPROGRAMUNIFORM1DPROC _glProgramUniform1d;
#define glProgramUniform1d _glProgramUniform1d
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORM1DVPROC _glProgramUniform1dv;
#define glProgramUniform1dv _glProgramUniform1dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIPROC)(GLuint program, GLint location, GLuint v0);
	GLAPI PFNGLPROGRAMUNIFORM1UIPROC _glProgramUniform1ui;
#define glProgramUniform1ui _glProgramUniform1ui
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
	GLAPI PFNGLPROGRAMUNIFORM1UIVPROC _glProgramUniform1uiv;
#define glProgramUniform1uiv _glProgramUniform1uiv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IPROC)(GLuint program, GLint location, GLint v0, GLint v1);
	GLAPI PFNGLPROGRAMUNIFORM2IPROC _glProgramUniform2i;
#define glProgramUniform2i _glProgramUniform2i
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
	GLAPI PFNGLPROGRAMUNIFORM2IVPROC _glProgramUniform2iv;
#define glProgramUniform2iv _glProgramUniform2iv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1);
	GLAPI PFNGLPROGRAMUNIFORM2FPROC _glProgramUniform2f;
#define glProgramUniform2f _glProgramUniform2f
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORM2FVPROC _glProgramUniform2fv;
#define glProgramUniform2fv _glProgramUniform2fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DPROC)(GLuint program, GLint location, GLdouble v0, GLdouble v1);
	GLAPI PFNGLPROGRAMUNIFORM2DPROC _glProgramUniform2d;
#define glProgramUniform2d _glProgramUniform2d
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORM2DVPROC _glProgramUniform2dv;
#define glProgramUniform2dv _glProgramUniform2dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIPROC)(GLuint program, GLint location, GLuint v0, GLuint v1);
	GLAPI PFNGLPROGRAMUNIFORM2UIPROC _glProgramUniform2ui;
#define glProgramUniform2ui _glProgramUniform2ui
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
	GLAPI PFNGLPROGRAMUNIFORM2UIVPROC _glProgramUniform2uiv;
#define glProgramUniform2uiv _glProgramUniform2uiv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IPROC)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
	GLAPI PFNGLPROGRAMUNIFORM3IPROC _glProgramUniform3i;
#define glProgramUniform3i _glProgramUniform3i
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
	GLAPI PFNGLPROGRAMUNIFORM3IVPROC _glProgramUniform3iv;
#define glProgramUniform3iv _glProgramUniform3iv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	GLAPI PFNGLPROGRAMUNIFORM3FPROC _glProgramUniform3f;
#define glProgramUniform3f _glProgramUniform3f
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORM3FVPROC _glProgramUniform3fv;
#define glProgramUniform3fv _glProgramUniform3fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DPROC)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
	GLAPI PFNGLPROGRAMUNIFORM3DPROC _glProgramUniform3d;
#define glProgramUniform3d _glProgramUniform3d
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORM3DVPROC _glProgramUniform3dv;
#define glProgramUniform3dv _glProgramUniform3dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIPROC)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
	GLAPI PFNGLPROGRAMUNIFORM3UIPROC _glProgramUniform3ui;
#define glProgramUniform3ui _glProgramUniform3ui
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
	GLAPI PFNGLPROGRAMUNIFORM3UIVPROC _glProgramUniform3uiv;
#define glProgramUniform3uiv _glProgramUniform3uiv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IPROC)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
	GLAPI PFNGLPROGRAMUNIFORM4IPROC _glProgramUniform4i;
#define glProgramUniform4i _glProgramUniform4i
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
	GLAPI PFNGLPROGRAMUNIFORM4IVPROC _glProgramUniform4iv;
#define glProgramUniform4iv _glProgramUniform4iv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	GLAPI PFNGLPROGRAMUNIFORM4FPROC _glProgramUniform4f;
#define glProgramUniform4f _glProgramUniform4f
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORM4FVPROC _glProgramUniform4fv;
#define glProgramUniform4fv _glProgramUniform4fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DPROC)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
	GLAPI PFNGLPROGRAMUNIFORM4DPROC _glProgramUniform4d;
#define glProgramUniform4d _glProgramUniform4d
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORM4DVPROC _glProgramUniform4dv;
#define glProgramUniform4dv _glProgramUniform4dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIPROC)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
	GLAPI PFNGLPROGRAMUNIFORM4UIPROC _glProgramUniform4ui;
#define glProgramUniform4ui _glProgramUniform4ui
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
	GLAPI PFNGLPROGRAMUNIFORM4UIVPROC _glProgramUniform4uiv;
#define glProgramUniform4uiv _glProgramUniform4uiv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX2FVPROC _glProgramUniformMatrix2fv;
#define glProgramUniformMatrix2fv _glProgramUniformMatrix2fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX3FVPROC _glProgramUniformMatrix3fv;
#define glProgramUniformMatrix3fv _glProgramUniformMatrix3fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX4FVPROC _glProgramUniformMatrix4fv;
#define glProgramUniformMatrix4fv _glProgramUniformMatrix4fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX2DVPROC _glProgramUniformMatrix2dv;
#define glProgramUniformMatrix2dv _glProgramUniformMatrix2dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX3DVPROC _glProgramUniformMatrix3dv;
#define glProgramUniformMatrix3dv _glProgramUniformMatrix3dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX4DVPROC _glProgramUniformMatrix4dv;
#define glProgramUniformMatrix4dv _glProgramUniformMatrix4dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC _glProgramUniformMatrix2x3fv;
#define glProgramUniformMatrix2x3fv _glProgramUniformMatrix2x3fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC _glProgramUniformMatrix3x2fv;
#define glProgramUniformMatrix3x2fv _glProgramUniformMatrix3x2fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC _glProgramUniformMatrix2x4fv;
#define glProgramUniformMatrix2x4fv _glProgramUniformMatrix2x4fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC _glProgramUniformMatrix4x2fv;
#define glProgramUniformMatrix4x2fv _glProgramUniformMatrix4x2fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC _glProgramUniformMatrix3x4fv;
#define glProgramUniformMatrix3x4fv _glProgramUniformMatrix3x4fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC _glProgramUniformMatrix4x3fv;
#define glProgramUniformMatrix4x3fv _glProgramUniformMatrix4x3fv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC _glProgramUniformMatrix2x3dv;
#define glProgramUniformMatrix2x3dv _glProgramUniformMatrix2x3dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC _glProgramUniformMatrix3x2dv;
#define glProgramUniformMatrix3x2dv _glProgramUniformMatrix3x2dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC _glProgramUniformMatrix2x4dv;
#define glProgramUniformMatrix2x4dv _glProgramUniformMatrix2x4dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC _glProgramUniformMatrix4x2dv;
#define glProgramUniformMatrix4x2dv _glProgramUniformMatrix4x2dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC _glProgramUniformMatrix3x4dv;
#define glProgramUniformMatrix3x4dv _glProgramUniformMatrix3x4dv
	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
	GLAPI PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC _glProgramUniformMatrix4x3dv;
#define glProgramUniformMatrix4x3dv _glProgramUniformMatrix4x3dv
	typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPIPELINEPROC)(GLuint pipeline);
	GLAPI PFNGLVALIDATEPROGRAMPIPELINEPROC _glValidateProgramPipeline;
#define glValidateProgramPipeline _glValidateProgramPipeline
	typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEINFOLOGPROC)(GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
	GLAPI PFNGLGETPROGRAMPIPELINEINFOLOGPROC _glGetProgramPipelineInfoLog;
#define glGetProgramPipelineInfoLog _glGetProgramPipelineInfoLog
	typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DPROC)(GLuint index, GLdouble x);
	GLAPI PFNGLVERTEXATTRIBL1DPROC _glVertexAttribL1d;
#define glVertexAttribL1d _glVertexAttribL1d
	typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DPROC)(GLuint index, GLdouble x, GLdouble y);
	GLAPI PFNGLVERTEXATTRIBL2DPROC _glVertexAttribL2d;
#define glVertexAttribL2d _glVertexAttribL2d
	typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
	GLAPI PFNGLVERTEXATTRIBL3DPROC _glVertexAttribL3d;
#define glVertexAttribL3d _glVertexAttribL3d
	typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	GLAPI PFNGLVERTEXATTRIBL4DPROC _glVertexAttribL4d;
#define glVertexAttribL4d _glVertexAttribL4d
	typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DVPROC)(GLuint index, const GLdouble *v);
	GLAPI PFNGLVERTEXATTRIBL1DVPROC _glVertexAttribL1dv;
#define glVertexAttribL1dv _glVertexAttribL1dv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DVPROC)(GLuint index, const GLdouble *v);
	GLAPI PFNGLVERTEXATTRIBL2DVPROC _glVertexAttribL2dv;
#define glVertexAttribL2dv _glVertexAttribL2dv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DVPROC)(GLuint index, const GLdouble *v);
	GLAPI PFNGLVERTEXATTRIBL3DVPROC _glVertexAttribL3dv;
#define glVertexAttribL3dv _glVertexAttribL3dv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DVPROC)(GLuint index, const GLdouble *v);
	GLAPI PFNGLVERTEXATTRIBL4DVPROC _glVertexAttribL4dv;
#define glVertexAttribL4dv _glVertexAttribL4dv
	typedef void (APIENTRYP PFNGLVERTEXATTRIBLPOINTERPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
	GLAPI PFNGLVERTEXATTRIBLPOINTERPROC _glVertexAttribLPointer;
#define glVertexAttribLPointer _glVertexAttribLPointer
	typedef void (APIENTRYP PFNGLGETVERTEXATTRIBLDVPROC)(GLuint index, GLenum pname, GLdouble *params);
	GLAPI PFNGLGETVERTEXATTRIBLDVPROC _glGetVertexAttribLdv;
#define glGetVertexAttribLdv _glGetVertexAttribLdv
	typedef void (APIENTRYP PFNGLVIEWPORTARRAYVPROC)(GLuint first, GLsizei count, const GLfloat *v);
	GLAPI PFNGLVIEWPORTARRAYVPROC _glViewportArrayv;
#define glViewportArrayv _glViewportArrayv
	typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
	GLAPI PFNGLVIEWPORTINDEXEDFPROC _glViewportIndexedf;
#define glViewportIndexedf _glViewportIndexedf
	typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFVPROC)(GLuint index, const GLfloat *v);
	GLAPI PFNGLVIEWPORTINDEXEDFVPROC _glViewportIndexedfv;
#define glViewportIndexedfv _glViewportIndexedfv
	typedef void (APIENTRYP PFNGLSCISSORARRAYVPROC)(GLuint first, GLsizei count, const GLint *v);
	GLAPI PFNGLSCISSORARRAYVPROC _glScissorArrayv;
#define glScissorArrayv _glScissorArrayv
	typedef void (APIENTRYP PFNGLSCISSORINDEXEDPROC)(GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height);
	GLAPI PFNGLSCISSORINDEXEDPROC _glScissorIndexed;
#define glScissorIndexed _glScissorIndexed
	typedef void (APIENTRYP PFNGLSCISSORINDEXEDVPROC)(GLuint index, const GLint *v);
	GLAPI PFNGLSCISSORINDEXEDVPROC _glScissorIndexedv;
#define glScissorIndexedv _glScissorIndexedv
	typedef void (APIENTRYP PFNGLDEPTHRANGEARRAYVPROC)(GLuint first, GLsizei count, const GLdouble *v);
	GLAPI PFNGLDEPTHRANGEARRAYVPROC _glDepthRangeArrayv;
#define glDepthRangeArrayv _glDepthRangeArrayv
	typedef void (APIENTRYP PFNGLDEPTHRANGEINDEXEDPROC)(GLuint index, GLdouble n, GLdouble f);
	GLAPI PFNGLDEPTHRANGEINDEXEDPROC _glDepthRangeIndexed;
#define glDepthRangeIndexed _glDepthRangeIndexed
	typedef void (APIENTRYP PFNGLGETFLOATI_VPROC)(GLenum target, GLuint index, GLfloat *data);
	GLAPI PFNGLGETFLOATI_VPROC _glGetFloati_v;
#define glGetFloati_v _glGetFloati_v
	typedef void (APIENTRYP PFNGLGETDOUBLEI_VPROC)(GLenum target, GLuint index, GLdouble *data);
	GLAPI PFNGLGETDOUBLEI_VPROC _glGetDoublei_v;
#define glGetDoublei_v _glGetDoublei_v
#endif
#ifndef GL_VERSION_4_2
#define GL_VERSION_4_2 1
	GLAPI int _GL_VERSION_4_2;
	typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance);
	GLAPI PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC _glDrawArraysInstancedBaseInstance;
#define glDrawArraysInstancedBaseInstance _glDrawArraysInstancedBaseInstance
	typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLuint baseinstance);
	GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC _glDrawElementsInstancedBaseInstance;
#define glDrawElementsInstancedBaseInstance _glDrawElementsInstancedBaseInstance
	typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance);
	GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC _glDrawElementsInstancedBaseVertexBaseInstance;
#define glDrawElementsInstancedBaseVertexBaseInstance _glDrawElementsInstancedBaseVertexBaseInstance
	typedef void (APIENTRYP PFNGLGETINTERNALFORMATIVPROC)(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params);
	GLAPI PFNGLGETINTERNALFORMATIVPROC _glGetInternalformativ;
#define glGetInternalformativ _glGetInternalformativ
	typedef void (APIENTRYP PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC)(GLuint program, GLuint bufferIndex, GLenum pname, GLint *params);
	GLAPI PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC _glGetActiveAtomicCounterBufferiv;
#define glGetActiveAtomicCounterBufferiv _glGetActiveAtomicCounterBufferiv
	typedef void (APIENTRYP PFNGLBINDIMAGETEXTUREPROC)(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
	GLAPI PFNGLBINDIMAGETEXTUREPROC _glBindImageTexture;
#define glBindImageTexture _glBindImageTexture
	typedef void (APIENTRYP PFNGLMEMORYBARRIERPROC)(GLbitfield barriers);
	GLAPI PFNGLMEMORYBARRIERPROC _glMemoryBarrier;
#define glMemoryBarrier _glMemoryBarrier
	typedef void (APIENTRYP PFNGLTEXSTORAGE1DPROC)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
	GLAPI PFNGLTEXSTORAGE1DPROC _glTexStorage1D;
#define glTexStorage1D _glTexStorage1D
	typedef void (APIENTRYP PFNGLTEXSTORAGE2DPROC)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
	GLAPI PFNGLTEXSTORAGE2DPROC _glTexStorage2D;
#define glTexStorage2D _glTexStorage2D
	typedef void (APIENTRYP PFNGLTEXSTORAGE3DPROC)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
	GLAPI PFNGLTEXSTORAGE3DPROC _glTexStorage3D;
#define glTexStorage3D _glTexStorage3D
	typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC)(GLenum mode, GLuint id, GLsizei instancecount);
	GLAPI PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC _glDrawTransformFeedbackInstanced;
#define glDrawTransformFeedbackInstanced _glDrawTransformFeedbackInstanced
	typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC)(GLenum mode, GLuint id, GLuint stream, GLsizei instancecount);
	GLAPI PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC _glDrawTransformFeedbackStreamInstanced;
#define glDrawTransformFeedbackStreamInstanced _glDrawTransformFeedbackStreamInstanced
#endif
#ifndef GL_VERSION_4_3
#define GL_VERSION_4_3 1
	GLAPI int _GL_VERSION_4_3;
	typedef void (APIENTRYP PFNGLCLEARBUFFERDATAPROC)(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void *data);
	GLAPI PFNGLCLEARBUFFERDATAPROC _glClearBufferData;
#define glClearBufferData _glClearBufferData
	typedef void (APIENTRYP PFNGLCLEARBUFFERSUBDATAPROC)(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
	GLAPI PFNGLCLEARBUFFERSUBDATAPROC _glClearBufferSubData;
#define glClearBufferSubData _glClearBufferSubData
	typedef void (APIENTRYP PFNGLDISPATCHCOMPUTEPROC)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
	GLAPI PFNGLDISPATCHCOMPUTEPROC _glDispatchCompute;
#define glDispatchCompute _glDispatchCompute
	typedef void (APIENTRYP PFNGLDISPATCHCOMPUTEINDIRECTPROC)(GLintptr indirect);
	GLAPI PFNGLDISPATCHCOMPUTEINDIRECTPROC _glDispatchComputeIndirect;
#define glDispatchComputeIndirect _glDispatchComputeIndirect
	typedef void (APIENTRYP PFNGLCOPYIMAGESUBDATAPROC)(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
	GLAPI PFNGLCOPYIMAGESUBDATAPROC _glCopyImageSubData;
#define glCopyImageSubData _glCopyImageSubData
	typedef void (APIENTRYP PFNGLFRAMEBUFFERPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
	GLAPI PFNGLFRAMEBUFFERPARAMETERIPROC _glFramebufferParameteri;
#define glFramebufferParameteri _glFramebufferParameteri
	typedef void (APIENTRYP PFNGLGETFRAMEBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint *params);
	GLAPI PFNGLGETFRAMEBUFFERPARAMETERIVPROC _glGetFramebufferParameteriv;
#define glGetFramebufferParameteriv _glGetFramebufferParameteriv
	typedef void (APIENTRYP PFNGLGETINTERNALFORMATI64VPROC)(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint64 *params);
	GLAPI PFNGLGETINTERNALFORMATI64VPROC _glGetInternalformati64v;
#define glGetInternalformati64v _glGetInternalformati64v
	typedef void (APIENTRYP PFNGLINVALIDATETEXSUBIMAGEPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);
	GLAPI PFNGLINVALIDATETEXSUBIMAGEPROC _glInvalidateTexSubImage;
#define glInvalidateTexSubImage _glInvalidateTexSubImage
	typedef void (APIENTRYP PFNGLINVALIDATETEXIMAGEPROC)(GLuint texture, GLint level);
	GLAPI PFNGLINVALIDATETEXIMAGEPROC _glInvalidateTexImage;
#define glInvalidateTexImage _glInvalidateTexImage
	typedef void (APIENTRYP PFNGLINVALIDATEBUFFERSUBDATAPROC)(GLuint buffer, GLintptr offset, GLsizeiptr length);
	GLAPI PFNGLINVALIDATEBUFFERSUBDATAPROC _glInvalidateBufferSubData;
#define glInvalidateBufferSubData _glInvalidateBufferSubData
	typedef void (APIENTRYP PFNGLINVALIDATEBUFFERDATAPROC)(GLuint buffer);
	GLAPI PFNGLINVALIDATEBUFFERDATAPROC _glInvalidateBufferData;
#define glInvalidateBufferData _glInvalidateBufferData
	typedef void (APIENTRYP PFNGLINVALIDATEFRAMEBUFFERPROC)(GLenum target, GLsizei numAttachments, const GLenum *attachments);
	GLAPI PFNGLINVALIDATEFRAMEBUFFERPROC _glInvalidateFramebuffer;
#define glInvalidateFramebuffer _glInvalidateFramebuffer
	typedef void (APIENTRYP PFNGLINVALIDATESUBFRAMEBUFFERPROC)(GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
	GLAPI PFNGLINVALIDATESUBFRAMEBUFFERPROC _glInvalidateSubFramebuffer;
#define glInvalidateSubFramebuffer _glInvalidateSubFramebuffer
	typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSINDIRECTPROC)(GLenum mode, const void *indirect, GLsizei drawcount, GLsizei stride);
	GLAPI PFNGLMULTIDRAWARRAYSINDIRECTPROC _glMultiDrawArraysIndirect;
#define glMultiDrawArraysIndirect _glMultiDrawArraysIndirect
	typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSINDIRECTPROC)(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride);
	GLAPI PFNGLMULTIDRAWELEMENTSINDIRECTPROC _glMultiDrawElementsIndirect;
#define glMultiDrawElementsIndirect _glMultiDrawElementsIndirect
	typedef void (APIENTRYP PFNGLGETPROGRAMINTERFACEIVPROC)(GLuint program, GLenum programInterface, GLenum pname, GLint *params);
	GLAPI PFNGLGETPROGRAMINTERFACEIVPROC _glGetProgramInterfaceiv;
#define glGetProgramInterfaceiv _glGetProgramInterfaceiv
	typedef GLuint(APIENTRYP PFNGLGETPROGRAMRESOURCEINDEXPROC)(GLuint program, GLenum programInterface, const GLchar *name);
	GLAPI PFNGLGETPROGRAMRESOURCEINDEXPROC _glGetProgramResourceIndex;
#define glGetProgramResourceIndex _glGetProgramResourceIndex
	typedef void (APIENTRYP PFNGLGETPROGRAMRESOURCENAMEPROC)(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
	GLAPI PFNGLGETPROGRAMRESOURCENAMEPROC _glGetProgramResourceName;
#define glGetProgramResourceName _glGetProgramResourceName
	typedef void (APIENTRYP PFNGLGETPROGRAMRESOURCEIVPROC)(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params);
	GLAPI PFNGLGETPROGRAMRESOURCEIVPROC _glGetProgramResourceiv;
#define glGetProgramResourceiv _glGetProgramResourceiv
	typedef GLint(APIENTRYP PFNGLGETPROGRAMRESOURCELOCATIONPROC)(GLuint program, GLenum programInterface, const GLchar *name);
	GLAPI PFNGLGETPROGRAMRESOURCELOCATIONPROC _glGetProgramResourceLocation;
#define glGetProgramResourceLocation _glGetProgramResourceLocation
	typedef GLint(APIENTRYP PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC)(GLuint program, GLenum programInterface, const GLchar *name);
	GLAPI PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC _glGetProgramResourceLocationIndex;
#define glGetProgramResourceLocationIndex _glGetProgramResourceLocationIndex
	typedef void (APIENTRYP PFNGLSHADERSTORAGEBLOCKBINDINGPROC)(GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);
	GLAPI PFNGLSHADERSTORAGEBLOCKBINDINGPROC _glShaderStorageBlockBinding;
#define glShaderStorageBlockBinding _glShaderStorageBlockBinding
	typedef void (APIENTRYP PFNGLTEXBUFFERRANGEPROC)(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
	GLAPI PFNGLTEXBUFFERRANGEPROC _glTexBufferRange;
#define glTexBufferRange _glTexBufferRange
	typedef void (APIENTRYP PFNGLTEXSTORAGE2DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
	GLAPI PFNGLTEXSTORAGE2DMULTISAMPLEPROC _glTexStorage2DMultisample;
#define glTexStorage2DMultisample _glTexStorage2DMultisample
	typedef void (APIENTRYP PFNGLTEXSTORAGE3DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
	GLAPI PFNGLTEXSTORAGE3DMULTISAMPLEPROC _glTexStorage3DMultisample;
#define glTexStorage3DMultisample _glTexStorage3DMultisample
	typedef void (APIENTRYP PFNGLTEXTUREVIEWPROC)(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);
	GLAPI PFNGLTEXTUREVIEWPROC _glTextureView;
#define glTextureView _glTextureView
	typedef void (APIENTRYP PFNGLBINDVERTEXBUFFERPROC)(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
	GLAPI PFNGLBINDVERTEXBUFFERPROC _glBindVertexBuffer;
#define glBindVertexBuffer _glBindVertexBuffer
	typedef void (APIENTRYP PFNGLVERTEXATTRIBFORMATPROC)(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
	GLAPI PFNGLVERTEXATTRIBFORMATPROC _glVertexAttribFormat;
#define glVertexAttribFormat _glVertexAttribFormat
	typedef void (APIENTRYP PFNGLVERTEXATTRIBIFORMATPROC)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
	GLAPI PFNGLVERTEXATTRIBIFORMATPROC _glVertexAttribIFormat;
#define glVertexAttribIFormat _glVertexAttribIFormat
	typedef void (APIENTRYP PFNGLVERTEXATTRIBLFORMATPROC)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
	GLAPI PFNGLVERTEXATTRIBLFORMATPROC _glVertexAttribLFormat;
#define glVertexAttribLFormat _glVertexAttribLFormat
	typedef void (APIENTRYP PFNGLVERTEXATTRIBBINDINGPROC)(GLuint attribindex, GLuint bindingindex);
	GLAPI PFNGLVERTEXATTRIBBINDINGPROC _glVertexAttribBinding;
#define glVertexAttribBinding _glVertexAttribBinding
	typedef void (APIENTRYP PFNGLVERTEXBINDINGDIVISORPROC)(GLuint bindingindex, GLuint divisor);
	GLAPI PFNGLVERTEXBINDINGDIVISORPROC _glVertexBindingDivisor;
#define glVertexBindingDivisor _glVertexBindingDivisor
	typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLPROC)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
	GLAPI PFNGLDEBUGMESSAGECONTROLPROC _glDebugMessageControl;
#define glDebugMessageControl _glDebugMessageControl
	typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
	GLAPI PFNGLDEBUGMESSAGEINSERTPROC _glDebugMessageInsert;
#define glDebugMessageInsert _glDebugMessageInsert
	typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback, const void *userParam);
	GLAPI PFNGLDEBUGMESSAGECALLBACKPROC _glDebugMessageCallback;
#define glDebugMessageCallback _glDebugMessageCallback
	typedef GLuint(APIENTRYP PFNGLGETDEBUGMESSAGELOGPROC)(GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
	GLAPI PFNGLGETDEBUGMESSAGELOGPROC _glGetDebugMessageLog;
#define glGetDebugMessageLog _glGetDebugMessageLog
	typedef void (APIENTRYP PFNGLPUSHDEBUGGROUPPROC)(GLenum source, GLuint id, GLsizei length, const GLchar *message);
	GLAPI PFNGLPUSHDEBUGGROUPPROC _glPushDebugGroup;
#define glPushDebugGroup _glPushDebugGroup
	typedef void (APIENTRYP PFNGLPOPDEBUGGROUPPROC)(void);
	GLAPI PFNGLPOPDEBUGGROUPPROC _glPopDebugGroup;
#define glPopDebugGroup _glPopDebugGroup
	typedef void (APIENTRYP PFNGLOBJECTLABELPROC)(GLenum identifier, GLuint name, GLsizei length, const GLchar *label);
	GLAPI PFNGLOBJECTLABELPROC _glObjectLabel;
#define glObjectLabel _glObjectLabel
	typedef void (APIENTRYP PFNGLGETOBJECTLABELPROC)(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label);
	GLAPI PFNGLGETOBJECTLABELPROC _glGetObjectLabel;
#define glGetObjectLabel _glGetObjectLabel
	typedef void (APIENTRYP PFNGLOBJECTPTRLABELPROC)(const void *ptr, GLsizei length, const GLchar *label);
	GLAPI PFNGLOBJECTPTRLABELPROC _glObjectPtrLabel;
#define glObjectPtrLabel _glObjectPtrLabel
	typedef void (APIENTRYP PFNGLGETOBJECTPTRLABELPROC)(const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label);
	GLAPI PFNGLGETOBJECTPTRLABELPROC _glGetObjectPtrLabel;
#define glGetObjectPtrLabel _glGetObjectPtrLabel
#endif
#ifndef GL_VERSION_4_4
#define GL_VERSION_4_4 1
	GLAPI int _GL_VERSION_4_4;
	typedef void (APIENTRYP PFNGLBUFFERSTORAGEPROC)(GLenum target, GLsizeiptr size, const void *data, GLbitfield flags);
	GLAPI PFNGLBUFFERSTORAGEPROC _glBufferStorage;
#define glBufferStorage _glBufferStorage
	typedef void (APIENTRYP PFNGLCLEARTEXIMAGEPROC)(GLuint texture, GLint level, GLenum format, GLenum type, const void *data);
	GLAPI PFNGLCLEARTEXIMAGEPROC _glClearTexImage;
#define glClearTexImage _glClearTexImage
	typedef void (APIENTRYP PFNGLCLEARTEXSUBIMAGEPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *data);
	GLAPI PFNGLCLEARTEXSUBIMAGEPROC _glClearTexSubImage;
#define glClearTexSubImage _glClearTexSubImage
	typedef void (APIENTRYP PFNGLBINDBUFFERSBASEPROC)(GLenum target, GLuint first, GLsizei count, const GLuint *buffers);
	GLAPI PFNGLBINDBUFFERSBASEPROC _glBindBuffersBase;
#define glBindBuffersBase _glBindBuffersBase
	typedef void (APIENTRYP PFNGLBINDBUFFERSRANGEPROC)(GLenum target, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizeiptr *sizes);
	GLAPI PFNGLBINDBUFFERSRANGEPROC _glBindBuffersRange;
#define glBindBuffersRange _glBindBuffersRange
	typedef void (APIENTRYP PFNGLBINDTEXTURESPROC)(GLuint first, GLsizei count, const GLuint *textures);
	GLAPI PFNGLBINDTEXTURESPROC _glBindTextures;
#define glBindTextures _glBindTextures
	typedef void (APIENTRYP PFNGLBINDSAMPLERSPROC)(GLuint first, GLsizei count, const GLuint *samplers);
	GLAPI PFNGLBINDSAMPLERSPROC _glBindSamplers;
#define glBindSamplers _glBindSamplers
	typedef void (APIENTRYP PFNGLBINDIMAGETEXTURESPROC)(GLuint first, GLsizei count, const GLuint *textures);
	GLAPI PFNGLBINDIMAGETEXTURESPROC _glBindImageTextures;
#define glBindImageTextures _glBindImageTextures
	typedef void (APIENTRYP PFNGLBINDVERTEXBUFFERSPROC)(GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides);
	GLAPI PFNGLBINDVERTEXBUFFERSPROC _glBindVertexBuffers;
#define glBindVertexBuffers _glBindVertexBuffers
#endif

#ifdef __cplusplus
}
#endif

#endif

extern int LoadExtensions();
inline void* LoadExtension(const char*);

/*
	OpenGL context creation/configuration
*/
#include <Core/Misc/Defines/PlatformInclude.hpp>
#if defined( OS_WINDOWS )
#if !defined(__gl_h_)
#define APIENTRYP WINAPI *
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef char GLchar;
#endif

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_SAMPLE_BUFFERS_ARB 0x2041
#define WGL_SAMPLES_ARB 0x2042
#define WGL_TYPE_RGBA_ARB 0x202B

typedef HGLRC(WINAPI * WGLCREATECONTEXTATTRIBSARB) (HDC hDC, HGLRC hShareContext, const int* attribList);
extern WGLCREATECONTEXTATTRIBSARB wglCreateContextAttribsARB;
typedef BOOL(WINAPI * WGLCHOOSEPIXELFORMATARB) (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
extern WGLCHOOSEPIXELFORMATARB wglChoosePixelFormatARB;
typedef BOOL(WINAPI * WGLSWAPINTERVALEXT) (int interval);
extern WGLSWAPINTERVALEXT wglSwapIntervalEXT;
#elif defined( OS_LINUX )
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#define GLX_CONTEXT_PROFILE_MASK_ARB 0x9126
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

typedef GLXContext(*GLXCREATECONTEXTATTRIBSARB) (Display* dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int* attrib_list);
extern GLXCREATECONTEXTATTRIBSARB glXCreateContextAttribsARB;
typedef int(*GLXSWAPINTERVALSGI) (int interval);
extern GLXSWAPINTERVALSGI glXSwapIntervalSGI;
#endif

///*
//	Return values/parameters
//*/
//
//#define GL_MAJOR_VERSION 0x821B
//#define GL_MINOR_VERSION 0x821C
//
///*
//	Shaders
//*/
//
//#define GL_FRAGMENT_SHADER 0x8B30
//#define GL_VERTEX_SHADER 0x8B31
//#define GL_GEOMETRY_SHADER 0x8DD9
//#define GL_COMPILE_STATUS 0x8B81
//#define GL_INFO_LOG_LENGTH 0x8B84
//
//typedef void (APIENTRYP GLCOMPILESHADER) (GLuint shader);
//extern GLCOMPILESHADER glCompileShader;
//typedef GLuint(APIENTRYP GLCREATESHADER) (GLenum type);
//extern GLCREATESHADER glCreateShader;
//typedef void (APIENTRYP GLDELETESHADER) (GLuint shader);
//extern GLDELETESHADER glDeleteShader;
//typedef void (APIENTRYP GLGETSHADERIV) (GLuint shader, GLenum pname, GLint* params);
//extern GLGETSHADERIV glGetShaderiv;
//typedef void (APIENTRYP GLGETSHADERINFOLOG) (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
//extern GLGETSHADERINFOLOG glGetShaderInfoLog;
//typedef void (APIENTRYP GLSHADERSOURCE) (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
//extern GLSHADERSOURCE glShaderSource;
//
//#define GL_LINK_STATUS 0x8B82
//
//typedef GLuint(APIENTRYP GLCREATEPROGRAM) (void);
//extern GLCREATEPROGRAM glCreateProgram;
//typedef void (APIENTRYP GLDELETEPROGRAM) (GLuint program);
//extern GLDELETEPROGRAM glDeleteProgram;
//typedef void (APIENTRYP GLUSEPROGRAM) (GLuint program);
//extern GLUSEPROGRAM glUseProgram;
//typedef void (APIENTRYP GLATTACHSHADER) (GLuint program, GLuint shader);
//extern GLATTACHSHADER glAttachShader;
//typedef void (APIENTRYP GLLINKPROGRAM) (GLuint program);
//extern GLLINKPROGRAM glLinkProgram;
//typedef void (APIENTRYP GLGETPROGRAMIV) (GLuint program, GLenum pname, GLint* params);
//extern GLGETPROGRAMIV glGetProgramiv;
//typedef void (APIENTRYP GLGETPROGRAMINFOLOG) (GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
//extern GLGETPROGRAMINFOLOG glGetProgramInfoLog;
//typedef GLint(APIENTRYP GLGETATTRIBLOCATION) (GLuint program, const GLchar* name);
//extern GLGETATTRIBLOCATION glGetAttribLocation;
//typedef GLint(APIENTRYP GLGETUNIFORMLOCATION) (GLuint program, const GLchar* name);
//extern GLGETUNIFORMLOCATION glGetUniformLocation;
//
///*
//	Uniforms
//*/
//
//typedef void (APIENTRYP GLUNIFORM1F) (GLint location, GLfloat v0);
//extern GLUNIFORM1F glUniform1f;
//typedef void (APIENTRYP GLUNIFORM2F) (GLint location, GLfloat v0, GLfloat v1);
//extern GLUNIFORM2F glUniform2f;
//typedef void (APIENTRYP GLUNIFORM3F) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
//extern GLUNIFORM3F glUniform3f;
//typedef void (APIENTRYP GLUNIFORM4F) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
//extern GLUNIFORM4F glUniform4f;
//typedef void (APIENTRYP GLUNIFORM1I) (GLint location, GLint v0);
//extern GLUNIFORM1I glUniform1i;
//typedef void (APIENTRYP GLUNIFORM2I) (GLint location, GLint v0, GLint v1);
//extern GLUNIFORM2I glUniform2i;
//typedef void (APIENTRYP GLUNIFORM3I) (GLint location, GLint v0, GLint v1, GLint v2);
//extern GLUNIFORM3I glUniform3i;
//typedef void (APIENTRYP GLUNIFORM4I) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
//extern GLUNIFORM4I glUniform4i;
//typedef void (APIENTRYP GLUNIFORM1UI) (GLuint location, GLuint v0);
//extern GLUNIFORM1UI glUniform1ui;
//typedef void (APIENTRYP GLUNIFORM2UI) (GLuint location, GLuint v0, GLuint v1);
//extern GLUNIFORM2UI glUniform2ui;
//typedef void (APIENTRYP GLUNIFORM3UI) (GLuint location, GLuint v0, GLuint v1, GLuint v2);
//extern GLUNIFORM3UI glUniform3ui;
//typedef void (APIENTRYP GLUNIFORM4UI) (GLuint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
//extern GLUNIFORM4UI glUniform4ui;
//typedef void (APIENTRYP GLUNIFORM1FV) (GLint location, GLsizei count, const GLfloat* value);
//extern GLUNIFORM1FV glUniform1fv;
//typedef void (APIENTRYP GLUNIFORM2FV) (GLint location, GLsizei count, const GLfloat* value);
//extern GLUNIFORM2FV glUniform2fv;
//typedef void (APIENTRYP GLUNIFORM3FV) (GLint location, GLsizei count, const GLfloat* value);
//extern GLUNIFORM3FV glUniform3fv;
//typedef void (APIENTRYP GLUNIFORM4FV) (GLint location, GLsizei count, const GLfloat* value);
//extern GLUNIFORM4FV glUniform4fv;
//typedef void (APIENTRYP GLUNIFORM1IV) (GLint location, GLsizei count, const GLint* value);
//extern GLUNIFORM1IV glUniform1iv;
//typedef void (APIENTRYP GLUNIFORM2IV) (GLint location, GLsizei count, const GLint* value);
//extern GLUNIFORM2IV glUniform2iv;
//typedef void (APIENTRYP GLUNIFORM3IV) (GLint location, GLsizei count, const GLint* value);
//extern GLUNIFORM3IV glUniform3iv;
//typedef void (APIENTRYP GLUNIFORM4IV) (GLint location, GLsizei count, const GLint* value);
//extern GLUNIFORM4IV glUniform4iv;
//typedef void (APIENTRYP GLUNIFORM1UIV) (GLuint location, GLsizei count, const GLuint* value);
//extern GLUNIFORM1UIV glUniform1uiv;
//typedef void (APIENTRYP GLUNIFORM2UIV) (GLuint location, GLsizei count, const GLuint* value);
//extern GLUNIFORM2UIV glUniform2uiv;
//typedef void (APIENTRYP GLUNIFORM3UIV) (GLuint location, GLsizei count, const GLuint* value);
//extern GLUNIFORM3UIV glUniform3uiv;
//typedef void (APIENTRYP GLUNIFORM4UIV) (GLuint location, GLsizei count, const GLuint* value);
//extern GLUNIFORM4UIV glUniform4uiv;
//typedef void (APIENTRYP GLUNIFORMMATRIX2FV) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//extern GLUNIFORMMATRIX2FV glUniformMatrix2fv;
//typedef void (APIENTRYP GLUNIFORMMATRIX3FV) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//extern GLUNIFORMMATRIX3FV glUniformMatrix3fv;
//typedef void (APIENTRYP GLUNIFORMMATRIX4FV) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//extern GLUNIFORMMATRIX4FV glUniformMatrix4fv;
//typedef void (APIENTRYP GLUNIFORMMATRIX2X3FV) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//extern GLUNIFORMMATRIX2X3FV glUniformMatrix2x3fv;
//typedef void (APIENTRYP GLUNIFORMMATRIX3X2FV) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//extern GLUNIFORMMATRIX3X2FV glUniformMatrix3x2fv;
//typedef void (APIENTRYP GLUNIFORMMATRIX2X4FV) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//extern GLUNIFORMMATRIX2X4FV glUniformMatrix2x4fv;
//typedef void (APIENTRYP GLUNIFORMMATRIX4X2FV) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//extern GLUNIFORMMATRIX4X2FV glUniformMatrix4x2fv;
//typedef void (APIENTRYP GLUNIFORMMATRIX3X4FV) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//extern GLUNIFORMMATRIX3X4FV glUniformMatrix3x4fv;
//typedef void (APIENTRYP GLUNIFORMMATRIX4X3FV) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//extern GLUNIFORMMATRIX4X3FV glUniformMatrix4x3fv;
//
///*
//	VBOs
//*/
//
//#define GL_STREAM_DRAW 0x88E0
//#define GL_STREAM_READ 0x88E1
//#define GL_STREAM_COPY 0x88E2
//#define GL_STATIC_DRAW 0x88E4
//#define GL_STATIC_READ 0x88E5
//#define GL_STATIC_COPY 0x88E6
//#define GL_DYNAMIC_DRAW 0x88E8
//#define GL_DYNAMIC_READ 0x88E9
//#define GL_DYNAMIC_COPY 0x88EA
//
//#define GL_ARRAY_BUFFER 0x8892
//#define GL_ELEMENT_ARRAY_BUFFER 0x8893
//
//typedef void (APIENTRYP GLGENBUFFERS) (GLsizei n, GLuint* buffers);
//extern GLGENBUFFERS glGenBuffers;
//typedef void (APIENTRYP GLDELETEBUFFERS) (GLsizei n, const GLuint* buffers);
//extern GLDELETEBUFFERS glDeleteBuffers;
//typedef void (APIENTRYP GLBINDBUFFER)  (GLenum target, GLuint buffer);
//extern GLBINDBUFFER glBindBuffer;
//typedef void (APIENTRYP GLBUFFERDATA) (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
//extern GLBUFFERDATA glBufferData;
//typedef void (APIENTRYP GLBUFFERSUBDATA) (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
//extern GLBUFFERSUBDATA glBufferSubData;
//typedef void (APIENTRYP GLGETBUFFERSUBDATA) (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid* data);
//extern GLGETBUFFERSUBDATA glGetBufferSubData;
//
///*
//	VAOs
//*/
//
//typedef void (APIENTRYP GLGENVERTEXARRAYS) (GLsizei n, GLuint* arrays);
//extern GLGENVERTEXARRAYS glGenVertexArrays;
//typedef void (APIENTRYP GLDELETEVERTEXARRAYS) (GLsizei n, const GLuint* arrays);
//extern GLDELETEVERTEXARRAYS glDeleteVertexArrays;
//typedef void (APIENTRYP GLBINDVERTEXARRAY) (GLuint array);
//extern GLBINDVERTEXARRAY glBindVertexArray;
//
//typedef void (APIENTRYP GLENABLEVERTEXATTRIBARRAY) (GLuint index);
//extern GLENABLEVERTEXATTRIBARRAY glEnableVertexAttribArray;
//typedef void (APIENTRYP GLVERTEXATTRIBPOINTER) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
//extern GLVERTEXATTRIBPOINTER glVertexAttribPointer;
//
///*
//	Textures
//*/
//
//#define GL_TEXTURE_WRAP_R 0x8072
//
//#define GL_CLAMP_TO_EDGE 0x812F
//#define GL_CLAMP_TO_BORDER 0x812D
//#define GL_MIRRORED_REPEAT 0x8370
//
//#define GL_TEXTURE0 0x84C0
//#define GL_TEXTURE1 0x84C1
//#define GL_TEXTURE2 0x84C2
//#define GL_TEXTURE3 0x84C3
//#define GL_TEXTURE4 0x84C4
//#define GL_TEXTURE5 0x84C5
//#define GL_TEXTURE6 0x84C6
//#define GL_TEXTURE7 0x84C7
//#define GL_TEXTURE8 0x84C8
//#define GL_TEXTURE9 0x84C9
//#define GL_TEXTURE10 0x84CA
//#define GL_TEXTURE11 0x84CB
//#define GL_TEXTURE12 0x84CC
//#define GL_TEXTURE13 0x84CD
//#define GL_TEXTURE14 0x84CE
//#define GL_TEXTURE15 0x84CF
//#define GL_TEXTURE16 0x84D0
//#define GL_TEXTURE17 0x84D1
//#define GL_TEXTURE18 0x84D2
//#define GL_TEXTURE19 0x84D3
//#define GL_TEXTURE20 0x84D4
//#define GL_TEXTURE21 0x84D5
//#define GL_TEXTURE22 0x84D6
//#define GL_TEXTURE23 0x84D7
//#define GL_TEXTURE24 0x84D8
//#define GL_TEXTURE25 0x84D9
//#define GL_TEXTURE26 0x84DA
//#define GL_TEXTURE27 0x84DB
//#define GL_TEXTURE28 0x84DC
//#define GL_TEXTURE29 0x84DD
//#define GL_TEXTURE30 0x84DE
//#define GL_TEXTURE31 0x84DF
//
//typedef void (APIENTRYP GLGENERATEMIPMAP) (GLenum target);
//extern GLGENERATEMIPMAP glGenerateMipmap;
//
//#ifndef GL_VERSION_1_3
//typedef void (APIENTRYP GLACTIVETEXTURE) (GLenum texture);
//extern GLACTIVETEXTURE glActiveTexture;
//#endif
//
///*
//	Data types
//*/
//
//#define GL_UNSIGNED_BYTE_3_3_2 0x8032
//#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
//#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
//#define GL_UNSIGNED_INT_8_8_8_8 0x8035
//#define GL_UNSIGNED_INT_10_10_10_2 0x8036
//#define GL_UNSIGNED_BYTE_2_3_3_REV 0x8362
//#define GL_UNSIGNED_SHORT_5_6_5 0x8363
//#define GL_UNSIGNED_SHORT_5_6_5_REV 0x8364
//#define GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
//#define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366
//#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
//#define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
//
//#define GL_COMPRESSED_RED 0x8225
//#define GL_COMPRESSED_RG 0x8226
//#define GL_COMPRESSED_RED_RGTC1 0x8DBB
//#define GL_COMPRESSED_SIGNED_RED_RGTC1 0x8DBC
//#define GL_COMPRESSED_RG_RGTC2 0x8DBD
//#define GL_COMPRESSED_SIGNED_RG_RGTC2 0x8DBE
//#define GL_COMPRESSED_RGB 0x84ED
//#define GL_COMPRESSED_RGBA 0x84EE
//#define GL_SRGB 0x8C40
//#define GL_SRGB8 0x8C41
//#define GL_SRGB_ALPHA 0x8C42
//#define GL_SRGB8_ALPHA8 0x8C43
//#define GL_COMPRESSED_SRGB 0x8C48
//#define GL_COMPRESSED_SRGB_ALPHA 0x8C49
//#define GL_DEPTH_STENCIL 0x84F9
//#define GL_DEPTH24_STENCIL8 0x88F0
//#define GL_DEPTH_COMPONENT32F 0x8CAC
//#define GL_DEPTH32F_STENCIL8 0x8CAD
//#define GL_DEPTH_COMPONENT16 0x81A5
//#define GL_DEPTH_COMPONENT24 0x81A6
//#define GL_DEPTH_COMPONENT32 0x81A7
//#define GL_RG 0x8227
//#define GL_R8 0x8229
//#define GL_R16 0x822A
//#define GL_RG8 0x822B
//#define GL_RG16 0x822C
//#define GL_R16F 0x822D
//#define GL_R32F 0x822E
//#define GL_RG16F 0x822F
//#define GL_RG32F 0x8230
//#define GL_R8I 0x8231
//#define GL_R8UI 0x8232
//#define GL_R16I 0x8233
//#define GL_R16UI 0x8234
//#define GL_R32I 0x8235
//#define GL_R32UI 0x8236
//#define GL_RG8I 0x8237
//#define GL_RG8UI 0x8238
//#define GL_RG16I 0x8239
//#define GL_RG16UI 0x823A
//#define GL_RG32I 0x823B
//#define GL_RG32UI 0x823C
//#define GL_RG_SNORM 0x8F91
//#define GL_RGB_SNORM 0x8F92
//#define GL_RGBA_SNORM 0x8F93
//#define GL_R8_SNORM 0x8F94
//#define GL_RG8_SNORM 0x8F95
//#define GL_RGB8_SNORM 0x8F96
//#define GL_RGBA8_SNORM 0x8F97
//#define GL_R16_SNORM 0x8F98
//#define GL_RG16_SNORM 0x8F99
//#define GL_RGB16_SNORM 0x8F9A
//#define GL_RGBA16_SNORM 0x8F9B
//#define GL_RGBA32F 0x8814
//#define GL_RGB32F 0x8815
//#define GL_RGBA16F 0x881A
//#define GL_RGB16F 0x881B
//#define GL_RGBA32UI 0x8D70
//#define GL_RGB32UI 0x8D71
//#define GL_RGBA16UI 0x8D76
//#define GL_RGB16UI 0x8D77
//#define GL_RGBA8UI 0x8D7C
//#define GL_RGB8UI 0x8D7D
//#define GL_RGBA32I 0x8D82
//#define GL_RGB32I 0x8D83
//#define GL_RGBA16I 0x8D88
//#define GL_RGB16I  0x8D89
//#define GL_RGBA8I 0x8D8E
//#define GL_RGB8I 0x8D8F
//#define GL_RGB9_E5 0x8C3D
//#define GL_BGR 0x80E0
//#define GL_BGRA 0x80E1
//
///*
//	Frame buffers
//*/
//
//#define GL_DRAW_FRAMEBUFFER 0x8CA9
//
//#define GL_COLOR_ATTACHMENT0 0x8CE0
//#define GL_COLOR_ATTACHMENT1 0x8CE1
//#define GL_COLOR_ATTACHMENT2 0x8CE2
//#define GL_COLOR_ATTACHMENT3 0x8CE3
//#define GL_COLOR_ATTACHMENT4 0x8CE4
//#define GL_COLOR_ATTACHMENT5 0x8CE5
//#define GL_COLOR_ATTACHMENT6 0x8CE6
//#define GL_COLOR_ATTACHMENT7 0x8CE7
//#define GL_COLOR_ATTACHMENT8 0x8CE8
//#define GL_COLOR_ATTACHMENT9 0x8CE9
//#define GL_COLOR_ATTACHMENT10 0x8CEA
//#define GL_COLOR_ATTACHMENT11 0x8CEB
//#define GL_COLOR_ATTACHMENT12 0x8CEC
//#define GL_COLOR_ATTACHMENT13 0x8CED
//#define GL_COLOR_ATTACHMENT14 0x8CEE
//#define GL_COLOR_ATTACHMENT15 0x8CEF
//#define GL_DEPTH_ATTACHMENT 0x8D00
//#define GL_STENCIL_ATTACHMENT 0x8D20
//
//#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
//#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
//
//#define GL_FRAMEBUFFER_BINDING 0x8CA6
//#define GL_DRAW_FRAMEBUFFER_BINDING GL_FRAMEBUFFER_BINDING
//#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
//
//typedef void (APIENTRYP GLGENFRAMEBUFFERS) (GLsizei n, GLuint* framebuffers);
//extern GLGENFRAMEBUFFERS glGenFramebuffers;
//typedef void (APIENTRYP GLDELETEFRAMEBUFFERS) (GLsizei n, const GLuint* framebuffers);
//extern GLDELETEFRAMEBUFFERS glDeleteFramebuffers;
//typedef void (APIENTRYP GLFRAMEBUFFERTEXTURE2D) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
//extern GLFRAMEBUFFERTEXTURE2D glFramebufferTexture2D;
//typedef void (APIENTRYP GLFRAMEBUFFERRENDERBUFFER) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
//extern GLFRAMEBUFFERRENDERBUFFER glFramebufferRenderbuffer;
//typedef void (APIENTRYP GLBINDFRAMEBUFFER) (GLenum target, GLuint framebuffer);
//extern GLBINDFRAMEBUFFER glBindFramebuffer;
//typedef void (APIENTRYP GLGETFRAMEBUFFERATTACHMENTPARAMETERIV) (GLenum target, GLenum attachment, GLenum pname, GLint* params);
//extern GLGETFRAMEBUFFERATTACHMENTPARAMETERIV glGetFramebufferAttachmentParameteriv;
//typedef GLenum(APIENTRYP GLCHECKFRAMEBUFFERSTATUS) (GLenum target);
//extern GLCHECKFRAMEBUFFERSTATUS glCheckFramebufferStatus;
//
///*
//	Render buffers
//*/
//
//#define GL_RENDERBUFFER 0x8D41
//#define GL_MAX_RENDERBUFFER_SIZE 0x84E8
//
//typedef void (APIENTRYP GLGENRENDERBUFFERS) (GLsizei n, GLuint* renderbuffers);
//extern GLGENRENDERBUFFERS glGenRenderbuffers;
//typedef void (APIENTRYP GLDELETERENDERBUFFERS) (GLsizei n, const GLuint* renderbuffers);
//extern GLDELETERENDERBUFFERS glDeleteRenderbuffers;
//typedef void (APIENTRYP GLRENDERBUFFERSTORAGE) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
//extern GLRENDERBUFFERSTORAGE glRenderbufferStorage;
//typedef void (APIENTRYP GLBINDRENDERBUFFER) (GLenum target, GLuint renderbuffer);
//extern GLBINDRENDERBUFFER glBindRenderbuffer;
//
///*
//	Stencil test
//*/
//
//#define GL_INCR_WRAP 0x8507
//#define GL_DECR_WRAP 0x8508
//
///*
//	Transform feedback
//*/
//
//#define GL_INTERLEAVED_ATTRIBS 0x8C8C
//
//#define GL_RASTERIZER_DISCARD 0x8C89
//
//#define GL_TRANSFORM_FEEDBACK_BUFFER 0x8C8E
//
//typedef void (APIENTRYP GLTRANSFORMFEEDBACKVARYINGS) (GLuint program, GLsizei count, const char** varyings, GLenum bufferMode);
//extern GLTRANSFORMFEEDBACKVARYINGS glTransformFeedbackVaryings;
//typedef void (APIENTRYP GLBINDBUFFERBASE) (GLenum target, GLuint index, GLuint buffer);
//extern GLBINDBUFFERBASE glBindBufferBase;
//typedef void (APIENTRYP GLBEGINTRANSFORMFEEDBACK) (GLenum primitiveMode);
//extern GLBEGINTRANSFORMFEEDBACK glBeginTransformFeedback;
//typedef void (APIENTRYP GLENDTRANSFORMFEEDBACK) ();
//extern GLENDTRANSFORMFEEDBACK glEndTransformFeedback;

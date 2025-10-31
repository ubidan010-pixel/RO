#ifndef _ITF_3DS_MACROS_H_
#define _ITF_3DS_MACROS_H_

#include <nn/gx.h>

#ifndef ITF_FINAL
#define ITF_ENABLE_OGL_ERROR
#define ITF_ENABLE_GFX_ASSERT
//#define ITF_ENABLE_BREAK_ON_UNEMPLEMENTED_FUNC
#endif //ITF_FINAL

#ifdef ITF_ENABLE_GFX_ASSERT
#define GFX_ASSERT(cond, ...) if (!(cond))				\
	{													\
		NN_LOG(__VA_ARGS__);							\
		nn::dbg::Break(nn::dbg::BREAK_REASON_ASSERT);	\
	}
#else
#define GFX_ASSERT(cond, ...) ((void)0)
#endif

#ifdef ITF_ENABLE_OGL_ERROR
#define VERIFY_OGL_ERROR { \
		GLenum error = glGetError();										\
		GFX_ASSERT( error == GL_NO_ERROR, "OpenGL error : %x\n", error );	\
	}
#else
#define VERIFY_OGL_ERROR 
#endif

#ifdef ITF_ENABLE_OGL_ERROR
class OGLFunctionGuard
{
public:
	OGLFunctionGuard()
	{
		VERIFY_OGL_ERROR;
		GFX_ASSERT(ITF::Synchronize::getCurrentThreadId() == ITF::ThreadSettings::m_settings[ITF::eThreadId_mainThread].m_threadID,
			"All OpenGL functions must be called by the main thread");
	}
	~OGLFunctionGuard() {VERIFY_OGL_ERROR;}

};
#define START_OGL_FUNCTION OGLFunctionGuard _oglfunctionguard
#else
#define START_OGL_FUNCTION		((void)0)
#endif

#ifdef ITF_ENABLE_BREAK_ON_UNEMPLEMENTED_FUNC
#define GFX_NOT_IMPLEMENTED(funcName)	GFX_ASSERT(false, "Function : %s not implemented", #funcName)
#else
#define GFX_NOT_IMPLEMENTED(funcName)	((void)0)
#endif

#endif // _ITF_3DS_MACROS_H_
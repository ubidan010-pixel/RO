#ifndef ITF_MEMMNG_TRACER_PLATFORM_PS3_H_
#define ITF_MEMMNG_TRACER_PLATFORM_PS3_H_

namespace ITF
{

namespace MemMngDebug
{
	// Init to go through stack
	void InitMemoryTracer();

	//-------------------------------------------------------------------------
	// This function collects the current callstack up do a maximum depth of
	// maxDepth. It allows to skip the n first functions (usually we dont
	// want to have "SStackFramePS3::GetFirstFrame()" and
	// "MemMngDebug::GetCallStack" in our callstack, and possibly others as
	// well.
	//-------------------------------------------------------------------------

	int GetCallStack(void** pBuffer, int maxDepth, int skip, const void* firstFrame);

} // namespace MemMngDebug

} // namespace ITF
#endif // ITF_MEMMNG_TRACER_PLATFORM_PS3_H_

#include "precompiled_core.h"
#include <cell/sysmodule.h>
#include <sys/paths.h>
#include <cell/cell_fs.h>
#include "engine/lzo/minilzo.h"
#include "MemMngPrivate_PS3.h"

#include "MemMngTracerPlatform_PS3.h"
#include "MemMngTracer_PS3.h"


namespace ITF
{

//-----------------------------------------------------------------------------
// This is to check compiler version. Changing compiler may affect the ABI
// and possibly how the stack frame is organized.
// A warning will be issued if a different compiler version is detected than
// the one the stack walker has been designed to work on.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Available compilers
//-----------------------------------------------------------------------------

#define MEMMNG_COMPILER_PS3_UNKNOWN    		0
#define MEMMNG_COMPILER_PS3_GCC        		1
#define MEMMNG_COMPILER_PS3_SNC        		2

//-----------------------------------------------------------------------------
// Current compiler
//-----------------------------------------------------------------------------

// __SNC__ must be first, because __GNUC__ is also define with SNC for compatibility issues
#ifdef __SNC__
# define MEMMNG_COMPILER_CURRENT				MEMMNG_COMPILER_PS3_SNC
# define MEMMNG_COMPILER_VERSION				(0)
#elif defined (__GNUC__)
# define MEMMNG_COMPILER_CURRENT				MEMMNG_COMPILER_PS3_GCC
# define MEMMNG_COMPILER_VERSION				(__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
# define MEMMNG_COMPILER_CURRENT				MEMMNG_COMPILER_PS3_UNKNOWN
# define MEMMNG_COMPILER_VERSION				0
#endif

//-----------------------------------------------------------------------------
// Compiler for which the stack walker is compatible
//-----------------------------------------------------------------------------

#define MEMMNG_COMPILER_SUPPORTED				MEMMNG_COMPILER_PS3_GCC
#define MEMMNG_COMPILER_SUPPORTED_VERSION		(4 * 10000 + 0 * 100 + 2)

//-----------------------------------------------------------------------------
// Verification
//-----------------------------------------------------------------------------

#if MEMMNG_COMPILER_CURRENT == MEMMNG_COMPILER_PS3_GCC
  #if MEMMNG_COMPILER_CURRENT_VERSION != MEMMNG_COMPILER_SUPPORTED_VERSION
    # pragma error("Compiler version changed, verify the stalk walker is still valid for new compiler!")
  #endif
#elif MEMMNG_COMPILER_CURRENT == MEMMNG_COMPILER_PS3_SNC
  // no version check yet for snc compiler
#else
# pragma error("Compiler changed, verify the stalk walker is still valid for new compiler!")
#endif

//-----------------------------------------------------------------------------
// SStackFramePS3 structure used by GetCallStack to collect call stack
//-----------------------------------------------------------------------------

struct SStackFramePS3
{
    //PS3 uses 64-bits register and stores 64-bits on the stack
    union 
    {
	    u64							m_previousFrame;	//Address of previous stack frame
        struct
        {
            u32 m_highPrevFrame;
            SStackFramePS3 * m_lowPrevFrame;
        };
    };
    u64							m_CR;				//Conditional register (we don't need that)
    u64							m_LR;				//Link register (return address)

    //Call site is 4 bytes (1 instruction) before return address
    inline u32					GetCaller() const { return u32(m_LR) - 4; }
    inline const SStackFramePS3*	GetPreviousFrame() const { return reinterpret_cast<const SStackFramePS3*>( u32(m_previousFrame) ); }

    //Get current stack frame
    static const SStackFramePS3*	GetFirstFrame(const void* pFirstFrame);
};

__attribute__((__noinline__)) const SStackFramePS3* SStackFramePS3::GetFirstFrame(const void* pFirstFrame)
{
	const SStackFramePS3* pCurrentFrame;
    
    if (pFirstFrame)
    {
        pCurrentFrame = reinterpret_cast<const SStackFramePS3*>(pFirstFrame);
    }
    else
    {
        //Pointer to current stack frame is stores in r1
        #if MEMMNG_COMPILER_CURRENT == MEMMNG_COMPILER_PS3_SNC
            pCurrentFrame = static_cast<const SStackFramePS3*>(__builtin_frame_address());
        #else
            asm ( "mr %0, 1" : "=r"(pCurrentFrame));
        #endif
        
        pCurrentFrame = pCurrentFrame->GetPreviousFrame();
    }

	//In first stack frame, only m_previousFrame is valid, and we have to go back 1 frame
	//to get the address of the instruction that called this function
    return pCurrentFrame;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


namespace MemMngDebug
{
	static unsigned int	g_ModuleOffset;
	static unsigned int	g_ModuleSize;

#if MEMMNG_MEMORY_TRACER!=0
	// Init to go through stack
	void InitMemoryTracer()
	{
	}

	//-------------------------------------------------------------------------
	// This function collects the current callstack up do a maximum depth of
	// maxDepth. It allows to skip the n first functions (usually we dont
	// want to have "SStackFramePS3::GetFirstFrame()" and
	// "MemMngDebug::GetCallStack" in our callstack, and possibly others as
	// well.
	//-------------------------------------------------------------------------

	int GetCallStack(void** pBuffer, int maxDepth, int skip, const void* firstFrame)
	{
		int depth = 0;
        
#if MEMMNG_MEMORY_TRACER==1

		const SStackFramePS3* pStackFrame = SStackFramePS3::GetFirstFrame(firstFrame);

		//When we reach the entry point, pStackFrame will become NULL
		while(pStackFrame)
		{
			u32 caller = pStackFrame->GetCaller();
            bool caller_break = false;
            
            if (!firstFrame)
            {
                caller_break = (caller < 0x00010000 || caller >= 0x20000000 );
            }
            
            //Verify pFrame and caller address range validity
			if( ( u32(pStackFrame) & 0xD0000000) != 0xD0000000 || caller_break)
			{
				//Invalid pStackFrame pointer or caller address, stop here
				break;
			}

			if(skip > 0)
			{
				skip--;
			}
			else if(depth < maxDepth)
			{
				pBuffer[depth] = reinterpret_cast<void*>(caller);
				depth++;
			}


			pStackFrame = pStackFrame->GetPreviousFrame();
		}
#endif
		return depth;
	}
#endif
}

//-----------------------------------------------------------------------------

#if MEMMNG_MEMORY_TRACER==1
extern MemMng::MemCounts * g_MemUsageStat;

FILE_HANDLE  MemMngTracer::m_fileHandle = (FILE_HANDLE)-1;

void MemMngTracer::Init()
{
    strncpy(m_dumpHeader.m_dumptype, "ENGINE", sizeof(m_dumpHeader.m_dumptype));
    strncpy(m_dumpHeader.m_platform, "PS3", sizeof(m_dumpHeader.m_platform));
    strncpy(m_dumpHeader.m_reserved, "....", sizeof(m_dumpHeader.m_reserved));

    InitCommon();
}

void MemMngTracer::Close()
{
    CloseCommon();
    cellFsClose(m_fileHandle);
}


bool MemMngTracer::ValidateFileHandle()
{
    if (m_fileHandle == (u32)-1)
    {
        return 0 <= cellFsOpen(SYS_APP_HOME "/heap_trace_0.raw", CELL_FS_O_CREAT|CELL_FS_O_TRUNC|CELL_FS_O_WRONLY, (int *)&m_fileHandle, NULL, 0);
    }

    return true;
}

u32 MemMngTracer::WriteBlock(u32 size)
{
    int compsize = MAX_TRACER_SIZE;
    if (lzo1x_1_compress(m_buffer, size, m_compBuffer, (lzo_uint*)&compsize, m_workBuffer) != LZO_E_OK)
    {
        return 0;
    }

    u32 writtenBytes = 0;
    int writesize = compsize;
    u64 nbWritten = 0;
    cellFsWrite(m_fileHandle, &writesize, u64(sizeof(writesize)), &nbWritten);
    cellFsWrite(m_fileHandle, m_compBuffer, u64(compsize), &nbWritten);
    return writtenBytes;
}

void MemMngTracer::Dump(const char* dumpname)
{
	if(m_dump)
	{
		char filename[CELL_FS_MAX_FS_PATH_LENGTH];
		sprintf(filename, SYS_APP_HOME "/%s.dump", dumpname);
        hwFile file;
        file.open(filename, ITF_FILE_ATTR_WRITE);

        //Write header
        file.write(&m_dumpHeader, sizeof(m_dumpHeader));

        // bucket
        u32 temp = g_MemUsageStat->BucketMemUsed.current;
        file.write(&temp, sizeof(temp));
        temp = g_MemUsageStat->BucketMemUsed.peak;
        file.write(&temp, sizeof(temp));

        temp = g_MemUsageStat->BucketPageMemAllocated.current;
        file.write(&temp, sizeof(temp));
        temp = g_MemUsageStat->BucketPageMemAllocated.peak;
        file.write(&temp, sizeof(temp));

        // chunk
        bool chunk = false;
        file.write(&chunk, sizeof(chunk));

        int currentorder = m_order;
        file.write(&currentorder, sizeof(currentorder));

		int modulecount = 0;
        file.write(&modulecount, sizeof(modulecount));

        WriteThreadInfo(file);

        file.close();
	}
}

void MemMngTracer::LogCallStack(void* addr, int maxDepth)
{
	int callStack[MAX_CALLSTACK_DEPTH];
	int depth = Min(MemMngDebug::GetCallStack((void**)callStack, MAX_CALLSTACK_DEPTH, 2, NULL), maxDepth);
	if(depth)
	{
		int swapdepth = depth - 1;
		WriteBuffer(&swapdepth, sizeof(swapdepth));
		WriteBuffer(callStack+1, sizeof(int)*(depth - 1));
	}
	else
	{
        printf("Error 0 depth callstack\n");
		WriteBuffer(&depth, sizeof(depth));
	}
}

#endif

}
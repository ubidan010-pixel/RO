#include "precompiled_core.h"

#if !defined(ITF_FINAL) && defined(ITF_MICROSOFT)

#include <intrin.h>
#include <dbghelp.h>

#ifndef _ITF_MEMORY_H_
#include "core/memory/memory.h"
#endif //_ITF_MEMORY_H_

#ifndef _ITF_STACKWALKER_H_
#include "core/StackWalker.h"
#endif //_ITF_STACKWALKER_H_

#ifndef _ITF_OUTPUTDEBUGGER_H_
#include "core/OutputDebugger.h"
#endif //_ITF_OUTPUTDEBUGGER_H_


namespace ITF
{    
    StackWalker::StackWalker():CaptureStackBackTrace(NULL),m_hModuleKernel(NULL),m_initialized(bfalse)
    {
    }
    
    StackWalker::~StackWalker()    
    {
        FreeLibrary(m_hModuleKernel);
    }

    void StackWalker::initialize()
    {
        if (!m_initialized)
        {
            m_hModuleKernel = LoadLibraryA("kernel32.dll");
            if (!SymInitialize(GetCurrentProcess(), NULL, TRUE))
            {
                // SymInitialize failed
                OutputDebugger dbg;
                dbg << "SymInitialize returned error : " << (u32) GetLastError() <<"\n";
                return;
            }

            CaptureStackBackTrace = (CaptureStackBackTraceType)(GetProcAddress(m_hModuleKernel, "RtlCaptureStackBackTrace"));

            captureContext = (CaptureContextType)(GetProcAddress(m_hModuleKernel, "RtlCaptureContext"));

		    DWORD symbolOptions = SymGetOptions();	
		    SymSetOptions(symbolOptions | SYMOPT_LOAD_LINES);

            m_initialized = true;
        }
    }

    #if defined(ITF_WIN64)
        #define SIZEOFPTR 8
        #define X86X64ARCHITECTURE IMAGE_FILE_MACHINE_AMD64
    #elif defined(ITF_WIN32)
        #define SIZEOFPTR 4
        #define X86X64ARCHITECTURE IMAGE_FILE_MACHINE_I386
        #define BPREG ebp
    #endif // _M_IX86

    __declspec(noinline) DWORD_PTR getprogramcounterx86x64()
    {
        return (DWORD_PTR)_AddressOfReturnAddress();
    }

    u8 StackWalker::retrieveStackFast(u32 frameToSkip,uPtr stack[], uSize maxStackSize)
    {
        return (u8)CaptureStackBackTrace(frameToSkip, (ULONG)maxStackSize , (PVOID*) stack, NULL);
    }

    size_t StackWalker::retrieveStack(ProcessAddress* stack, 
        size_t maxStackSize, 
        size_t offset)
    {
        DWORD architecture;
        CONTEXT context;
        STACKFRAME64 frame;
        DWORD_PTR framePointer;
        DWORD_PTR programCounter;
	    HANDLE process = GetCurrentProcess();
	    HANDLE thread = GetCurrentThread();

        // Get the required values for initialization of the STACKFRAME64 
        // structure to be passed to StackWalk64(). Required fields are 
        // AddrPC and AddrFrame.
        #if defined(ITF_WIN64)
            architecture = X86X64ARCHITECTURE;

            CONTEXT ctxt;
            memset(&ctxt, 0, sizeof(CONTEXT));
            ctxt.ContextFlags = CONTEXT_FULL;
            (*captureContext)(&ctxt);

            programCounter = ctxt.Rip;
            framePointer = ctxt.Rsp; // or rbp ?
        #elif defined(ITF_WIN32)
            architecture = X86X64ARCHITECTURE;
            programCounter = getprogramcounterx86x64();
            // Get the frame pointer (aka base pointer)
            __asm mov [framePointer], BPREG // Get the frame pointer (aka base pointer)
        #else
            // If you want to retarget Visual Leak 
            // Detector to another processor
            // architecture then you'll need to provide 
            // architecture-specific code to
            // retrieve the current frame pointer 
            // and program counter in order to initialize
            // the STACKFRAME64 structure below.
            #error MemoryDebugger: Stack tracing not supported
        #endif // defined(_M_IX86) || defined(_M_X64)

        // Initialize the STACKFRAME64 structure.
        memset(&frame, 0x0, sizeof(frame));
        frame.AddrPC.Offset    = programCounter;
        frame.AddrPC.Mode      = AddrModeFlat;
        frame.AddrFrame.Offset = framePointer;
        frame.AddrFrame.Mode   = AddrModeFlat;

        // Walk the stack.
        size_t counter = 0;
        size_t depth = 0;
        while (depth < maxStackSize &&
               StackWalk64(architecture, process, thread, 
                   &frame, &context, NULL, SymFunctionTableAccess64, 
                   SymGetModuleBase64, NULL))
        {
            if (frame.AddrFrame.Offset != 0 && counter >= offset) 
            {
                stack[depth++] = frame.AddrPC.Offset;
            }
            counter++;
        }    

        return depth;
    }

    void StackWalker::fillDescriptorToFile(FILE* file, uPtr* stack,size_t maxStackSize)
    {
        HANDLE process = GetCurrentProcess();

        // get the line number details
        DWORD displacement32 = 0;
        IMAGEHLP_LINE64 line; 
        memset(&line, NULL, sizeof(IMAGEHLP_LINE64));
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
      //  ProcessAddressDescriptor desc;
        u32 stackCount = 0;
        char tmp[512];
        while (stackCount<maxStackSize)
        {
            uPtr address = stack[stackCount];

            ::SymGetLineFromAddr64(process, address, &displacement32, &line);
           
            {
                // get the symbol details
                DWORD64 displacement64 = 0;

                char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_PATH];
                PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);

                memset(symbol, NULL, sizeof(symbolBuffer));
                symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                symbol->MaxNameLen = MAX_PATH;
                BOOL symbolOk = SymFromAddr(process, address, &displacement64, symbol);

                if (symbolOk)
                {
                    SPRINTF_S(tmp,512,"%s(%d)%s\n",line.FileName,line.LineNumber,symbol->Name);
                    fputs(tmp,file);
                }
                else
                {
                    break;     
                }

            }
            stackCount++;
        }

    }

    void StackWalker::fillDescriptor(ProcessAddress address,ProcessAddressDescriptor& desc)
    {
	    HANDLE process = GetCurrentProcess();

		// get the line number details
		DWORD displacement32 = 0;
		IMAGEHLP_LINE64 line; 
		memset(&line, NULL, sizeof(IMAGEHLP_LINE64));
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
		BOOL lineOk = SymGetLineFromAddr64(process, address, &displacement32, &line);
        //seems some middle cleanup the Sym... 
        if (!lineOk && GetLastError() == ERROR_INVALID_HANDLE) //error if the sym is not init
        {
            SymInitialize(GetCurrentProcess(), NULL, TRUE);
            lineOk = SymGetLineFromAddr64(process, address, &displacement32, &line);
        }

        if (lineOk)
        {
		    // get the symbol details
            DWORD64 displacement64 = 0;

            char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_PATH];
            PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
		    
		    memset(symbol, NULL, sizeof(symbolBuffer));
		    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		    symbol->MaxNameLen = MAX_PATH;
		    BOOL symbolOk = SymFromAddr(process, address, &displacement64, symbol);
        
		    if (symbolOk)
		    {
                strncpy(desc.m_filename,line.FileName,sizeof(desc.m_filename));
                desc.m_line = line.LineNumber;
                strncpy(desc.m_symbolName, symbol->Name,sizeof(desc.m_symbolName));
                return;//succeed
		    }
        }
         

        //bad information;
        desc.m_filename[0] = 0;
        desc.m_line = 0;
        desc.m_symbolName[0] = 0;
        return;
    }

    void StackWalker::retrieveStack(char *_buffer, ux _bufferSize, size_t _offset)
    {
        static const u32 MAX_STACK_ADDRESS = 64;
        ProcessAddress stack[MAX_STACK_ADDRESS] = { 0 };

        uSize numAddress = uSize(retrieveStack(stack, MAX_STACK_ADDRESS, _offset));

        for(ux i = 0; i < numAddress; ++i)
        {
            ProcessAddressDescriptor desc;

            fillDescriptor(stack[i], desc);
            const char *basename = strrchr(desc.m_filename, '\\');

            ix len = _snprintf(_buffer, _bufferSize, "%s %s(%d)\n", desc.m_symbolName, basename ? basename + 1 : desc.m_filename, desc.m_line);

            if(len > 0)
            {
                _buffer += len;
                _bufferSize -= len;
            }
            else
                break;
        }

        _buffer[_bufferSize - 1] = '\0';
    }
}

#endif // ITF_IPAD, ...

#ifndef _ITF_STACKWALKER_H_
#define _ITF_STACKWALKER_H_

#ifndef _ITF_MEMORY_H_
#include "core/memory/memory.h"
#endif //_ITF_MEMORY_H_


#if !defined(ITF_FINAL) && defined(ITF_WINDOWS)
//#ifdef ITF_DEBUG_MEMORY

namespace ITF
{

    ///
    /// Description of a process address
    ///
    class ProcessAddressDescriptor
    {
    public:
        char m_filename[512];
        int m_line;
        char m_symbolName[512];
    };


    class StackWalker
    {
    public:
        StackWalker();

        virtual ~StackWalker();

        typedef USHORT (WINAPI *CaptureStackBackTraceType)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);

        typedef VOID (WINAPI *CaptureContextType)(__out PCONTEXT);
         ///
        /// represents a process address from which we can retrieve more information
        ///
        typedef u64 ProcessAddress; 

        ///
        /// initialize the stack walker
        ///
        void initialize();

        ///
        /// retrieve stack addresses
        /// @param stack is an array of process address 
        /// @param maxStackSize is the size of the array of process address
        /// @param offset from the stack top to start retrieval
        /// @return the number of addresses actually retrieved.
        ///
        size_t retrieveStack(ProcessAddress* stack, size_t maxStackSize, size_t offset);

        u8   retrieveStackFast(u32 frameToSkip,uPtr stack[], uSize maxStackSize);

        void fillDescriptorToFile(FILE* file, uPtr* stack,size_t maxStackSize);

        ///
        /// fill the descriptor class of the given process address
        /// @param address of the process to use
        /// @param desc is the descriptor to fill representing the address       
        ///        
        void fillDescriptor(ProcessAddress address, ProcessAddressDescriptor& desc);

        void retrieveStack(char *_buffer, ux _bufferSize, size_t _offset);

    private:
        bool m_initialized;
        CaptureStackBackTraceType CaptureStackBackTrace;
        CaptureContextType  captureContext;

        HMODULE         m_hModuleKernel;
    };
}

#endif // !ITF_FINAL && ITF_WINDOWS
//#endif // ITF_DEBUG_MEMORY

#endif // _ITF_STACKWALKER_H_

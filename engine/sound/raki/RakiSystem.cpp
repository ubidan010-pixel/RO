#include "precompiled_engine.h"



#if defined ITF_WINDOWS
// for timeGetTime
#include <timeapi.h>
#endif

#ifndef _ITF_CORE_MACROS_H_
#include "core/macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifndef _ITF_RAKISYSTEM_H_
#include "engine/sound/raki/RakiSystem.h"
#endif //_ITF_RAKISYSTEM_H_

#ifndef _ITF_THREADMANAGER_H_
#include "engine/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef RAKI_THREADBASE_H
#include "RakiEngine/System/ThreadBase/ThreadBase.h"
#endif // RAKI_THREADBASE_H

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

#ifdef ITF_NINTENDO

#include <nn/os/os_TickApi.h>

#endif // ITF_WII 



#ifdef ITF_WII 

#ifndef __MEM_MACROS_WII_H__
#include "core/memory/WII/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__

#endif // ITF_WII 



//#define LOG_RAKIALLOCATOR



namespace ITF
{
#ifdef LOG_RAKIALLOCATOR
    static u32 malloc_size = 0;
    static u32 mallocAligned_size = 0;
    static u32 mallocMem2Aligned32_size = 0;
#endif // LOG_RAKIALLOCATOR
#ifdef ITF_SUPPORT_RAKI

    void * RakiAllocator::malloc( const u64 _size, const raki::Memory::Type _type )
    {
#ifdef LOG_RAKIALLOCATOR
        malloc_size += (u32)_size;

        if ( _size > 1024 )
            RAKI_OUTPUT("RakiAllocator::malloc %d total %d ko", (unsigned int)_size, malloc_size/1024 );
#endif // LOG_RAKIALLOCATOR

        void * ptr = NULL;
        
        switch ( _type )
        {
        case raki::Memory::data:
            ptr = newAlloc( mId_AudioData, char )[ (unsigned int)_size ];
            break;

        case raki::Memory::streamBuffers:
            ptr = newAlloc( mId_AudioStreamBuffers, char )[ (unsigned int)_size ];
            break;

        default:
            ptr = newAlloc( mId_AudioEngine, char )[ (unsigned int)_size ];
        }

        return ptr;
    }


    void RakiAllocator::free( void * _ptr )
    {
#ifdef LOG_RAKIALLOCATOR
        malloc_size -= GET_HEADER_FROM_USER_PTR( _ptr )->m_size;
#endif // LOG_RAKIALLOCATOR

        delete[]( (char*)_ptr ); 
    }

    void * RakiAllocator::mallocAligned( u64 _size, u64 _alignment, const raki::Memory::Type _type )
    {
#ifdef LOG_RAKIALLOCATOR
        mallocAligned_size += (u32)_size;

        if ( _size > 1024 )
            RAKI_OUTPUT("RakiAllocator::mallocAligned %d total %d ko", (unsigned int)_size, mallocAligned_size / 1024 );
#endif // LOG_RAKIALLOCATOR

        void * ptr = NULL;

        switch ( _type )
        {
        case raki::Memory::data:
            ptr = ITF::Memory::alignedMallocCategory( (size_t)_size, (size_t)_alignment, MemoryId::mId_AudioData );
            break;

        case raki::Memory::streamBuffers:
            ptr = ITF::Memory::alignedMallocCategory( (size_t)_size, (size_t)_alignment, MemoryId::mId_AudioStreamBuffers );
            break;

        default:
            ptr = ITF::Memory::alignedMallocCategory( (size_t)_size, (size_t)_alignment, MemoryId::mId_AudioEngine );
        }

        return ptr;
    }


    void RakiAllocator::freeAligned( void * _ptr )
    {
#ifdef LOG_RAKIALLOCATOR
        mallocAligned_size -= GET_HEADER_FROM_USER_PTR( _ptr )->m_size;
#endif // LOG_RAKIALLOCATOR

        ITF::Memory::alignedFree( _ptr );
    }

    const u64 RakiAllocator::getUsedSize()
    {
        //ITF_ASSERT_MSG(0, "Cannot use RakiAllocator::getUsedSize");
        return 0;
    }


#if defined ( RAKI_PLATFORM_WII )
    
    void * RakiAllocator::mallocMem2Aligned32( u64 _size, const raki::Memory::Type _type )
    {
#ifdef LOG_RAKIALLOCATOR
        mallocMem2Aligned32_size += (u32)_size;

        if ( _size > 1024 )
            RAKI_OUTPUT("RakiAllocator::mallocMem2Aligned32 %d total %d ko", (u32)_size, mallocMem2Aligned32_size / 1024 );
#endif // LOG_RAKIALLOCATOR

        MEM_M_PushExt(MEM_C_MEM2_ALLOC);

        void * ptr = NULL;

        switch ( _type )
        {
        case raki::Memory::data:
            ptr = Memory::alignedMallocCategory( (u32)_size, 32, MemoryId::mId_AudioData );
            break;

        case raki::Memory::streamBuffers:
            ptr = Memory::alignedMallocCategory( (u32)_size, 32, MemoryId::mId_AudioStreamBuffers );
            break;

        default:
            ptr = Memory::alignedMallocCategory( (u32)_size, 32, MemoryId::mId_AudioEngine );
        }

        MEM_M_PopExt();

        return ptr;
    }

    void RakiAllocator::freeMem2Aligned32( void * _ptr )
    {
#ifdef LOG_RAKIALLOCATOR
        mallocMem2Aligned32_size -= GET_HEADER_FROM_USER_PTR( _ptr )->m_size;
#endif // LOG_RAKIALLOCATOR

        Memory::alignedFree( _ptr );
    }

#endif // ( RAKI_PLATFORM_WII )




#if defined ( RAKI_PLATFORM_XB360 )

    void * RakiAllocator::mallocPhysicalAligned( u64 _size, const raki::Memory::Type _type )
    {
        void * p = slotAllocatorManager::allocatePhysicalMemory( (u32)_size, 2048,  PAGE_READWRITE );

        return p;
    }

    void RakiAllocator::freePhysicalAligned( void * _ptr )
    {
        slotAllocatorManager::deallocatePhysicalMemory( _ptr );
    }

#endif // ( RAKI_PLATFORM_XB360 )




    void RakiAllocator::memcpy( void * _dest, const void * _src, u64 _size )
    {
        ITF_MEMCOPY( _dest, _src, (size_t) _size );
    }


    void RakiAllocator::memZero( void * _dest, u64 _size )
    {
        ITF_MemSet( _dest, 0, (size_t) _size );
    }

#endif


}

namespace raki
{

    // Debug

    void Debug::output( const char * _text, ...)
    {
#if defined ( ITF_WINDOWS ) || defined ( ITF_X360 )

        char formattedText[0x200];
        va_list args ;
        va_start( args, _text );

        vsprintf_s( formattedText, _text, args );
        va_end (args) ;

        // show time 
        char formattedTextWithTime[0x200];
        sprintf_s( formattedTextWithTime, 0x200, "(%.3f) (thread 0x%x) %s\n", Time::getTime(), GetCurrentThreadId(), formattedText);

        OutputDebugStringA( formattedTextWithTime );

#else

        char formattedText[0x200];
        va_list args ;
        va_start( args, _text );

        vsprintf( formattedText, _text, args );
        va_end (args) ;

        // show time 
        char formattedTextWithTime[0x200];
        sprintf( formattedTextWithTime, "(%.3f) %s", Time::getTime(), formattedText);

#ifdef ITF_WII
        OSReport(formattedTextWithTime);
        OSReport("\n");
#else // ITF_WII
        LOG( formattedTextWithTime );
#endif // ITF_WII
#endif
    }

    void Debug::debugBreak()
    {
        //RAKI_OUTPUT( "raki::Debug::debugBreak" );
        ITF_ASSERT(0);
    }


    // Time 

    f32 Time::getTime()
    {
#if defined(ITF_WINDOWS)
        static DWORD timeBeginning = timeGetTime();

        DWORD duration = timeGetTime() - timeBeginning;

        return ( (f32) duration ) / 1000.f;
#elif defined(ITF_X360) || defined(ITF_XBOX_SERIES)
        static DWORD timeBeginning = GetTickCount();

        DWORD duration = GetTickCount() - timeBeginning;

        return ( (f32) duration ) / 1000.f;
#elif defined ITF_PS3
        static system_time_t timeBeginning = sys_time_get_system_time();

        system_time_t duration = sys_time_get_system_time() - timeBeginning;

        return ( (f32) duration ) / 1000000.f;
#elif defined ITF_PS5
        static uint64_t timeBeginning = sceKernelGetProcessTime();

        uint64_t duration = sceKernelGetProcessTime() - timeBeginning;

        return ( (f32) duration ) / 1000000.f;
#elif defined(ITF_NINTENDO)
        static nn::os::Tick timeBeginning = nn::os::GetSystemTick();

        u64 microseconds = ( nn::os::GetSystemTick() - timeBeginning ).ToTimeSpan().GetMicroSeconds();

        return ( (f32) microseconds ) / 1000000.f;
#else
        #error Unknown platform
#endif
    }



    // Thread

#ifdef ITF_SUPPORT_RAKI
    ITF::u32 WINAPI RakiThread_Function( void * _data )
    {
        RAKI_ASSERT( _data );

        ThreadBase * object = (ThreadBase*) _data;

        Thread::theThread( object );

        return 0;
    }


    void Thread::theThread( ThreadBase * _object )
    {
        //RAKI_OUTPUT( "Thread::theThread launching thread on Thread 0x%x\n", _object );

        _object->internalRun();
    }


    bool Thread::beginThread( ThreadBase * _object )
    {
        ITF::Thread * thread = ITF::THREADMANAGER->createThread( RakiThread_Function, _object, ITF::ThreadSettings::m_settings[ITF::eThreadId_soundStreamer]);

        return ( thread != NULL );
    }


    void Thread::sleep( u32 _milliseconds )
    {
        ITF::sleep( _milliseconds );
    }
#endif




} // namespace raki

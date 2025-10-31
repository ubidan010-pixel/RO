
#ifndef _ITF_RAKISYSTEM_H_
#define _ITF_RAKISYSTEM_H_




#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_


#ifndef RAKI_ALLOCATOR_H
#include "RakiEngine/System/Memory/Allocator.h"
#endif // RAKI_ALLOCATOR_H




namespace ITF
{
    


    class RakiAllocator 
        : public TemplateSingleton<RakiAllocator>
        , public raki::Allocator
    {
    public:

        virtual void * malloc( const u64 _size, const raki::Memory::Type _type );
        virtual void free( void * _ptr );

        virtual void * mallocAligned( u64 _size, u64 _alignment, const raki::Memory::Type _type );
        virtual void freeAligned( void * _ptr );

#if defined ( RAKI_PLATFORM_WII )

        virtual void * mallocMem2Aligned32( u64 _size, const raki::Memory::Type _type );
        virtual void freeMem2Aligned32( void * _ptr );

#elif defined ( RAKI_PLATFORM_XB360 )

        virtual void * mallocPhysicalAligned( const u64 _size, const raki::Memory::Type _type );
        virtual void freePhysicalAligned( void * _ptr );

#endif 

        virtual const u64 getUsedSize();

        virtual void memcpy( void * _dest, const void * _src, u64 _size );
        virtual void memZero( void * _dest, u64 _size );
    };




}
#endif // _ITF_RAKISYSTEM_H_


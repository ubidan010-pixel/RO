#ifndef _ITF_CELL_H_
#define _ITF_CELL_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

//#define ENABLE_CELL_FLAGS_FAST_DISCARD

namespace ITF
{
    class   World;

    class Cell
    {
    public:
        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            static const u32   cellObjectFlag_None = 0;
            static const u32   cellObjectFlag_FirstLine = 1;
            static const u32   cellObjectFlag_FirstColumn = 2;
            static const u32   cellObjectFlag_Corner = cellObjectFlag_FirstLine | cellObjectFlag_FirstColumn;
        #endif // ENABLE_CELL_FLAGS_FAST_DISCARD

        Cell(): m_FrameEnable(U32_INVALID), m_ignoreLoadResourceMask(0)
        {
        };
        ~Cell();

        void init(const Vec2d& _pos, const Vec2d& _size,const i16 _indexX,const i16 _indexY, World* _world);

        ITF_INLINE const Vec2d&    getPos()    const               {return m_aabb.getMin();}
        ITF_INLINE const AABB&     getAABB()   const               {return m_aabb;}
        ITF_INLINE const u32       getLoadResourcesMask()   const  {return m_loadResourcesMask;}

        void            enable(u32 _currentFrame);
        void            disable();
        void            signalIgnoreLoadResourceMask() {m_ignoreLoadResourceMask = 1;}
        void            clearIgnoreLoadResourceMask() {m_ignoreLoadResourceMask = 0;}
        bbool           getIgnoreLoadResourceMask() const {return m_ignoreLoadResourceMask;}

        ITF_INLINE bbool           isEnable() const {return (1 == m_bEnable);}
              
        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            void            addObject(ObjectRef _ObjRef, u32 _iLayer, u32 _flags, bbool _checkDoublons = btrue);
        #else
            void            addObject(ObjectRef _ObjRef, u32 _iLayer, bbool _checkDoublons = btrue);
        #endif
        bbool           removeObject(ObjectRef _ObjRef);
        bbool           hasObject(ObjectRef _ObjRef) const;
        bbool           hasObjects() const;
        bbool           checkForPhysicalReady();
        bbool           checkObjectBelongsToCell (class Pickable* _pObj) const;

        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            struct CellObjData
            {
                ObjectRef   m_obj;
                u32         m_flags;
            };
            void            getObjects(SafeArray<CellObjData>& _list, u32 _flags) const;
            void            getObjects(SafeArray<ObjectRef>& _list, u32 _flags) const;
            const           SafeArray<CellObjData>& getAllObjects() const         {return m_vPickable;}
        #else
            void            getObjects(ObjectRefList& _list) const;
            const           SafeArray<ObjectRef>& getAllObjects() const         {return m_vPickable;}
        #endif

    private:


        static  Cell*                   m_beingUpdatedCell;
        static SafeArray<BaseObject*>   m_solvedPointers;

        u32     m_FrameEnable;

        AABB    m_aabb;

        u32     m_bEnable : 1,
                m_loadResourcesMask : 3,
                m_ignoreLoadResourceMask : 1;

        World*  m_world;

 
#ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
        SafeArray<CellObjData> m_vPickable;
#else
        SafeArray<ObjectRef> m_vPickable;
#endif // ENABLE_CELL_FLAGS_FAST_DISCARD



    };

}

#endif // _ITF_CELL_H_

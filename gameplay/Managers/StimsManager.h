#ifndef _ITF_STIMSMANAGER_H_
#define _ITF_STIMSMANAGER_H_

#ifndef _ITF_GAMEPLAYSTIMS_H_
#include "gameplay/GameplayStims.h"
#endif //_ITF_GAMEPLAYSTIMS_H_

namespace ITF
{

class StimsManager
{

public:

    StimsManager();
    ~StimsManager();


    void                update( f32 _deltaTime );
    void                sendStim( EventStim* _stim );
    ITF_INLINE void     sendStimDirectly( EventStim* _stim, SafeArray<ObjectRef>& _processed )
    {
        processStim(_stim, _processed);
        recycleStim(_stim);
    }

    template <class T>
    T*                  requestStim( const PhysShape* _shape );
    void                recycleStim( EventStim* _stim );

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void                toggleDrawStims();
    ITF_INLINE u32      getDebugMode() const { return m_drawMode; }
#endif // ITF_SUPPORT_DEBUGFEATURE

private:

    typedef ITF_VECTOR <EventStim*> StimsArray;
    typedef ITF_QUEUE <EventStim*> StimsQueue;
    typedef ITF_MAP <StringID::StringIdValueType,StimsQueue> StimsRecyclingMap;

    typedef ITF_LIST <PhysShape*> ShapesList;
    typedef ITF_VECTOR <PhysShape*> ShapesArray;

    bbool               prepareStim( EventStim* _stim, const PhysShape* _shape );
    void                processStim( EventStim* _stim, SafeArray<ObjectRef>& _processed );
    void                createPools();
    void                destroyPools();
    void                destroyPool( ShapesList* _list );
    void                restorePools();
#ifndef ITF_FINAL
    void                dumpStimsDebugInfoAndCrash();
#endif // ITF_FINAL
    ShapesList*         getShapePoolForShape( const PhysShape* _shape );
    
    StimsArray          m_pendingStims;
    StimsRecyclingMap   m_recyclingStims;

    ShapesList          m_circlePool;
    ShapesList          m_polygonPool;
    ShapesList          m_boxPool;
    ShapesArray         m_usedShapes;

    static const u32 s_PoolSize = 100;

#ifdef ITF_SUPPORT_DEBUGFEATURE
    u32                 m_drawMode; // 0: don't draw, 1: draw for 1 second, 2: draw for 1 frame
#endif // ITF_SUPPORT_DEBUGFEATURE
};

template <class T>
T* StimsManager::requestStim( const PhysShape* _shape )
{
    StimsRecyclingMap::iterator it = m_recyclingStims.find(T::GetClassCRCStatic());
    EventStim* stim;    // with this we make sure that we are creating a eventstim (in compilation time)

    if ( it != m_recyclingStims.end() && !it->second.empty() )
    {
        stim = it->second.front();
        ((T*)stim)->~T();
        new(stim) T();
        if ( prepareStim(stim,_shape) )
        {
            it->second.pop();
        }
        else
        {
            stim = NULL;
        }
    }
    else
    {
        stim = newAlloc(mId_Gameplay,T);

        if ( !prepareStim(stim,_shape) )
        {
            SF_DEL(stim);
        }
    }

#ifndef ITF_FINAL
    if ( !stim )
    {
        dumpStimsDebugInfoAndCrash();
    }
#endif

    if ( stim )
    {
        stim->reset();
    }

    return static_cast<T*>(stim);
}

}

#endif // _ITF_STIMSMANAGER_H_


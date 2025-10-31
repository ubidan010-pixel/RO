#include "precompiled_engine.h"

#ifndef _ITF_SCENE_TYPES_H_  
#include "engine\scene\scene_types.h"
#endif //_ITF_SCENE_TYPES_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
    BEGIN_SERIALIZATION(TargetFilterList)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            SERIALIZE_MEMBER("platform", m_platform);
            SERIALIZE_CONTAINER("objects", m_objList);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()
    
    BEGIN_SERIALIZATION(FriezeConnectionResult)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            SERIALIZE_MEMBER("pos", m_pos);
            SERIALIZE_MEMBER("prevOwner", m_prevOwner);
            SERIALIZE_MEMBER("prevColId", m_prevColId);
            SERIALIZE_MEMBER("nextOwner", m_nextOwner);
            SERIALIZE_MEMBER("nextColId", m_nextColId);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    void FriezeConnectionResult::applyResult() const
    {
        if(m_prev && m_next)
        {
            m_next->m_connection.setPrevious(m_prev, m_pos);
        }
        else
        {
            ITF_ASSERT_MSG(0, "Runtime info missing, did you called generateRuntimeDataFromSerialization() ?");
        }
    }

    void FriezeConnectionResult::generateSerializationDataFromRuntime()
    {
        if(const Frise* prevOwner = m_prev->m_ownerFrise)
        {
            SceneObjectPathUtils::getAbsolutePathFromObject(prevOwner, m_prevOwner);
            const SafeArray<PolyLine*>* pCollisionData = prevOwner->getCollisionData_WorldSpace();

            if ( pCollisionData )
                m_prevColId = pCollisionData->find(m_prev);
            else
                m_prevColId = U32_INVALID;                

            ITF_ASSERT(m_prevColId != U32_INVALID);
        }
        else
        {
            ITF_ASSERT(0);
        }
        
        if(const Frise* nextOwner = m_next->m_ownerFrise)
        {
            SceneObjectPathUtils::getAbsolutePathFromObject(nextOwner, m_nextOwner);
            const SafeArray<PolyLine*>* pCollisionData = nextOwner->getCollisionData_WorldSpace();

            if ( pCollisionData )
                m_nextColId = pCollisionData->find(m_next);
            else
                m_nextColId = U32_INVALID;

            ITF_ASSERT(m_nextColId != U32_INVALID);
        }
        else
        {
            ITF_ASSERT(0);
        }
    }

    void FriezeConnectionResult::generateRuntimeDataFromSerialization()
    {
        ITF_ASSERT(m_prevOwner.isValid());
        if(const Frise* prevOwner = static_cast<const Frise*>(SceneObjectPathUtils::getObjectFromAbsolutePath(m_prevOwner)))
        {
            const SafeArray<PolyLine*>* pCollisionData = prevOwner->getCollisionData_WorldSpace();
            if ( pCollisionData )
                m_prev = (*pCollisionData)[m_prevColId];
            else
            {
                ITF_ASSERT(0);
            }
        }
        else
        {
            ITF_ASSERT(0);
        }

        ITF_ASSERT(m_nextOwner.isValid());
        if(const Frise* nextOwner = static_cast<const Frise*>(SceneObjectPathUtils::getObjectFromAbsolutePath(m_nextOwner)))
        {
            const SafeArray<PolyLine*>* pCollisionData = nextOwner->getCollisionData_WorldSpace();
            if ( pCollisionData )
                m_next = (*pCollisionData)[m_nextColId];
            else
            {
                ITF_ASSERT(0);
             }
        }
        else
        {
            ITF_ASSERT(0);
        }
    }

} // namespace ITF

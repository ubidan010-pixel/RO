
#include "precompiled_gameplay.h"

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    AIManager* AIManager::s_instance = NULL;

AIManager::AIManager()
{
}

AIManager::~AIManager()
{
    ITF_ASSERT_MSG(m_depthRanges.size() == 0, "Actors were not all removed, so it will leak");
}

AIData * AIManager::addActor( ActorRef _ref, const DepthRange& _depthRange,AIData * _data)
{
    DepthRangeData* depthRangeData = NULL;

    for ( u32 i = 0; i < m_depthRanges.size(); i++ )
    {
        if ( m_depthRanges[i].m_depthRange == _depthRange )
        {
            depthRangeData = &m_depthRanges[i];
            break;
        }
    }

    if ( !depthRangeData )
    {
        m_depthRanges.push_back(DepthRangeData());
        depthRangeData = &m_depthRanges.back();
        depthRangeData->m_depthRange = _depthRange;
    }

    if (!_data)
    {
        AIDataMap::const_iterator it = depthRangeData->m_actors.find(_ref);
        if (it != depthRangeData->m_actors.end())
        {
            // this actor was already registered, it can happen if we have 2 components registering
            // NOTE: in theory we should check all depthRanges, in case the actor was registered in a different one, but that would indicate a major logical screwup.
            _data = it->second;
        }
        else
        {
            _data = allocateAiData();
        }
        ITF_ASSERT(_data);
    }
    depthRangeData->m_actors[_ref] = _data;
    return _data;
}

AIData * AIManager::addActor( ActorRef _ref, const DepthRange& _depthRange )
{
    return addActor(_ref,_depthRange,NULL);
}
void AIManager::removeActor( ActorRef _ref,  const DepthRange& _depthRange)
{
    DepthRangeArray::iterator itArray = m_depthRanges.begin();
    while(itArray != m_depthRanges.end())
    {
        DepthRangeData& depthRangeData = *itArray;
        if ( depthRangeData.m_depthRange == _depthRange )
        {
            AIDataMap::iterator it = depthRangeData.m_actors.find(_ref);
            if (it != depthRangeData.m_actors.end())
            {
                AIData * data = it->second;
                delete data;
                depthRangeData.m_actors.erase(it);

                if(depthRangeData.m_actors.size() == 0)
                {
                    itArray = m_depthRanges.erase(itArray);
                    continue;
                }
            }
            else
            {
                //NB : we no longer display a warning in case we don't find, because multiple components of same actor can register/unregister themselves in AIMANAGER
            }
        }
        ++itArray;
    }
}

void AIManager::getActorsFromLayer( const DepthRange& _depthRange, ActorList& _actors ) const
{
    for ( u32 i = 0; i < m_depthRanges.size(); i++ )
    {
        const DepthRangeData& depthRangeData = m_depthRanges[i];

        if ( depthRangeData.m_depthRange == _depthRange )
        {
            for ( AIDataMap::const_iterator it = depthRangeData.m_actors.begin(); it != depthRangeData.m_actors.end(); ++it )
            {
                if ( Actor* actor = it->first.getActor() )
                {
                    _actors.push_back(actor);
                }
            }

            break;
        }
    }
}

void   AIManager::getActorsFromLayerWithAIComponent( const DepthRange& _depthRange, const AABB &_searchZone, ActorList* _actors, StringID::StringIdValueType _classCRC, SafeArray<AIBehavior*> *_behaviorList, StringID::StringIdValueType _behaviorClassCRC ) const
{
    bbool isAABBValid = _searchZone.isValid();
    bbool scopedSearch = _depthRange.isValid();

    for ( u32 i = 0; i < m_depthRanges.size(); i++ )
    {
        const DepthRangeData& depthRangeData = m_depthRanges[i];

        if ( !scopedSearch || depthRangeData.m_depthRange == _depthRange)
        {
            for ( AIDataMap::const_iterator it = depthRangeData.m_actors.begin(); it != depthRangeData.m_actors.end(); ++it )
            {
                BaseObject* obj = GETOBJECT(it->first);

                if ( obj )
                {
                    Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                    if ( !actor )
                        continue;
                    const AABB& actorAABB = actor->getAABB();
                    if ( (!isAABBValid || !actorAABB.isValid() || _searchZone.checkOverlap(actorAABB)) )
                    {
                        AIComponent *component = (AIComponent*)actor->GetComponent<AIComponent>();
                        if (component && component->IsClassCRC(_classCRC))
                        {
                            //If we filter by behavior
                            if (_behaviorClassCRC != StringID::InvalidId)
                            {
                                const SafeArray<AIBehavior*> &behaviors = component->getBehaviors();
                                bbool actorWasAdded = bfalse;
                                for (u32 i=0; i<behaviors.size(); i++)
                                {
                                    AIBehavior *behavior = behaviors[i];
                                    if (behavior && behavior->IsClassCRC(_behaviorClassCRC) )
                                    {
                                        if (_actors && !actorWasAdded)
                                        {
                                            _actors->push_back(actor);
                                            actorWasAdded = btrue;
                                        }
                                        if (_behaviorList)
                                        {
                                            _behaviorList->push_back(behavior);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                //if we dont filter by behavior
                                ITF_ASSERT_CRASH(_actors!=NULL, "actor result list is invalid");
                                _actors->push_back(actor);
                            }
                        }
                    }
                }
            }

            break;
        }
    }
}

AIData* AIManager::getAIData( ActorRef _ref, const DepthRange& _depthRange )
{
    const u32 size = m_depthRanges.size();
    for ( u32 i = 0; i < size; i++ )
    {
        const DepthRangeData& depthRangeData = m_depthRanges[i];

        if ( depthRangeData.m_depthRange == _depthRange )
        {
            AIDataMap::const_iterator it = depthRangeData.m_actors.find(_ref);
            if (it != depthRangeData.m_actors.end())
            {
                return it->second;
            }
        }
    }
    return NULL;
}

void AIManager::changeDepth( ActorRef _ref, const DepthRange & _oldDepthRange, const DepthRange & _newDepthRange )
{
    if (_oldDepthRange == _newDepthRange)
        return;

    AIData* data = NULL;
    DepthRangeArray::iterator itArray = m_depthRanges.begin();
    while(itArray != m_depthRanges.end())
    {
        DepthRangeData& depthRangeData = *itArray;

        if ( depthRangeData.m_depthRange == _oldDepthRange )
        {
            AIDataMap::iterator it = depthRangeData.m_actors.find(_ref);
            if (it != depthRangeData.m_actors.end())
            {
                data = it->second;
                depthRangeData.m_actors.erase(it);

                if(depthRangeData.m_actors.size() == 0)
                    itArray = m_depthRanges.erase(itArray);
                break;
            }
            else
            {
                Actor * actor = ActorRef(_ref).getActor();
                ITF_WARNING(NULL,0,"AIMANAGER::changeDepth failed to find actor %s in old depthrange", actor?actor->getUserFriendly().cStr():"");
            }
        }

        ++itArray;
    }
    addActor(_ref,_newDepthRange,data);

}

}
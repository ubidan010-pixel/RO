#ifndef _ITF_AIMANAGER_H_
#define _ITF_AIMANAGER_H_

#ifndef _ITF_DEPTH_RANGE_H_
#include "engine/scene/DepthRange.h"
#endif //_ITF_DEPTH_RANGE_H_

namespace ITF
{
    #define AI_MANAGER AIManager::get()
    
    class AIBehavior;

    struct AIData
    {
        AIData() : m_faction(0){}
        virtual ~AIData() {}
        Faction m_faction;
    };
    class AIManager
    {
    protected:
        static AIManager*    s_instance;
    public:

        static void                 create()    { s_instance = newAlloc(mId_Singleton, AIManager()); }
        static void                 destroy()   { SF_DEL(s_instance); }
        static AIManager*           get()       { return s_instance; }  
    public:

        AIManager();
        virtual ~AIManager();

        virtual AIData *                allocateAiData(){ return newAlloc(mId_Gameplay,AIData()); }

        AIData *                        addActor( ActorRef _ref, const DepthRange& _depthRange );
        void                            removeActor( ActorRef _ref, const DepthRange & _depthRange );
        void                            changeDepth( ActorRef _ref, const DepthRange & _oldDepthRange, const DepthRange & _newDepthRange);
        void                            getActorsFromLayer( const DepthRange& _depthRange, ActorList& _actors ) const;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// search actors which were registered in given depth range, provided they have a given component (that must be a AI component)
        /// @param _depthRange : The depthRange to search in. If InvalidDepthRange search in all.
        /// @param _searchZone : if this AABB is valid, only return object whose AABB overlaps. NB : If object has invalid AABB, return it too.
        /// @param _actors : the result
        template <typename AIComponentSubclass>
        void getActorsFromLayerWithAIComponent(const DepthRange& _depthRange, const AABB &_searchZone, ActorList& _actors) const
        {
            getActorsFromLayerWithAIComponent(_depthRange, _searchZone, &_actors, ITF_GET_STRINGID_CRC(AIComponentSubclass,2716328018), NULL, StringID::InvalidId);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// search actors which were registered in given depth range, provided they have a given component (that must be a AI component) and behavior
        /// @param _depthRange : The depthRange to search in. If InvalidDepthRange search in all.
        /// @param _searchZone : if this AABB is valid, only return object whose AABB overlaps. NB : If object has invalid AABB, return it too.
        /// @param _actors : the resulting actor List. May be NULL.
        /// @param _behaviorList : the resulting behavior list. May be NULL.
        template<class AIComponentSubclass, class AIBehaviorClass> void  getActorsFromLayerWithAIComponentAndBehavior(const DepthRange& _depthRange, const AABB &_searchZone, ActorList* _actors, SafeArray<AIBehavior*> *_behaviorList) const { getActorsFromLayerWithAIComponent(_depthRange, _searchZone, _actors, ITF_GET_STRINGID_CRC(AIComponentSubclass,2716328018), _behaviorList, ITF_GET_STRINGID_CRC(AIBehaviorClass,219134792));}

        AIData* getAIData( ActorRef _ref,const DepthRange & _depthRange );

        typedef ITF_MAP<ActorRef, AIData*> AIDataMap;

        class LayerIterator
        {
        public:

            LayerIterator( const DepthRange& _depthRange );

            ITF_INLINE Actor* getNext();

        private:

            AIDataMap::const_iterator m_it;
            AIDataMap::const_iterator m_end;
        };
    private:
        AIData* addActor( ActorRef _ref, const DepthRange& _depthRange, AIData* _data );
        void getActorsFromLayerWithAIComponent(const DepthRange& _depthRange, const AABB &_searchZone, ActorList* _actors, StringID::StringIdValueType _classCRC, SafeArray<AIBehavior*> *_behaviorList, StringID::StringIdValueType _behaviorClassCRC ) const;

        struct DepthRangeData
        {
            DepthRange  m_depthRange;
            AIDataMap   m_actors;
        };

        typedef ITF_VECTOR <DepthRangeData> DepthRangeArray;

        DepthRangeArray                      m_depthRanges;
    };


    ITF_INLINE AIManager::LayerIterator::LayerIterator( const DepthRange& _depthRange )
    {
        for ( u32 i = 0; i < s_instance->m_depthRanges.size(); i++ )
        {
            const DepthRangeData& depthRangeData = s_instance->m_depthRanges[i];

            if ( depthRangeData.m_depthRange == _depthRange )
            {
                m_it = depthRangeData.m_actors.begin();
                m_end = depthRangeData.m_actors.end();
                return;
            }
        }

        // if we got here, there are no actors in this range
        m_it = m_end;
    }

    ITF_INLINE Actor* AIManager::LayerIterator::getNext()
    {
        for ( ; m_it != m_end; ++m_it )
        {
            if ( Actor* actor = m_it->first.getActor() )
            {
                ++m_it;
                return actor;
            }
        }

        return NULL;
    }

};

#endif //_ITF_AIMANAGER_H_

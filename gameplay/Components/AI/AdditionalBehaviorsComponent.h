#ifndef _ITF_ADDITIONALBEHAVIORSCOMPONENT_H_
#define _ITF_ADDITIONALBEHAVIORSCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    //-----------------------------------------------------------------
    class ExternBehaviorData_Template
    {
    public:
        DECLARE_SERIALIZE();
        ExternBehaviorData_Template() : m_behaviorTemplate(NULL), m_disablePhys( bfalse )   {}
        ~ExternBehaviorData_Template();

        const StringID &     getBehaviorName() const;
        const StringID &     getNextBehavior( const StringID & _prevBhv, bbool _getDefaultNext ) const;

        struct NextBhvData
        {
            DECLARE_SERIALIZE();
            StringID    m_previousBvh;
            StringID    m_nextBvh;
        };
                
        class TemplateAIBehavior *  m_behaviorTemplate;
        StringID                    m_behaviorName;
        ITF_VECTOR<StringID>        m_cuttableBehaviors;
        ITF_VECTOR<NextBhvData>     m_nextBehaviors;
        bbool                       m_disablePhys;
        StringID                    m_defaultNextBehavior;
    };

    //-----------------------------------------------------------------
    class AdditionalBehaviorsComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AdditionalBehaviorsComponent, ActorComponent,239653273);
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE()
    public:

        AdditionalBehaviorsComponent();
        virtual ~AdditionalBehaviorsComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onEvent( Event * _event );
        virtual void        onCheckpointLoaded();
    
    private:
        class ExternBehaviorData
        {
        public:
            ExternBehaviorData() : m_behavior(NULL), m_template(NULL), m_prevPhysState( bfalse ) {}
            AIBehavior *                        m_behavior;
            const ExternBehaviorData_Template * m_template;
            bbool                               m_prevPhysState;
        };

        void                    processChangeExternBhvValidationQuery( EventChangeExternBhvValidationQuery * _query );

        bbool                   onBehaviorFinished( const StringID &  _bhvName, const StringID & _newBhvName, bbool _stopped );

        void                    changeBehaviorQuery( const StringID &  _bhvName, bbool _forceRestart = btrue );
        ExternBehaviorData *    getBehaviorData( const StringID &  _bhvName );
        bbool                   isBehaviorCuttableBy( const StringID & _bhvName, const StringID & _newBhvName );
        bbool                   isBehaviorCuttableBy( ExternBehaviorData * _curBhvData, const StringID & _newBhvName );
        void                    restorePhysState( ExternBehaviorData * _bhvData );

        bbool                   isCurrentBehavior( const StringID &  _bhvName );
        ITF_INLINE const class AdditionalBehaviorsComponent_Template *  getTemplate() const;

    private:
        SafeArray<ExternBehaviorData>   m_externBehaviorDataList;
        class AIComponent  *            m_aiComponent;
        class PhysComponent *           m_physComponent;
        bbool                           m_skipBvhValidation;

    };

    //---------------------------------------------------------------------
    class AdditionalBehaviorsComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AdditionalBehaviorsComponent_Template, TemplateActorComponent,1452832865);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(AdditionalBehaviorsComponent);
        
    public:

        AdditionalBehaviorsComponent_Template();
        ~AdditionalBehaviorsComponent_Template(){}

        ITF_INLINE const ITF_VECTOR<ExternBehaviorData_Template> & getExternBehaviorDataList() const       { return m_externBehaviorDataList; }
        ITF_INLINE const StringID &                                getStartBhv() const                     { return m_startBhv; }
        ITF_INLINE const StringID &                                getOnTriggerActiveBhv() const           { return m_onTriggerActiveBhv; }
        ITF_INLINE const StringID &                                getOnTriggerDesactivateBhv() const      { return m_onTriggerDesactivateBhv; }
        ITF_INLINE const bbool                                     getDisablePhys() const                  { return m_disablePhys; }
        ITF_INLINE const bbool                                     getOnTriggerActiveCheckNextBhv() const  { return m_onTriggerActiveCheckNextBhv; }
        
    private:
        ITF_VECTOR<ExternBehaviorData_Template>  m_externBehaviorDataList;
        StringID                                 m_startBhv;
        StringID                                 m_onTriggerActiveBhv;
        StringID                                 m_onTriggerDesactivateBhv;
        bbool                                    m_disablePhys;
        bbool                                    m_onTriggerActiveCheckNextBhv;
        
    };

    ITF_INLINE const class AdditionalBehaviorsComponent_Template * AdditionalBehaviorsComponent::getTemplate() const
    {
        return static_cast<const class AdditionalBehaviorsComponent_Template *>(m_template);
    }

}


#endif // _ITF_ADDITIONALBEHAVIORSCOMPONENT_H_
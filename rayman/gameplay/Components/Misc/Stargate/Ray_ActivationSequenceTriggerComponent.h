#ifndef _ITF_RAY_ACTIVATIONSEQUENCETRIGGERCOMPONENT_H_
#define _ITF_RAY_ACTIVATIONSEQUENCETRIGGERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{

    class Ray_ActivationSequenceTriggerComponent : public ActorComponent, IUpdateAnimInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ActivationSequenceTriggerComponent, ActorComponent,2552097327)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_ActivationSequenceTriggerComponent();
        virtual ~Ray_ActivationSequenceTriggerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event * _event);
        virtual void        updateAnimInput();

    private:

        struct SlotInfo
        {
            ActorRef        m_visual;
            ActorRef        m_stone;
            ActorRef        m_user;
            ActorRef        m_reserved;
            bbool           m_valid;
            bbool           m_isDisabled;
        };

        typedef SafeArray <SlotInfo> SlotInfoList;

        ITF_INLINE const class Ray_ActivationSequenceTriggerComponent_Template * getTemplate() const;

        void                        processQueryEmpty( Ray_EventQueryEmptyStone* _queryEmpty ) const;
        void                        processAnimEvent( AnimGameplayEvent* _animEvent );

        void                        updateSlotsList();
        void                        updateSlotsActivation( f32 _dt );
        void                        processActivation();
        u32                         findSlotInfoFromVisual( ActorRef _visual ) const;
        u32                         findSlotInfoFromReserved( ActorRef _visual ) const;
        u32                         findSlotInfoFree() const;

        void                        checkCharacterActivate();

        void                        spawnFx();
        void                        updateFx();

        class DetectorComponent*    m_detectorComponent;
        class AnimatedComponent*    m_animComponent;

        ActorRef                    m_fxRef;
        SpawneeGenerator            m_fxSpawn;
        u32                         m_fxBoneIndex;
        SlotInfoList                m_slotsInfo;
        bbool                       m_activated;
        bbool                       m_activationFinished;
        bbool                       m_done;
    };

    //-------------------------------------------------------------------------------------
    class Ray_ActivationSequenceTriggerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ActivationSequenceTriggerComponent_Template,TemplateActorComponent,4164320951);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ActivationSequenceTriggerComponent);

    public:

        Ray_ActivationSequenceTriggerComponent_Template();
        ~Ray_ActivationSequenceTriggerComponent_Template() {}

        const Path&                             getFX() const { return m_fx; }
        StringID                                getFXBone() const { return m_fxBone;}
        bbool                                   getCanActivate() const { return m_canActivate; }
        bbool                                   getIsNPC() const { return m_npc; }

    private:

        Path                                    m_fx;
        StringID                                m_fxBone;
        bbool                                   m_canActivate;
        bbool                                   m_npc;
    };

    ITF_INLINE const class Ray_ActivationSequenceTriggerComponent_Template * Ray_ActivationSequenceTriggerComponent::getTemplate() const
    {
        return static_cast<const class Ray_ActivationSequenceTriggerComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_ACTIVATIONSEQUENCETRIGGERCOMPONENT_H_


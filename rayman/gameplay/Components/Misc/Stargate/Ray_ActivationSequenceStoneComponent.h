#ifndef _ITF_RAY_ACTIVATIONSEQUENCESTONECOMPONENT_H_
#define _ITF_RAY_ACTIVATIONSEQUENCESTONECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{

    class Ray_ActivationSequenceStoneComponent : public ActorComponent, IUpdateAnimInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ActivationSequenceStoneComponent, ActorComponent,2008432726)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_ActivationSequenceStoneComponent();
        virtual ~Ray_ActivationSequenceStoneComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _dt );
        virtual void                onEvent( Event * _event);
        virtual void                updateAnimInput();

        void                        setActivationFinished() { m_activationFinished = btrue; }

    private:
        ITF_INLINE const class Ray_ActivationSequenceStoneComponent_Template * getTemplate() const;

        void                        processQueryEmpty( Ray_EventQueryEmptyStone* _queryEmpty );

        void                        spawnFx();
        void                        updateFx();

        class LinkComponent*        m_linkComponent;
        class AnimatedComponent*    m_animComponent;
        ActorRef                    m_fxRef;
        SpawneeGenerator            m_fxSpawn;
        u32                         m_fxBoneIndex;
        ActorRef                    m_user;
        ActorRef                    m_visual;
        f32                         m_activationCounter;
        bbool                       m_activationFinished;
    };

    //-------------------------------------------------------------------------------------
    class Ray_ActivationSequenceStoneComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ActivationSequenceStoneComponent_Template,TemplateActorComponent,1879566921);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ActivationSequenceStoneComponent);

    public:

        Ray_ActivationSequenceStoneComponent_Template();
        ~Ray_ActivationSequenceStoneComponent_Template() {}

        const Path&                     getFX() const { return m_fx; }
        StringID                        getFXBone() const { return m_fxBone;}
        f32                             getActivationTime() const { return m_activationTime; }

    private:

        Path                            m_fx;
        StringID                        m_fxBone;
        f32                             m_activationTime;
    };

    ITF_INLINE const class Ray_ActivationSequenceStoneComponent_Template * Ray_ActivationSequenceStoneComponent::getTemplate() const
    {
        return static_cast<const class Ray_ActivationSequenceStoneComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_ACTIVATIONSEQUENCESTONECOMPONENT_H_


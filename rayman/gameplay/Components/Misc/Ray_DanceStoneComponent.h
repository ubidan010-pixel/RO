#ifndef _ITF_RAY_DANCESTONECOMPONENT_H_
#define _ITF_RAY_DANCESTONECOMPONENT_H_

#ifndef _ITF_INPUTLISTENER_H_
#include "engine/zinput/ZInputListener.h"
#endif //_ITF_INPUTLISTENER_H_

namespace ITF
{

    class LinkComponent;
    class Ray_DanceDoorComponent;
    class AnimLightComponent;
    class FXControllerComponent;

    class Ray_DanceStoneComponent : public ActorComponent, public IInputListener 
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DanceStoneComponent, ActorComponent,1997048734)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_DanceStoneComponent();
        virtual ~Ray_DanceStoneComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual void        onCheckpointLoaded();

        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();

        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event* _event );

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif

        virtual void        Receive( u32 player, f32 axis, const StringID& action ) ;

    private:
        ITF_INLINE const class Ray_DanceStoneComponent_Template * getTemplate() const;

        Actor*              findActorByName( const String8& _name );

        void                processTrigger( EventTrigger* trigger );
        void                processCanDance( Ray_EventCanDanceOnStone* _canDance );
        void                processQueryEmpty( Ray_EventQueryEmptyStone* _queryEmpty );
        void                validatePlayer( f32 axis , const StringID& action );
        void                updateFeedback();
        void                spawnReward(Actor* _sender, ObjectRef _receiver, Vec3d _offset = Vec3d::Zero, u32 _count = 1 );

        u32                 startFX(StringID _fxName);
        void                stopFX(u32 _handleFx);

    private:

        LinkComponent*      m_linkComponent;
        AnimLightComponent* m_animLightComponent;
        FXControllerComponent* m_fxControllerComponent;
        ObjectRef           m_dancer;
        ActorRef            m_eye;
        bbool               m_playerIsSnap;
        bbool               m_isPerfect;

        bbool               m_doorIsOpen;
        bbool               m_danceIsStarted;
        bbool               m_warmupIsStarted;


        bbool               m_stoneUnlockedByPlayer;
        bbool               m_stoneUnlockedByNPC;
        bbool               m_animUnlockIsPlayed;
        bbool               m_animOpenDoorIsPlayed;


        // who to signal when we get locked/unlocked
        u32                 m_validTemposInARow;
        u32                 m_validTemposPrev;

        // FX
        u32                 m_handleFxRed;
        u32                 m_handleFxGreen;

        u32                 m_stateFeedBack;


        struct Tempo
        {

            Tempo()
                : m_min(0.f)
                , m_max(0.f)
                , m_open(bfalse)
                , m_validated(bfalse)
            {
            }

            f32     m_min;
            f32     m_max;
            bbool   m_open;
            bbool   m_validated;

            void    update( f32 _metronome, u32& _validTemposInARow );
            void    validate( bbool _condition  );
        };

        Tempo               m_upTempo;
        Tempo               m_downTempo;

    };

    //-------------------------------------------------------------------------------------
    class Ray_DanceStoneComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DanceStoneComponent_Template,TemplateActorComponent,2698312311);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DanceStoneComponent);

    public:

        class Tempo
        {
            DECLARE_SERIALIZE();

        public:

            Tempo() : m_min(0.f), m_max(0.f) {}
            Tempo( f32 _min, f32 _max ) : m_min(_min), m_max(_max) {}

            f32         getMin() const { return m_min; }
            f32         getMax() const { return m_max; }

        private:

            f32     m_min;
            f32     m_max;
        };

        Ray_DanceStoneComponent_Template();
        ~Ray_DanceStoneComponent_Template() {}

        u32                 getNumTemposToUnlock() const { return m_numTemposToUnlock; }
        const Tempo&        getUpTempo() const { return m_upTempo; }
        const Tempo&        getDownTempo() const { return m_downTempo; }
        f32                 getSyncRatio() const { return m_syncRatio; }
        f32                 getSyncOffset() const { return m_syncOffset; }
        bbool               getDancerShouldFlip() const { return m_dancerShouldFlip; }

        f32                 getDistXSnap() const { return m_distXSnap; }
        f32                 getDistYSnap() const { return m_distYSnap; }

        u32                 getNumLumNormalReward() const { return m_numLumNormalReward; }
        u32                 getNumLumPowerReward() const { return m_numLumPowerReward; }

    private:

        u32                 m_numTemposToUnlock;
        Tempo               m_upTempo;
        Tempo               m_downTempo;
        f32                 m_syncRatio;
        f32                 m_syncOffset;
        bbool               m_dancerShouldFlip;      // should the dancer be flipped or not

        f32                 m_distXSnap;
        f32                 m_distYSnap;

        u32                 m_numLumNormalReward;
        u32                 m_numLumPowerReward;

    };

    ITF_INLINE const class Ray_DanceStoneComponent_Template * Ray_DanceStoneComponent::getTemplate() const
    {
        return static_cast<const class Ray_DanceStoneComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_DANCESTONECOMPONENT_H_
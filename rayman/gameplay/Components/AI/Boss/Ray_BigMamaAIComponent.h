#ifndef _ITF_RAY_BIGMAMAAICOMPONENT_H_
#define _ITF_RAY_BIGMAMAAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{

    class AnimGameplayEvent;
    class Ray_BigMamaAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BigMamaAIComponent, Ray_AIComponent,1359314854)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BigMamaAIComponent();
        virtual ~Ray_BigMamaAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent(Event* _event);
        //virtual void        Update( f32 _dt );

    private:

        void                init();

        void                onGameplayEvent(AnimGameplayEvent* _animEvent);
        void                processPunch(PunchStim* _evt);
        void                getChildren();
        void                triggerBubo(bbool _isMega);
        void                goToNextPhase();
        void                goToPreviousPhase();
        void                updateAnimInputs();
        void                ejectPlayers(ActorRef pos);
        void                detectLayer(Ray_EventBouncePropagate* _evt);
        const AnimBoneDyn*  getBone(const StringID& _bone);
        void                spawnEyes();
        void                playMusic();
        void                startCine();

        ActorRef            m_ejectPos;
        ActorRef            m_ejectOnHeadPos;
        ActorRef            m_finalEjectPos;
        ActorRef            m_bubo;
        ActorRef            m_secondBubo;
        ActorRef            m_megaBubo;
        ActorRef            m_outro;
        u32                 m_currentPhase;
        u32                 m_eyesHitPoints;
        SpawneeGenerator    m_eyeSpawner;

		const static u32	m_nbEyeAnims = 4;


        ITF_INLINE const class Ray_BigMamaAIComponent_Template* getTemplate() const;
    };

    //-------------------------------------------------------------------------------------

    class Ray_BigMamaAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BigMamaAIComponent_Template, Ray_AIComponent_Template,2869252870)
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BigMamaAIComponent)

    public:

        typedef ITF_VECTOR<EventPlayMusic> MusicList;

        Ray_BigMamaAIComponent_Template();
        ~Ray_BigMamaAIComponent_Template() {}

        ITF_INLINE u32          getEyesHitPoints() const        { return m_eyesHitPoints;   }
        ITF_INLINE u32          getEyesPerHit() const           { return m_eyesPerHit;      }
        ITF_INLINE const Path&  getEyeSpawn() const             { return m_eyeSpawn;        }
        ITF_INLINE const StringID& getEyeSpawnBone() const      { return m_eyeSpawnBone;    }
        ITF_INLINE const Faction& getAllowedFaction() const     { return m_allowedFaction;  }

        ITF_INLINE f32          getEjectHeight1() const         { return m_ejectHeight1;    }
        ITF_INLINE f32          getEjectHeight2() const         { return m_ejectHeight2;    }
        ITF_INLINE f32          getEjectSpeed() const           { return m_ejectSpeed;      }

        ITF_INLINE const MusicList& getMusics() const           { return m_musics;          }


    private:

        u32             m_eyesHitPoints;
        u32             m_eyesPerHit;
        Path            m_eyeSpawn;
        StringID        m_eyeSpawnBone;
        Faction         m_allowedFaction;

        f32             m_ejectHeight1;
        f32             m_ejectHeight2;
        f32             m_ejectSpeed;

        MusicList       m_musics;

};

    //-------------------------------------------------------------------------------------

    ITF_INLINE const Ray_BigMamaAIComponent_Template* Ray_BigMamaAIComponent::getTemplate() const
    {
        return static_cast<const Ray_BigMamaAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_BIGMAMAAICOMPONENT_H_
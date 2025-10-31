#ifndef _ITF_RAY_BOSSBIRDAICOMPONENT_H_
#define _ITF_RAY_BOSSBIRDAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_PHYSFORCEMODIFIER_H_
#include "engine/physics/PhysForceModifier.h"
#endif //_ITF_PHYSFORCEMODIFIER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/GraphicComponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

#ifndef _ITF_ANIMATIONBONES_H_
#include    "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_

#ifndef _ITF_OBJBINDING_H_
#include "engine/actors/ObjBinding.h"
#endif //_ITF_OBJBINDING_H_



namespace ITF
{
    class AnimGameplayEvent;
    class Ray_BossBirdAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossBirdAIComponent, Ray_AIComponent,2632013408)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BossBirdAIComponent();
        virtual ~Ray_BossBirdAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual void        onEvent(Event* _event);
        virtual void        Update( f32 _dt );

		u32					getNbLumsToSpawn(u32 _nbWanted);

    private:

        void                init();

        void                onGameplayEvent(AnimGameplayEvent* _animEvent);

        void                goToNextPhase();
        void                updateAnimInputs();
        void                setBirdTarget();
        void                updateAnim();
        void                moveToNextSequence();
        void                getChildren();
        const AnimBoneDyn*  getBone(const StringID& _bone);
        void                updateCircles(f32 _dt);
		void				checkTailDelay(f32 _dt);
        void                triggerTail(EventTrigger* _evt);
        void                enableTail();
        void                triggerBubo();
        void                triggerGeyser();
        void                shakeCamera(AnimGameplayEvent* _animEvent);
        f32                 blend(f32 _from, f32 _to, f32 _factor);
        void                spawnBirds();
		void				triggerSkullCoin();
        void                startEggs();
        void                stopEggs();
        void                dropEggs(f32 _dt);
        void                setupSpawnedObject(Actor* _actor);
        void                startSequence();
        void                playMusic();

        u32                 m_currentPhase;

        SpawneeGenerator    m_eggGenerator;
        SpawneeGenerator    m_birdGenerator;
        ActorRef            m_bubo;
        ActorRef            m_currentSequence;
        ActorRef            m_sucking;
        ActorRef            m_tail;
		ActorRef			m_skullCoin;
		ActorRef			m_skullCoin2;
        bbool               m_tailIsReady;
        u32                 m_nbEggsToSpawn;
        f32                 m_eggDelay;
        bbool               m_highBirds;
        u32                 m_lumsToSpawn;
		u32					m_nbPlayersHanging;
		f32					m_tailDelay;
		bbool				m_tailUsed;
		bbool				m_initDone;
        
        ITF_INLINE const class Ray_BossBirdAIComponent_Template* getTemplate() const;
    };

    //-------------------------------------------------------------------------------------

    class Ray_BossBirdAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossBirdAIComponent_Template, Ray_AIComponent_Template,2152536748)
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BossBirdAIComponent)

    public:
        typedef ITF_VECTOR<EventPlayMusic> MusicList;

        Ray_BossBirdAIComponent_Template();
        ~Ray_BossBirdAIComponent_Template() {}

        ITF_INLINE bbool            getIsMecha() const      { return m_isMecha;         }
        ITF_INLINE bbool            getPlayIntroMusic() const { return m_playIntroMusic;}
        ITF_INLINE bbool            getPlayIntro() const    { return m_playIntro;       }
        ITF_INLINE bbool            getPlayOutro() const    { return m_playOutro;       }
		ITF_INLINE f32				getTailDelay() const	{ return m_tailDelay;		}
        ITF_INLINE const Path &     getEgg() const          { return m_egg;             }
        ITF_INLINE const Path &     getBird() const         { return m_bird;            }
        ITF_INLINE u32              getNbEggs() const       { return m_nbEggs;          }
        ITF_INLINE f32              getFirstEggDelay() const{ return m_firstEggDelay;   }
        ITF_INLINE f32              getEggDelay() const     { return m_eggDelay;        }
        ITF_INLINE u32              getNbBirds() const      { return m_nbBirds;         }
        ITF_INLINE f32              getBirdYStart() const   { return m_birdYStart;      }
        ITF_INLINE f32              getBirdYLow() const     { return m_birdYLow;        }
        ITF_INLINE f32              getBirdXOffset() const  { return m_birdXOffset;     }
        ITF_INLINE f32              getBirdXSpace() const   { return m_birdXSpace;      }
        ITF_INLINE f32              getBirdYSpace() const   { return m_birdYSpace;      }
        ITF_INLINE const StringID&  getEggBone() const      { return m_eggBone;         }
        ITF_INLINE u32              getMaxLums() const      { return m_maxLums;         }
        ITF_INLINE const MusicList& getMusics() const       { return m_musics;          }

    private:
        bbool       m_isMecha;
        bbool       m_playIntroMusic;
        bbool       m_playIntro;
        bbool       m_playOutro;
		f32			m_tailDelay;
        Path        m_egg;
        Path        m_bird;
        u32         m_nbBirds;
        f32         m_birdYStart;
        f32         m_birdYLow;
        f32         m_birdXOffset;
        f32         m_birdXSpace;
        f32         m_birdYSpace;
        u32         m_nbEggs;
        f32         m_firstEggDelay;
        f32         m_eggDelay;
        StringID    m_eggBone;
        u32         m_maxLums;
        MusicList   m_musics;
    };

    //-------------------------------------------------------------------------------------

    ITF_INLINE const Ray_BossBirdAIComponent_Template* Ray_BossBirdAIComponent::getTemplate() const
    {
        return static_cast<const Ray_BossBirdAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_BOSSBIRDAICOMPONENT_H_
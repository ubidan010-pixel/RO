#ifndef _ITF_RAY_BOSSPLANTARENAAICOMPONENT_H_
#define _ITF_RAY_BOSSPLANTARENAAICOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif // _ITF_SPAWNER_H_

//#define BOSSP_USERETURNTOMENUFORDEMO    // TODO: remove this after demo

namespace ITF
{
    class LinkComponent;
    class Ray_BossPlantArenaAIComponent_Template;

    class Ray_BossPlantArenaAIComponent : public AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossPlantArenaAIComponent, AIComponent,2592008217)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:
        
        Ray_BossPlantArenaAIComponent();
        virtual ~Ray_BossPlantArenaAIComponent();

        virtual bbool       needsUpdate() const { return btrue;     }
        virtual bbool       needsDraw() const   { return bfalse;    }
        virtual bbool       needsDraw2D() const { return bfalse;    }

        virtual void        onEvent(Event* _event);
        void                onGameplayEvent(AnimGameplayEvent* animEvent);
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update(f32 _dt);

    private:

        ITF_INLINE const class Ray_BossPlantArenaAIComponent_Template * getTemplate() const;

        void                init();

        void                updateAnim();
        void                goToNextPhase();
        void                playMusic();
        void                updateAnimInputs();


        void                triggerBubo();
        void                updateBubo(f32 _dt);

        void                getChildren();

        void                startSequence();

        void                shakeCamera(AnimGameplayEvent* _animEvent);

        void                startCine();

        u32                 m_phaseNumber;
        ActorRef            m_bubo;
        ActorRef            m_outro;
		u32					m_buboBoneIndex;
        bbool               m_moveBubo;
		bbool				m_initDone;

#ifdef BOSSP_USERETURNTOMENUFORDEMO
        f32                 TMPDEMO_delayLoadMenu;
#endif // BOSSP_USERETURNTOMENUFORDEMO
    };

    //-------------------------------------------------------------------------------------
    class Ray_BossPlantArenaAIComponent_Template : public AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossPlantArenaAIComponent_Template,AIComponent_Template,2688442976)
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BossPlantArenaAIComponent)

    public:

        class BuboBone
        {
            DECLARE_SERIALIZE();
            StringID m_bone;
        };

        typedef ITF_VECTOR<BuboBone> BuboBones;
        typedef ITF_VECTOR<EventPlayMusic> MusicList;

        Ray_BossPlantArenaAIComponent_Template();
        ~Ray_BossPlantArenaAIComponent_Template() {}

        ITF_INLINE bbool                getIsMecha() const              { return m_isMecha;                     }
        ITF_INLINE const StringID &     getBuboBone(u32 _index) const   { return m_buboBones[_index].m_bone;    }
        ITF_INLINE const MusicList&     getMusics() const               { return m_musics;                      }

    private:

        bbool           m_isMecha;
        BuboBones       m_buboBones;
        MusicList       m_musics;
    };

    ITF_INLINE const Ray_BossPlantArenaAIComponent_Template * Ray_BossPlantArenaAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_BossPlantArenaAIComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_BOSSPLANTARENAAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSBIRDPAWNAICOMPONENT_H_
#define _ITF_RAY_BOSSBIRDPAWNAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{
    class ShapeDetectorComponent;

    class Ray_BossBirdPawnAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossBirdPawnAIComponent, Ray_AIComponent,281372016)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BossBirdPawnAIComponent();
        virtual ~Ray_BossBirdPawnAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent(Event* _event);
        virtual void        Update( f32 _dt );
        void                setup(const ActorRef &_boss);

    private:

        void                move(f32 _dt);
        void                onGameplayEvent(AnimGameplayEvent* animEvent);
        void                receiveHit(PunchStim* _hit);
        void                stopTween();
        bbool               isEnemy(Faction _mine, Faction _his);
        void                detectActorsInside();
        void                sendHitMessage(Actor* _receiver, const Vec2d& _dir);
        void                die();
        void                dieNow();
        void                spawnReward(ActorRef _actor);

        ITF_INLINE const class Ray_BossBirdPawnAIComponent_Template* getTemplate() const;

        ShapeDetectorComponent* m_shapeDetector;
        bbool                   m_isHitable;
        ActorRef                m_boss;
        f32                     m_timer;
        Vec2d                   m_speed;
        f32                     m_gravity;
        f32                     m_yGround;
        f32                     m_rotation;
    };

    //-------------------------------------------------------------------------------------

    class Ray_BossBirdPawnAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossBirdPawnAIComponent_Template, Ray_AIComponent_Template,91834156);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BossBirdPawnAIComponent);

    public:

        Ray_BossBirdPawnAIComponent_Template();
        ~Ray_BossBirdPawnAIComponent_Template() {}

        ITF_INLINE f32      getGravity() const                  { return m_gravity;         }
        ITF_INLINE f32      getScale() const                    { return m_scale;           }
        ITF_INLINE f32      getXMax() const                     { return m_xMax;            }
        ITF_INLINE Faction  getFaction() const                  { return m_faction;         }
        ITF_INLINE f32      getLifeDuration() const             { return m_lifeDuration;    }
        ITF_INLINE f32      getYGround() const                  { return m_yGround;         }
        ITF_INLINE const Vec2d& getInitSpeed() const            { return m_initSpeed;       }
		ITF_INLINE f32		getSpeedRand() const				{ return m_speedRand;		}
        ITF_INLINE f32      getRotation() const                 { return m_rotation;        }
        ITF_INLINE f32      getXBreakFactor() const             { return m_xBreakFactor;    }
        ITF_INLINE const StringID& getDeathAnim() const         { return m_deathAnim;       }
        ITF_INLINE f32      getHitSpeed() const                 { return m_hitSpeed;        }
		ITF_INLINE u32		getNbRewards() const				{ return m_nbRewards;		}

    private:

        Faction     m_faction;
        f32         m_gravity;
        f32         m_scale;
        f32         m_xMax;

        f32         m_lifeDuration;
        f32         m_yGround;
        Vec2d       m_initSpeed;
		f32			m_speedRand;
        f32         m_xBreakFactor;
        f32         m_hitSpeed;
        f32         m_rotation;

        StringID    m_deathAnim;

		u32			m_nbRewards;
    };


    //-------------------------------------------------------------------------------------

    ITF_INLINE const Ray_BossBirdPawnAIComponent_Template* Ray_BossBirdPawnAIComponent::getTemplate() const
    {
        return static_cast<const Ray_BossBirdPawnAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_BOSSBIRDPAWNAICOMPONENT_H_
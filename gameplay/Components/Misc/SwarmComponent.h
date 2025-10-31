
#ifndef _ITF_SWARMCOMPONENT_H_
#define _ITF_SWARMCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

namespace ITF
{

    class SwarmComponent : public ActorComponent
    {
    public:

        DECLARE_OBJECT_CHILD_RTTI(SwarmComponent,ActorComponent,2308967169);
        DECLARE_SERIALIZE()

        SwarmComponent();
        ~SwarmComponent();

        virtual bbool           needsUpdate() const { return btrue; }
        virtual bbool           needsDraw() const { return btrue; }
        virtual bbool           needsDraw2D() const { return bfalse; }
        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onForceMove();
        virtual void            Update( f32 _deltaTime );
        virtual void            Draw();

	protected:

        virtual void            initGenerator();
        virtual void            updateGenerator( f32 _deltaTime );
        virtual void            updateParticles( f32 _deltaTime );
        virtual void            initParticles();
        virtual void            updateSwarmComponentAABB();

        
        ITF_ParticleGenerator   m_particleGenerator;
        ResourceID              m_resource;

    private:


        ITF_INLINE const class SwarmComponent_Template*  getTemplate() const;

        void                    clear();

        
		struct ParticleData
        {
            Vec2d       m_pos;              // The current position
            Vec2d       m_speed;            // The current speed
            Vec2d       m_best;             // The best target position
            f32         m_rot;              // The current rotation
            f32         m_angularSpeed;     // The angular speed
            f32         m_bestDistSq;       // The best target distance squared
        };
		typedef SafeArray <ParticleData> ParticlesContainer;
        ParticlesContainer      m_particles;

        Vec2d                   m_best;
        f32                     m_bestDistSq;
        Vec2d                   m_target;
        f32                     m_targetCounter;
    };

    //---------------------------------------------------------------------------------------------------

    class SwarmComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SwarmComponent_Template,TemplateActorComponent,1576593566);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SwarmComponent);

    public:

        SwarmComponent_Template();
        ~SwarmComponent_Template() {}

        const Path&                             getTextureFile() const { return m_textureFile; }
        u32                                     getNumParticles() const { return m_numParticles; }
        f32                                     getStartRadius() const { return m_startRadius; }
        f32                                     getStartSpeed() const { return m_startSpeed; }
        f32                                     getFollowBestChance() const { return m_followBestChance; }
        f32                                     getFollowTargetChance() const { return m_followTargetChance; }
        f32                                     getTargetTimer() const { return m_targetTimer; }
        f32                                     getWindMultiplier() const { return m_windMultiplier; }
        f32                                     getAngularRotationScale() const { return m_angularRotationScale; }
        f32                                     getGlobalInfluence() const { return m_globalInfluence; }
        f32                                     getLocalInfluence() const { return m_localInfluence; }
        f32                                     getDamp() const { return m_damp; }
        f32                                     getStiff() const { return m_stiff; }
        const ITF_ParticleGenerator_Template&   getParticleGenerator() const { return m_particleGenerator; }

    private:

        Path                            m_textureFile;
        u32                             m_numParticles;
        f32                             m_startRadius;
        f32                             m_startSpeed;
        f32                             m_followBestChance;
        f32                             m_followTargetChance;
        f32                             m_targetTimer;
        f32                             m_windMultiplier;
        f32                             m_angularRotationScale;
        f32                             m_globalInfluence;
        f32                             m_localInfluence;
        f32                             m_damp;
        f32                             m_stiff;
        ITF_ParticleGenerator_Template  m_particleGenerator;
    };


    ITF_INLINE const SwarmComponent_Template*  SwarmComponent::getTemplate() const {return static_cast<const SwarmComponent_Template*>(m_template);}

}

#endif // _ITF_SWARMCOMPONENT_H_

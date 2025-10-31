#ifndef _ITF_RAY_WATERPERTURBATIONCOMPONENT_H_
#define _ITF_RAY_WATERPERTURBATIONCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_PERLIN_NOISE_H_
#include "Core/math/PerlinNoise.h"
#endif // _ITF_PERLIN_NOISE_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    class Ray_WaterPerturbationComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WaterPerturbationComponent, ActorComponent,4194052678)
        DECLARE_SERIALIZE()

    public:

        Ray_WaterPerturbationComponent();
        virtual ~Ray_WaterPerturbationComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        virtual void                onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void                onBecomeActive();

        virtual void                Update( f32 _dt );
        virtual void                onEvent( Event * _event);

    private:
        ITF_INLINE const class Ray_WaterPerturbationComponent_Template * getTemplate() const;

        void                        updatePos( f32 _dt );
        void                        updateWaterData();
        void                        updateFX();

        SpawneeGenerator            m_fxGenerator;
        ActorRef                    m_fxRef;
        PerlinNoise                 m_noiseGenX;
        PerlinNoise                 m_noiseGenY;
        ObjectRef                   m_polyline;
        u32                         m_surfaceEdge;
        f32                         m_surfaceT;
        bbool                       m_onSurface;
        u32                         m_closestEdge;
        f32                         m_closestT;
        f32                         m_currentTimer;
        bbool                       m_updated;
        Vec2d                       m_pos;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_WaterPerturbationComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WaterPerturbationComponent_Template,TemplateActorComponent,377221835);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WaterPerturbationComponent);

    public:

        Ray_WaterPerturbationComponent_Template();
        ~Ray_WaterPerturbationComponent_Template();

        const Path&                 getFxFile() const { return m_fxFile; }
        f32                         getNoiseAmplitudeX() const { return m_noiseAmplitudeX; }
        f32                         getNoiseFrequencyX() const { return m_noiseFrequencyX; }
        f32                         getNoisePersistenceX() const { return m_noisePersistenceX; }
        u32                         getNoiseNbOctaveX() const { return m_noiseNbOctaveX; }
        f32                         getNoiseAmplitudeY() const { return m_noiseAmplitudeY; }
        f32                         getNoiseFrequencyY() const { return m_noiseFrequencyY; }
        f32                         getNoisePersistenceY() const { return m_noisePersistenceY; }
        u32                         getNoiseNbOctaveY() const { return m_noiseNbOctaveY; }
        f32                         getNoiseTimeMultiplier() const { return m_noiseTimeMultiplier; }
        f32                         getRadius() const { return m_radius; }
        f32                         getWaterMultiplier() const { return m_waterMultiplier; }
        f32                         getWeight() const { return m_weight; }
        bbool                       getQueryPosition() const { return m_queryPosition; }

    private:

        Path                        m_fxFile;
        f32                         m_noiseAmplitudeX;
        f32                         m_noiseFrequencyX;
        f32                         m_noisePersistenceX;
        u32                         m_noiseNbOctaveX;
        f32                         m_noiseAmplitudeY;
        f32                         m_noiseFrequencyY;
        f32                         m_noisePersistenceY;
        u32                         m_noiseNbOctaveY;
        f32                         m_noiseTimeMultiplier;
        f32                         m_radius;
        f32                         m_waterMultiplier;
        f32                         m_weight;
        bbool                       m_queryPosition;
    };

    ITF_INLINE const class Ray_WaterPerturbationComponent_Template * Ray_WaterPerturbationComponent::getTemplate() const
    {
        return static_cast<const class Ray_WaterPerturbationComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_WATERPERTURBATIONCOMPONENT_H_


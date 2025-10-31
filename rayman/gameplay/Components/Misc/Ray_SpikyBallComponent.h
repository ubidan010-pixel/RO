#ifndef _ITF_RAY_SPIKYBALLCOMPONENT_H_
#define _ITF_RAY_SPIKYBALLCOMPONENT_H_

#ifndef __ITF_RAY_FLUIDSHAPERS_H__
#include "rayman/gameplay/Components/Misc/Ray_FluidShapers.h"
#endif

namespace ITF
{
    class Ray_SpikyBall_Transition;
    class Ray_SpikyBall_Circle;
    class FXControllerComponent;

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_SpikyBallComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SpikyBallComponent, ActorComponent,936470761);
    public:
        static const u32 sampleCount = 64;
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

        Ray_SpikyBallComponent();
        ~Ray_SpikyBallComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual void                Update( f32 _deltaTime );
        virtual void                onEvent( Event* _event );
        virtual bbool               needsUpdate() const {return btrue;}
        virtual bbool               needsDraw() const {return btrue;}
        virtual bbool               needsDraw2D() const {return bfalse;}
        virtual void                Draw();

    protected:
        enum State_t {State_Wave, State_Transition};

        void                        computeWave();
        void                        computeAABB();
        void                        setState(State_t _newState, u32 _circleIndex);
        void                        setScaleCausedByState(f32 _scaleFactor, const Ray_SpikyBall_Circle &_circle);
        void                        fillMesh();
        const Ray_SpikyBall_Transition *findTransition(u32 _i1, u32 _i2);
        void                        prepareFX(const StringID &_id) {m_fxToPlay = _id;}

        static bbool m_wasInitialized;
        static Vec2d m_precomputedWaveCosSin[sampleCount+1];

        static u32 m_lastTimeComputed;
        static f32 m_computedWave[sampleCount];
        Vec2d m_computedPoints[sampleCount+1];
        f32 m_scaleCausedByState;
        Color m_color;
        f32 m_spikeHeight;

        State_t m_state;
        f32 m_timeInCurrentState;

        u32 m_circleIndex, m_toCircleIndex;
        const Ray_SpikyBall_Transition *m_currentTransition;
        static Ray_FluidShaper_Spikes m_shaper;
        ITF_Mesh m_mesh;
        f32 m_maxRadiusFactor;
        ITF_IndexBuffer *m_indexBuffer;
        ResourceID m_textureID;
        FXControllerComponent *m_fxComponent;
        StringID m_fxToPlay;

        ITF_INLINE const class Ray_SpikyBallComponent_Template * getTemplate() const;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_SpikyBall_Circle : public IRTTIObject
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(Ray_SpikyBall_Circle,IRTTIObject,2461771937)
        DECLARE_SERIALIZE()

        Ray_SpikyBall_Circle() : m_radiusFactor(1.f), m_hurtRadiusFactor(1.f), m_color(Color::red()), m_hurts(btrue), m_spikeHeight(0.5f) {}

        f32 m_radiusFactor;
        f32 m_hurtRadiusFactor;
        Color m_color;
        bbool m_hurts;
        f32 m_spikeHeight;
        StringID m_fx;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_SpikyBall_Transition : public IRTTIObject
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(Ray_SpikyBall_Transition,IRTTIObject,1571425860)
        DECLARE_SERIALIZE()

        Ray_SpikyBall_Transition() : m_firstIndex(0), m_secondIndex(1), m_duration(0.1f) {}

        u32 m_firstIndex, m_secondIndex;
        f32 m_duration;
        StringID m_fx;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_SpikyBallComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SpikyBallComponent_Template, TemplateActorComponent,1442888449);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SpikyBallComponent);

    public:
        Ray_SpikyBallComponent_Template();

        const Path &getTexturePath() const {return m_texturePath;}
        const ITF_VECTOR<Ray_SpikyBall_Circle>& getCircles() const {return m_circles;}
        const ITF_VECTOR<Ray_SpikyBall_Transition>& getTransitions() const {return m_transitions;}

    private:
        ITF_VECTOR<Ray_SpikyBall_Circle> m_circles;
        ITF_VECTOR<Ray_SpikyBall_Transition> m_transitions;

        Path m_texturePath;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    ITF_INLINE const Ray_SpikyBallComponent_Template * Ray_SpikyBallComponent::getTemplate() const
    {
        return static_cast<const class Ray_SpikyBallComponent_Template *>(m_template);
    }

}

#endif

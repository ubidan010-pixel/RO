#ifndef _ITF_RAY_WAVEBALLCOMPONENT_H_
#define _ITF_RAY_WAVEBALLCOMPONENT_H_

#ifndef __ITF_RAY_FLUIDSHAPERS_H__
#include "rayman/gameplay/Components/Misc/Ray_FluidShapers.h"
#endif

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_WaveBallComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WaveBallComponent, ActorComponent,310418336);
    public:
        static const u32 sampleCount = 64;
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

        Ray_WaveBallComponent();
        ~Ray_WaveBallComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual void                Update( f32 _deltaTime );
        virtual void                onEvent( Event* _event );
        virtual bbool               needsUpdate() const {return btrue;}
        virtual bbool               needsDraw() const {return btrue;}
        virtual bbool               needsDraw2D() const {return bfalse;}
        virtual void                Draw();

    protected:
        enum State_t {State_Off, State_OffToOn, State_On, State_OnToOff};

        void                        computeWave();
        void                        computeAABB();
        void                        setState(State_t _newState);
        ITF_INLINE f32              getCurrentPrepareScale() const;
        void                        setScaleCausedByState(f32 _scaleFactor);
        void                        fillMesh();

        static bbool m_wasInitialized;
        static Vec2d m_precomputedWaveCosSin[sampleCount+1];

        static u32 m_lastTimeComputed;
        static f32 m_computedWave[sampleCount];
        Vec2d m_computedPoints[sampleCount+1];
        f32 m_scaleCausedByState;
        Color m_color;
        State_t m_state;
        f32 m_timeInCurrentState;
        f32 m_spikeMultiplier;
        bbool m_isPreparing;
        static Ray_FluidShaper_Spikes m_shaper;
        ITF_Mesh m_mesh;
        ITF_IndexBuffer *m_indexBuffer;
        ResourceID m_textureID;

        ITF_INLINE const class Ray_WaveBallComponent_Template * getTemplate() const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_WaveBallComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WaveBallComponent_Template, TemplateActorComponent,1561038693);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WaveBallComponent);

    public:
        Ray_WaveBallComponent_Template();

        f32 getScaleWhenOn() const {return m_scaleWhenOn;}
        f32 getScaleWhenOff() const {return m_scaleWhenOff;}
        f32 getOffToOnDelay() const {return m_offToOnDelay;}
        f32 getOnToOffDelay() const {return m_onToOffDelay;}
        f32 getSpikeMultiplier_Preparing() const {return m_spikeMultiplier_Preparing;}
        f32 getSpikeMultiplier_Idle() const {return m_spikeMultiplier_Idle;}
        const Color &getColorOn() const {return m_colorOn;}
        const Color &getColorOff() const {return m_colorOff;}
        const Path &getTexturePath() const {return m_texturePath;}

    private:
        f32 m_scaleWhenOff, m_scaleWhenOn;
        f32 m_offToOnDelay, m_onToOffDelay;
        f32 m_spikeMultiplier_Preparing, m_spikeMultiplier_Idle;
        Color m_colorOn, m_colorOff;
        Path m_texturePath;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    ITF_INLINE const Ray_WaveBallComponent_Template * Ray_WaveBallComponent::getTemplate() const
    {
        return static_cast<const class Ray_WaveBallComponent_Template *>(m_template);
    }

}

#endif

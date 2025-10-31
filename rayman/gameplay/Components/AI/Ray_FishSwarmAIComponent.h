#ifndef _ITF_RAY_FISHSWARMAICOMPONENT_H_
#define _ITF_RAY_FISHSWARMAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

namespace ITF
{

    class FXControllerComponent;

    class Ray_FishSwarmAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FishSwarmAIComponent, Ray_AIComponent,2501834615)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

  private:
                
        //*****************************************************************************
        // Ray_FishSwarmAIComponent::SwarmObj
        //*****************************************************************************

        class SwarmObj
        {
        public:
            SwarmObj();
            ~SwarmObj();

            Vec2d	m_pos;
            Vec2d	m_posTarget;
            Vec2d	m_posDisplay;
            Vec2d   m_posDisplayPrev;
            Vec2d	m_speed;
            Vec2d   m_dir;

            f32     m_fSpeed;
            f32     m_originalY;
            f32     m_scale;
            f32     m_flipped;
            f32     m_angle;

            u8     m_indexSinus;
            u8     m_indexAnimation;
            u8     m_curIndexAtlas;


        };

     
        //*****************************************************************************

         typedef ITF_VECTOR<SwarmObj> SwarmObjList;

    public:

        Ray_FishSwarmAIComponent();
        virtual ~Ray_FishSwarmAIComponent();

        virtual bbool   needsUpdate() const     { return btrue; }
        virtual bbool   needsDraw() const       { return btrue; }
        virtual bbool   needsDraw2D() const     { return bfalse; }

        virtual void    Update( f32 _deltaTime );
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        
        virtual void    Draw();

#ifdef ITF_SUPPORT_EDITOR
        virtual void    drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR


    private:

        ITF_INLINE const class Ray_FishSwarmAIComponent_Template*  getTemplate() const;

        struct PlayerInfo
        {
            PlayerInfo()
            : m_pos(Vec2d::Zero)
            , m_dir(Vec2d::Zero)
            , m_speed(0.0f)
            , m_selector(0.0f)
            {
                // none
            }

            Vec2d m_pos;
            Vec2d m_dir;
            f32   m_speed;
            f32   m_selector;

        };
                void    init();
                void    updateObj( f32 _dt );

                void    updateObjList( SwarmObjList & _listObj, const f32 repulseForce, const f32 zoneWidth,  const f32 zoneHeight, const u32 countPlayer, SafeArray<PlayerInfo>& playerInfoList, const f32 sqrRadiusMinScale, const f32 repulseForceMax, const u32 countTrameByAnim, f32 _dt );
                void    drawObjList( SwarmObjList & _listObj, const Vec2d actorPos, const f32 semiWidthEdge,  const f32 semiHeightEdge, const f32 zActor,AtlasDynamicObject* atlas );

                void    postLoadProperties();

                void    updateAnimation();
                void    growAABB();

    private:
     
        FXControllerComponent*  m_fxControllerComponent;

        // Atlas
        u32     m_trame;
        f32     m_time;
        u32     m_countAtlasUv;
        AtlasDynamicObject   m_atlasSwarm;
        AtlasDynamicObject   m_atlasSwarmLayer2;
        
        u32     m_countMaxParticles;
        f32     m_widthZone;
        f32     m_heightZone;

        SafeArray<Vec2d> m_listUV;

        SwarmObjList m_listObj;
        SwarmObjList m_listObjZ;

        f32 m_globalIntensity;
        f32 m_globalFrequency;

        struct SinusInfo
        {
            SinusInfo()
            : m_value(0.0f)
            , m_intensity(0.0f)
            , m_frequency(0.0f)
            , m_dephase(0.0f)
            {
                // none
            }
            f32 m_value;
            f32 m_intensity;
            f32 m_frequency;
            f32 m_dephase;

        };



        SafeArray<SinusInfo> m_listSin;

        Color m_frontColor;
        Color m_frontFogColor;
        Color m_backColor;
        Color m_backFogColor;

    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_FishSwarmAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FishSwarmAIComponent_Template, Ray_AIComponent_Template,1320950508);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_FishSwarmAIComponent);

    public:

        Ray_FishSwarmAIComponent_Template();
        ~Ray_FishSwarmAIComponent_Template();

        ITF_INLINE Path getPathAtlas() const            { return m_pathAtlas; }

        ITF_INLINE f32 getSpeedMin() const              { return m_speedMin; }
        ITF_INLINE f32 getSpeedMax() const              { return m_speedMax; }

        ITF_INLINE f32 getRadiusFree() const            { return m_radiusFree; }

        ITF_INLINE f32 getRepulseForce() const          { return m_repulseForce; }
        ITF_INLINE f32 getRepulseForceMax() const       { return m_repulseForceMax; }

        ITF_INLINE u32 getCountSinus() const            { return m_countSinus; }

        ITF_INLINE f32 getIntensityMin() const          { return m_intensityMin; }
        ITF_INLINE f32 getIntensityMax() const          { return m_intensityMax; }
        ITF_INLINE f32 getFrequencyMin() const          { return m_frequencyMin; }
        ITF_INLINE f32 getFrequencyMax() const          { return m_frequencyMax; }
        ITF_INLINE f32 getDephaseMin() const            { return m_dephaseMin; }
        ITF_INLINE f32 getDephaseMax() const            { return m_dephaseMax; }

        ITF_INLINE f32 getSizeWidthParticles() const    { return m_sizeWidthParticles; }
        ITF_INLINE f32 getSizeHeightParticles() const   { return m_sizeHeightParticles; }

        ITF_INLINE f32 getScaleMin() const              { return m_scaleMin; }
        ITF_INLINE f32 getScaleMax() const              { return m_scaleMax; }

        ITF_INLINE f32 getZOffsetBetweenLayer() const   { return m_zOffsetBetweenLayer; }

        ITF_INLINE u32 getCountAnimation() const        { return m_countAnimation; }
        ITF_INLINE u32 getCountTrameByAnimation() const { return m_countTrameByAnimation; }

        ITF_INLINE u32 getModAnimation() const          { return m_modAnimation; }

        ITF_INLINE f32 getWidthDensity() const          { return m_widthDensity; }
        ITF_INLINE f32 getHeightDensity() const         { return m_heightDensity; }

        ITF_INLINE f32 getSmoothFactorMove() const      { return m_smoothFactorMove; }
        ITF_INLINE f32 getSmoothFactorRotate() const    { return m_smoothFactorRotate; }

        ITF_INLINE f32 getGlobalIntensityMin() const    { return m_globalIntensityMin; }
        ITF_INLINE f32 getGlobalIntensityMax() const    { return m_globalIntensityMax; }
        ITF_INLINE f32 getGlobalFrequencyMin() const    { return m_globalFrequencyMin; }
        ITF_INLINE f32 getGlobalFrequencyMax() const    { return m_globalFrequencyMax; }

    private:

        // Atlas
        Path m_pathAtlas;

        // Particles
        f32 m_speedMin;
        f32 m_speedMax;
        f32 m_radiusFree;

        f32 m_repulseForce;
        f32 m_repulseForceMax;
        u32 m_countSinus;

        f32 m_intensityMin;
        f32 m_intensityMax;
        f32 m_frequencyMin;
        f32 m_frequencyMax;
        f32 m_dephaseMin;
        f32 m_dephaseMax;

        f32 m_sizeWidthParticles;
        f32 m_sizeHeightParticles;

        f32 m_scaleMin;
        f32 m_scaleMax;

        f32 m_zOffsetBetweenLayer;

        u32 m_countAnimation;
        u32 m_countTrameByAnimation;

        u32 m_modAnimation;

        f32 m_widthDensity;
        f32 m_heightDensity;

        f32 m_smoothFactorMove;
        f32 m_smoothFactorRotate;

        f32 m_globalIntensityMin;
        f32 m_globalIntensityMax;
        f32 m_globalFrequencyMin;
        f32 m_globalFrequencyMax;

    };

    const Ray_FishSwarmAIComponent_Template*  Ray_FishSwarmAIComponent::getTemplate() const {return static_cast<const Ray_FishSwarmAIComponent_Template*>(m_template);}
}

#endif // _ITF_RAY_FISHSWARMAICOMPONENT_H_
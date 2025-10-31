#ifndef _ITF_RAYWMWORLDRECAPCOMPONENT_H_
#define _ITF_RAYWMWORLDRECAPCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{
    class Ray_WM_WorldRecapComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_WorldRecapComponent, ActorComponent, 1108271511)
        DECLARE_SERIALIZE()

    public:

        Ray_WM_WorldRecapComponent()
            : m_currentWorldTag(StringID::Invalid)
            , m_state(STATE_HIDDEN)
            , m_currentNbLevels(0)
            , m_elemSpace(0.0f)
            , m_bgElemScale(0.0f)
            , m_currentTransitionTime(0.0f)
            , m_boneIndex(U32_INVALID)
            , m_screenScale(Vec2d::One)
        {};
        virtual ~Ray_WM_WorldRecapComponent(){};

        virtual bbool       needsUpdate()   const { return btrue; }
        virtual bbool       needsDraw()     const { return bfalse; }
        virtual bbool       needsDraw2D()   const { return btrue; }

        virtual void        onEvent(Event * _event);
        virtual void        onActorClearComponents();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        Update( f32 _dt );
        virtual void        Draw2D();

    private:

        enum EWorldRecapState
        {
            STATE_DISPLAYING_BACKGROUND,
            STATE_DISPLAYING_LEVELS,
            STATE_DISPLAYED,
            STATE_HIDING_LEVELS,
            STATE_WAITING_LEVELS_HIDDEN,
            STATE_HIDING_BACKGROUND,
            STATE_HIDDEN,
        };

        struct levelRecap_t
        {
            levelRecap_t()
                : m_recapActorRef(ActorRef::InvalidRef)
                , m_pRecapComp(NULL)
            {}
            ~levelRecap_t(){}

            ActorRef                                m_recapActorRef;
            class Ray_WM_LevelRecapComponent *      m_pRecapComp;
        };

        ITF_INLINE const class Ray_WM_WorldRecapComponent_Template* getTemplate() const;

        void                showRecap(const StringID& _worldTag);
        void                hideRecap();
        void                deleteLevelRecaps();
        Vec3d               getElemPosAt(u32 _index, u32 _total);

        SpawneeGenerator            m_recapSpawnee;

        StringID                    m_currentWorldTag;
        ITF_VECTOR<levelRecap_t>    m_levelRecaps;

        EWorldRecapState            m_state;
        u32                         m_currentNbLevels;

        f32                         m_elemSpace;
        f32                         m_bgElemScale;

        f32                         m_currentTransitionTime;

        u32                         m_boneIndex;

        // Screen scale
        Vec2d                       m_screenScale;

        AnimLightComponent *        m_animLightCmp;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_WM_WorldRecapComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_WorldRecapComponent_Template, TemplateActorComponent,2596140885);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_WorldRecapComponent);

    public:

        Ray_WM_WorldRecapComponent_Template()
            : m_yBGScale(50.0f) 
            , m_xMaxBGScale(80.0f)
            , m_elemMaxLength(1000.0f)
            , m_elemMaxScale(80.0f)
            , m_yOffsetFromBottom(0.0f)
            , m_transitionTime(0.2f)
            , m_boneToFollow(StringID::Invalid)
        {}
        virtual ~Ray_WM_WorldRecapComponent_Template() {}

        ITF_INLINE const Path&      getRecapElem()          const { return m_recapElem; }
        ITF_INLINE const float&     getYBGScale()           const { return m_yBGScale; }
        ITF_INLINE const float&     getXMaxBGScale()        const { return m_xMaxBGScale; }
        ITF_INLINE const float&     getElemMaxLength()      const { return m_elemMaxLength; }
        ITF_INLINE const float&     getElemMaxScale()       const { return m_elemMaxScale; }
        ITF_INLINE const float&     getyOffsetFromBottom()  const { return m_yOffsetFromBottom; }
        ITF_INLINE const float&     getTransitionTime()     const { return m_transitionTime; }
        ITF_INLINE const StringID&  getBoneToFollow()       const { return m_boneToFollow; }

    private:

        Path    m_recapElem;
        f32     m_elemMaxScale;

        f32     m_yBGScale;
        f32     m_xMaxBGScale;
        f32     m_elemMaxLength;

        f32     m_transitionTime;

        f32     m_yOffsetFromBottom;

        StringID    m_boneToFollow;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_WM_WorldRecapComponent_Template* Ray_WM_WorldRecapComponent::getTemplate() const
    {
        return static_cast<const Ray_WM_WorldRecapComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAYWMWORLDRECAPCOMPONENT_H_
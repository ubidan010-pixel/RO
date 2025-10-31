#ifndef _ITF_RAYWMNEWCOMPONENT_H_
#define _ITF_RAYWMNEWCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{
    class Ray_WM_NewComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_NewComponent, ActorComponent, 167288612)
        DECLARE_SERIALIZE()

    public:

        Ray_WM_NewComponent()
            : m_state(STATE_HIDDEN)
            , m_currentTransitionTime(0.0f)
            , m_allowNotification(btrue)
        {};
        virtual ~Ray_WM_NewComponent(){};

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

        enum ENewState
        {
            STATE_WAITING_BEFORE_DISPLAYING,
            STATE_DISPLAYING,
            STATE_DISPLAYED,
            STATE_HIDING,
            STATE_HIDDEN,
        };

        ITF_INLINE const class Ray_WM_NewComponent_Template* getTemplate() const;

        void                        showNew(const StringID& _worldTag);
        void                        hideNew();

        ENewState                   m_state;

        f32                         m_currentTransitionTime;

        AnimLightComponent *        m_animLightCmp;

        ITF_VECTOR<StringID>        m_newWorlds;

        StringID                    m_currentworldTag;
        StringID                    m_currentLevelTag;

        // Screen scale
        Vec2d                       m_screenScale;

        bbool                       m_allowNotification;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_WM_NewComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_NewComponent_Template, TemplateActorComponent,1677538008);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_NewComponent);

    public:

        Ray_WM_NewComponent_Template()
            : m_scale(50.0f) 
            , m_yOffsetFromBottom(0.0f)
            , m_xOffsetFromLeft(0.0f)
            , m_transitionTime(0.2f)
            , m_waitTime(2.0f)
        {}
        virtual ~Ray_WM_NewComponent_Template() {}

        ITF_INLINE const float&     getScale()              const { return m_scale; }
        ITF_INLINE const float&     getyOffsetFromBottom()  const { return m_yOffsetFromBottom; }
        ITF_INLINE const float&     getxOffsetFromLeft()    const { return m_xOffsetFromLeft; }
        ITF_INLINE const float&     getTransitionTime()     const { return m_transitionTime; }
        ITF_INLINE const float&     getWaitTime()           const { return m_waitTime; }

    private:

        f32     m_scale;

        f32     m_transitionTime;
        f32     m_waitTime;

        f32     m_yOffsetFromBottom;
        f32     m_xOffsetFromLeft;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_WM_NewComponent_Template* Ray_WM_NewComponent::getTemplate() const
    {
        return static_cast<const Ray_WM_NewComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAYWMNEWCOMPONENT_H_
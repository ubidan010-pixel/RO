#ifndef _ITF_SAVENOTIFICATIONCOMPONENT_H_
#define _ITF_SAVENOTIFICATIONCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class SaveNotificationComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SaveNotificationComponent,ActorComponent,4133785793);

    public:
        DECLARE_SERIALIZE()

        SaveNotificationComponent();
        ~SaveNotificationComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType _hotReload);

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return btrue; }
        virtual void        Draw2D();
        virtual void        Update( f32 _deltaTime );

        ITF_INLINE bbool    canShowNotification() const;
        ITF_INLINE bbool    canHideNotification() const;
        
        ITF_INLINE bbool    isDisplayed() const {return GetActor()->isEnabled() && m_timeStarted != 0; }
        void                show();
        void                hide();


    private:
        ITF_INLINE const class SaveNotificationComponent_Template*  getTemplate() const;
        f64                 m_timeStarted;
        bbool               m_isRegistered;
    };

    //---------------------------------------------------------------------------------------------------
    class SaveNotificationComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SaveNotificationComponent_Template,TemplateActorComponent,495222735);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SaveNotificationComponent);

    public:

        SaveNotificationComponent_Template(): 
            m_minDisplayingTime(1.5), m_relativePosition(0.5f,0.5f)
        {}
        ~SaveNotificationComponent_Template() {}

        ITF_INLINE f32 getMinDisplayingTime() const { return m_minDisplayingTime; }
        ITF_INLINE const Vec2d& getRelativePosition() const { return m_relativePosition; }

    private:
        f32     m_minDisplayingTime;
        Vec2d    m_relativePosition;
    };

    //---------------------------------------------------------------------------------------------------
    const SaveNotificationComponent_Template*  SaveNotificationComponent::getTemplate() const {return static_cast<const SaveNotificationComponent_Template*>(m_template);}

    ITF_INLINE bbool SaveNotificationComponent::canShowNotification() const {return (m_timeStarted == 0);}
    ITF_INLINE bbool SaveNotificationComponent::canHideNotification() const {return (SYSTEM_ADAPTER->getTime() - m_timeStarted) > getTemplate()->getMinDisplayingTime();}

}

#endif // _ITF_SAVENOTIFICATIONCOMPONENT_H_


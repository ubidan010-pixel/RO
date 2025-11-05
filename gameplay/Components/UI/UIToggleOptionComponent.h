#ifndef _ITF_UITOGGLEOPTIONCOMPONENT_H_
#define _ITF_UITOGGLEOPTIONCOMPONENT_H_

#ifndef _ITF_UIGAMEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIGameOptionComponent.h"
#endif //_ITF_UIGAMEOPTIONCOMPONENT_H_


namespace ITF
{
    class UIToggleOptionComponent : public UIGameOptionComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIToggleOptionComponent, UIGameOptionComponent, 3689192266);

    public:
        DECLARE_SERIALIZE()

        UIToggleOptionComponent();
        virtual ~UIToggleOptionComponent();

        virtual     bbool       needsUpdate         (       ) const             { return btrue; }
        virtual     bbool       needsDraw           (       ) const             { return bfalse; }
        virtual     bbool       needsDraw2D         (       ) const             { return bfalse; }
        virtual     void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual     void        onActorClearComponents();
        virtual     void        onBecomeActive();
        virtual     void        onBecomeInactive();

        virtual     void        onPressed           ();
        virtual     void        onReleased          ();
        virtual     void        onRollover          ();
        virtual     void        onRollout           ();
        virtual     void        onAction            (const StringID & action);

        ITF_INLINE Actor*       getLabelActor() const { return m_labelActor; }
        ITF_INLINE Actor*       getCheckboxOnActor() const { return m_checkboxOnActor; }
        ITF_INLINE Actor*       getCheckboxOffActor() const { return m_checkboxOffActor; }
        
        void                    setValue(bbool value);

    protected:
        ITF_INLINE const class UIToggleOptionComponent_Template* getTemplate() const;

    private:
        void                    clear();
        void                    resolveCheckboxActors();

        String8                 m_checkboxOnPath;
        Actor*                  m_checkboxOnActor;
        String8                 m_checkboxOffPath;
        Actor*                  m_checkboxOffActor;
    };

    class UIToggleOptionComponent_Template : public UIGameOptionComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIToggleOptionComponent_Template, UIGameOptionComponent_Template, 3491665612);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIToggleOptionComponent);

    public:
        UIToggleOptionComponent_Template();
        ~UIToggleOptionComponent_Template();

    private:

    };

    ITF_INLINE const UIToggleOptionComponent_Template* UIToggleOptionComponent::getTemplate() const
    {
        return static_cast<const UIToggleOptionComponent_Template*>(m_template);
    }

}
#endif // _ITF_UITOGGLEOPTIONCOMPONENT_H_



#ifndef _ITF_UIPROFILESLOTCOMPONENT_H_
#define _ITF_UIPROFILESLOTCOMPONENT_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_


namespace ITF
{
    class Actor;

    class UIProfileSlotComponent : public UIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIProfileSlotComponent, UIComponent, 1711782705);

    public:
        DECLARE_SERIALIZE()

        UIProfileSlotComponent();
        virtual ~UIProfileSlotComponent();

        virtual     bbool       needsUpdate         (       ) const             { return btrue; }
        virtual     bbool       needsDraw           (       ) const             { return bfalse; }
        virtual     bbool       needsDraw2D         (       ) const             { return bfalse; }
        virtual     void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual     void        Update              ( f32 _deltaTime );

        virtual     void        onActorClearComponents();
        virtual     void        onBecomeActive();
        virtual     void        onBecomeInactive();

        virtual     void        onPressed           ();
        virtual     void        onReleased          ();
        virtual     void        onRollover          ();
        virtual     void        onRollout           ();
        virtual     void        onAction            (const StringID & action);

        // Profile slot index (0-3 for 4 players)
        ITF_INLINE u32          getPlayerIndex() const { return m_playerIndex; }
        void                    setPlayerIndex(u32 index) { m_playerIndex = index; }

    protected:
        ITF_INLINE const class UIProfileSlotComponent_Template* getTemplate() const;

    private:
        virtual void            clear();

        u32                     m_playerIndex;
    };

    class UIProfileSlotComponent_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIProfileSlotComponent_Template, UIComponent_Template, 4231517306);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIProfileSlotComponent);

    public:
        UIProfileSlotComponent_Template();
        ~UIProfileSlotComponent_Template();

    private:

    };

    ITF_INLINE const UIProfileSlotComponent_Template* UIProfileSlotComponent::getTemplate() const
    {
        return static_cast<const UIProfileSlotComponent_Template*>(m_template);
    }

}
#endif // _ITF_UIPROFILESLOTCOMPONENT_H_

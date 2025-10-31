#ifndef _ITF_TODOCOMPONENT_H_
#define _ITF_TODOCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    class UITextBox;

    class TODOComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TODOComponent,ActorComponent,269527281);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        TODOComponent();
        ~TODOComponent();

        virtual void        onActorClearComponents();
        virtual void        onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        Update( f32 _dt );

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        const String&       getTextLabel() {return m_TextLabel;}
        void                setTextLabel(const String& _label) {m_TextLabel = _label;}
		void				setTextColor(f32 _alpha, f32 _red, f32 _green, f32 _blue);

        virtual void        onEvent(Event * _event);

    private:

        const class TODOComponent_Template* getTemplate() const;

        SpawneeGenerator                    m_spawner;
        ActorRef                            m_textRef;

        String                              m_TextLabel;
        bbool                               m_drawUsingEngine;

        f32                                 m_drawBoxWidth, m_drawBoxHeight;
        f32                                 m_textSize;

        f32                                 m_textAlpha;
        f32                                 m_textRed;
        f32                                 m_textGreen;
        f32                                 m_textBlue;

        f32                                 m_backgroundAlpha;
        f32                                 m_backgroundRed;
        f32                                 m_backgroundGreen;
        f32                                 m_backgroundBlue;

        bbool                               m_center;
        bbool                               m_activated;
    };

    //---------------------------------------------------------------------------------------------------

    class TODOComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TODOComponent_Template,TemplateActorComponent,1007411025);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(TODOComponent);

    public:

        TODOComponent_Template() {}
        ~TODOComponent_Template() {}

        const Path&                     getTextPath() const { return m_textPath; }

    private:

        Path                            m_textPath;
    };

    ITF_INLINE const TODOComponent_Template* TODOComponent::getTemplate() const { return static_cast<const TODOComponent_Template*>(m_template); }
}

#endif // _ITF_TODOCOMPONENT_H_


#ifndef _ITF_PLAYANIMONTOUCHPOLYLINECOMPONENT_H_
#define _ITF_PLAYANIMONTOUCHPOLYLINECOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    class PlayAnimOnTouchPolylineComponent : public ActorComponent, IUpdateAnimInputCriteria
    {
    public:

        DECLARE_OBJECT_CHILD_RTTI(PlayAnimOnTouchPolylineComponent,ActorComponent,2276589626);
        DECLARE_SERIALIZE()

        PlayAnimOnTouchPolylineComponent();
        virtual ~PlayAnimOnTouchPolylineComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _deltaTime );
        virtual void                updateAnimInput();
        virtual void                validate(bbool &_isComponentValidated);

    protected:

        ITF_INLINE const class PlayAnimOnTouchPolylineComponent_Template*  getTemplate() const;

        class AnimatedComponent*    m_animComponent;
        class PolylineComponent*    m_polyComponent;
        
        f32                         m_timer;
        f32                         m_animPos;
        f32                         m_animSpeed;
    };

    //---------------------------------------------------------------------------------------------------

    class PlayAnimOnTouchPolylineComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PlayAnimOnTouchPolylineComponent_Template,TemplateActorComponent,3958001507);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PlayAnimOnTouchPolylineComponent);

    public:

        PlayAnimOnTouchPolylineComponent_Template();
        ~PlayAnimOnTouchPolylineComponent_Template() {}

        f32                         getMaxSpeed() const { return m_maxSpeed; }
        f32                         getStiff() const { return m_stiff; }
        f32                         getDamp() const { return m_damp; }

    private:

        f32                         m_maxSpeed;
        f32                         m_stiff;
        f32                         m_damp;
    };


    ITF_INLINE const PlayAnimOnTouchPolylineComponent_Template*  PlayAnimOnTouchPolylineComponent::getTemplate() const {return static_cast<const PlayAnimOnTouchPolylineComponent_Template*>(m_template);}
}

#endif // _ITF_PLAYANIMONTOUCHPOLYLINECOMPONENT_H_

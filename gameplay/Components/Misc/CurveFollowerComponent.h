#ifndef _ITF_CURVEFOLLOWERCOMPONENT_H_
#define _ITF_CURVEFOLLOWERCOMPONENT_H_

namespace ITF
{
    class CurveComponent;

    class CurveFollowerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CurveFollowerComponent, ActorComponent,3282771180);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        // ********************************************************************

     public:

        CurveFollowerComponent();
        virtual ~CurveFollowerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const   { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onBecomeActive();
        virtual void        Update( f32 _dt );
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event * _event );

                void        setDisabled(bbool _value);

    private:
                void        start();

    private:

        ITF_INLINE const class CurveFollowerComponent_Template*  getTemplate() const;

        CurveComponent* m_curveComponent;
        f32             m_dist;
        bbool           m_disabled;
        bbool           m_playing;
        bbool           m_isTrig;

    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class CurveFollowerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CurveFollowerComponent_Template,TemplateActorComponent,4222421127);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(CurveFollowerComponent);

    public:

        CurveFollowerComponent_Template();
        ~CurveFollowerComponent_Template();

        ITF_INLINE f32 getSpeed() const  { return m_speed; }
        ITF_INLINE bbool getLoop() const { return m_loop; }


    private:

        f32     m_speed;
        bbool   m_loop;

    };


    const CurveFollowerComponent_Template*  CurveFollowerComponent::getTemplate() const {return static_cast<const CurveFollowerComponent_Template*>(m_template);}
};

#endif //_ITF_CURVEFOLLOWERCOMPONENT_H_

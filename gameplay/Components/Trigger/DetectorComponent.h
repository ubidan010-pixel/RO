#ifndef _ITF_DETECTORCOMPONENT_H_
#define _ITF_DETECTORCOMPONENT_H_

namespace ITF
{
    class DetectorComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(DetectorComponent, ActorComponent,1845751495);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        DetectorComponent();
        virtual ~DetectorComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onBecomeInactive();
        virtual void        onCheckpointLoaded();
        virtual void        onEvent( Event* _event );

        ITF_INLINE ActorRefList& getActorsInside() { return m_actorsInside; }
        ITF_INLINE const ActorRefList& getActorsInside()const  { return m_actorsInside; }
        bbool               isActorInside( ActorRef _actor ) const { return m_actorsInside.find(_actor) >= 0; }

        ITF_INLINE bbool    isDisabled() const { return m_disabled; }
        ITF_INLINE void     setDisabled( const bbool _value ) { m_disabled = _value; if ( _value ) m_actorsInside.clear(); }

    protected:
        ITF_INLINE const class DetectorComponent_Template*  getTemplate() const;

        ActorRefList        m_actorsInside;
        bbool               m_disabled;
    };

    //-------------------------------------------------------------------------------------
    class DetectorComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(DetectorComponent_Template,TemplateActorComponent,2207592726);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(DetectorComponent);

    public:

        DetectorComponent_Template();
        ~DetectorComponent_Template() {}

        ITF_INLINE bbool getResetOnCheckpoint() const {return m_resetOnCheckpoint;}

    private:

        bbool               m_resetOnCheckpoint;
    };


    const DetectorComponent_Template*  DetectorComponent::getTemplate() const {return static_cast<const DetectorComponent_Template*>(m_template);}

};

#endif //_ITF_DETECTORCOMPONENT_H_
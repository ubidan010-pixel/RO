#ifndef _ITF_PLAYANIMBASEDONBONEANGLECOMPONENT_H_
#define _ITF_PLAYANIMBASEDONBONEANGLECOMPONENT_H_

namespace ITF
{
    class PlayAnimBasedOnBoneAngleComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PlayAnimBasedOnBoneAngleComponent, ActorComponent,1943591819);

    public:

        DECLARE_SERIALIZE()

        PlayAnimBasedOnBoneAngleComponent();
        virtual ~PlayAnimBasedOnBoneAngleComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event );

    private:

        ITF_INLINE const class PlayAnimBasedOnBoneAngleComponent_Template*  getTemplate() const;
        void                processAnimUpdated( EventAnimUpdated* _animUpdated );

        AnimLightComponent* m_animComponent;
    };

    //-------------------------------------------------------------------------------------
    class PlayAnimBasedOnBoneAngleComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PlayAnimBasedOnBoneAngleComponent_Template,TemplateActorComponent,4264622728);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PlayAnimBasedOnBoneAngleComponent);

    public:

        PlayAnimBasedOnBoneAngleComponent_Template();
        ~PlayAnimBasedOnBoneAngleComponent_Template() {}

        const StringID&     getBoneName() const { return m_boneName; }
        const Angle&        getMinAngle() const { return m_minAngle; }
        const Angle&        getMaxAngle() const { return m_maxAngle; }

    private:

        StringID            m_boneName;                 // the bone whose angle we read
        Angle               m_minAngle;                 // minimum angle of the bone
        Angle               m_maxAngle;                 // maximum angle of the bone
    };



    ITF_INLINE const PlayAnimBasedOnBoneAngleComponent_Template*  PlayAnimBasedOnBoneAngleComponent::getTemplate() const {return static_cast<const PlayAnimBasedOnBoneAngleComponent_Template*>(m_template);}


}

#endif // _ITF_PLAYANIMBASEDONBONEANGLECOMPONENT_H_

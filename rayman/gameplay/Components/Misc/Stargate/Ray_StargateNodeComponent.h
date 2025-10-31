#ifndef _ITF_RAY_STARGATENODECOMPONENT_H_
#define _ITF_RAY_STARGATENODECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    enum StargateNodeType
    {
        STARGATENODETYPE_POINT,
        STARGATENODETYPE_TELEPORT,
    };

    class Ray_StargateNodeComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StargateNodeComponent, ActorComponent,2612773739)
        DECLARE_SERIALIZE()

    public:

        Ray_StargateNodeComponent();
        virtual ~Ray_StargateNodeComponent();

        virtual bbool               needsUpdate() const { return bfalse; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        ITF_INLINE StargateNodeType getType() const;
        ITF_INLINE f32              getFadeTime() const;
        ITF_INLINE f32              getFadeLength() const;
        ITF_INLINE f32              getTimeMultiplier() const;

    private:

        ITF_INLINE const class Ray_StargateNodeComponent_Template* getTemplate() const;

    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_StargateNodeComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StargateNodeComponent_Template, TemplateActorComponent,521951556);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_StargateNodeComponent);

    public:

        

        Ray_StargateNodeComponent_Template();
        virtual ~Ray_StargateNodeComponent_Template();

        StargateNodeType                    getType() const { return m_type; }
        f32                                 getFadeTime() const { return m_fadeTime; }
        f32                                 getFadeLength() const { return m_fadeLength; }
        f32                                 getTimeMultiplier() const { return m_timeMultiplier; }

    private:

        StargateNodeType                    m_type;
        f32                                 m_fadeTime;
        f32                                 m_fadeLength;
        f32                                 m_timeMultiplier;
    };

    //*****************************************************************************

    ITF_INLINE const Ray_StargateNodeComponent_Template* Ray_StargateNodeComponent::getTemplate() const
    {
        return static_cast<const Ray_StargateNodeComponent_Template*>(m_template);
    }

    ITF_INLINE StargateNodeType Ray_StargateNodeComponent::getType() const { return getTemplate()->getType(); }
    ITF_INLINE f32 Ray_StargateNodeComponent::getFadeTime() const { return getTemplate()->getFadeTime(); }
    ITF_INLINE f32 Ray_StargateNodeComponent::getFadeLength() const { return getTemplate()->getFadeLength(); }
    ITF_INLINE f32 Ray_StargateNodeComponent::getTimeMultiplier() const { return getTemplate()->getTimeMultiplier(); }

}; // end namespace

#endif //_ITF_RAY_STARGATENODECOMPONENT_H_

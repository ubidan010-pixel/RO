#ifndef _ITF_RAY_SUBANCHORCOMPONENT_H_
#define _ITF_RAY_SUBANCHORCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF {

class Ray_SubAnchor_Template;
class Ray_SubAnchorComponent_Template;

//------------------------------------------------------------------------------
class Ray_SubAnchor
{
    DECLARE_SERIALIZE()
    
public:
    Ray_SubAnchor()
        : m_name()
        , m_pos(Vec3d::Zero)
        , m_actor(NULL)
        , m_template(NULL)
    {}
    
    ITF_INLINE const Ray_SubAnchor_Template* getTemplate() const { return m_template; }

    void init(Ray_SubAnchor* _other, Actor* _actor, const Ray_SubAnchor_Template* _template);
    void clamp();

    ITF_INLINE const StringID& getName() const { return m_name; }
    const Vec3d getPos() const;

#ifdef ITF_SUPPORT_EDITOR
    void editor_setPos(const Vec3d& _val);
#endif // ITF_SUPPORT_EDITOR

private:
    StringID m_name;
    Vec3d m_pos;
    Actor* m_actor;
    const Ray_SubAnchor_Template* m_template;
};
typedef ITF_VECTOR<Ray_SubAnchor> Ray_SubAnchorList;

//------------------------------------------------------------------------------
class Ray_SubAnchor_Template
{
    DECLARE_SERIALIZE()
    
public:
    Ray_SubAnchor_Template()
        : m_name()
        , m_defaultPos(Vec3d::Zero)
        , m_color(Color::white())
        , m_clampAABB(Vec2d::Zero, Vec2d::Zero)
    {}
    
    ITF_INLINE const StringID& getName() const { return m_name; }
    ITF_INLINE const Vec3d& getDefaultPos() const { return m_defaultPos; }
    ITF_INLINE const Color& getColor() const { return m_color; }
    ITF_INLINE const AABB& getClampAABB() const { return m_clampAABB; }

private:
    StringID m_name;
    Vec3d m_defaultPos;
    Color m_color;
    AABB m_clampAABB;
};
typedef ITF_VECTOR<Ray_SubAnchor_Template> Ray_SubAnchor_TemplateList;

//------------------------------------------------------------------------------
class Ray_SubAnchorComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_SubAnchorComponent, ActorComponent,571243035);
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_SubAnchorComponent();
    virtual ~Ray_SubAnchorComponent();

    virtual bbool needsUpdate() const { return bfalse; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }
    
    virtual void onActorLoaded(Pickable::HotReloadType _hotReload);

#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const;
#endif // ITF_SUPPORT_EDITOR

    const Vec3d getPos(const StringID& _name) const;

#ifdef ITF_SUPPORT_EDITOR
    void editor_setPos(const StringID& _name, const Vec3d& _val);
    ITF_INLINE Ray_SubAnchorList& editor_getSubAnchors() { return m_subAnchors; }
#endif // ITF_SUPPORT_EDITOR

private:
    ITF_INLINE const Ray_SubAnchorComponent_Template* getTemplate() const;

    Ray_SubAnchor* get(const StringID& _name);
    const Ray_SubAnchor* get(const StringID& _name) const;
    
    Ray_SubAnchorList m_subAnchors;
};

//------------------------------------------------------------------------------
class Ray_SubAnchorComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_SubAnchorComponent_Template, TemplateActorComponent,2416740633);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SubAnchorComponent);

public:
    Ray_SubAnchorComponent_Template();
    virtual ~Ray_SubAnchorComponent_Template();

    ITF_INLINE const Ray_SubAnchor_TemplateList& getSubAnchors() const { return m_subAnchors; }
    
private:
    Ray_SubAnchor_TemplateList m_subAnchors;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_SubAnchorComponent_Template*
Ray_SubAnchorComponent::getTemplate() const
{
    return static_cast<const Ray_SubAnchorComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_SUBANCHORCOMPONENT_H_
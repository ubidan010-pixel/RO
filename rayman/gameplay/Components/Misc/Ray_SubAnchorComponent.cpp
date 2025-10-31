#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SUBANCHORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SubAnchorComponent.h"
#endif //_ITF_RAY_SUBANCHORCOMPONENT_H_

#ifndef ITF_CONSOLE_FINAL
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#endif ITF_CONSOLE_FINAL

namespace ITF {

//------------------------------------------------------------------------------
BEGIN_SERIALIZATION(Ray_SubAnchor)
    SERIALIZE_MEMBER("name", m_name);
    SERIALIZE_MEMBER("pos", m_pos);
END_SERIALIZATION()

//------------------------------------------------------------------------------
void Ray_SubAnchor::init(Ray_SubAnchor* _other, Actor* _actor, const Ray_SubAnchor_Template* _template)
{
    if (_other)
    {
        ITF_ASSERT(_other->getName() == _template->getName());

        m_name = _other->getName();
        m_pos = _other->m_pos; // if was serialized, use serialized pos
    }
    else
    {
        m_name = _template->getName();
        m_pos = _template->getDefaultPos(); // if not, use default pos
    }
    
    m_actor = _actor;
    m_template = _template;
    
    clamp();
}

//------------------------------------------------------------------------------
void Ray_SubAnchor::clamp()
{
    if (!getTemplate()->getClampAABB().isZero())
    {
        if (!getTemplate()->getClampAABB().isValid())
        {
            ITF_WARNING(m_actor, bfalse, "Invalid sub-anchor clamp AABB");
        }
        else
        {
            const Vec2d& min = getTemplate()->getClampAABB().getMin();
            const Vec2d& max = getTemplate()->getClampAABB().getMax();
            m_pos.m_x = Clamp(m_pos.m_x, min.m_x, max.m_x);
            m_pos.m_y = Clamp(m_pos.m_y, min.m_y, max.m_y);
        }
    }
}

//------------------------------------------------------------------------------
const Vec3d Ray_SubAnchor::getPos() const
{
    return m_actor->getWorldInitialPos() + m_pos;
}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void Ray_SubAnchor::editor_setPos(const Vec3d& _val)
{
    m_pos = _val - m_actor->getWorldInitialPos();
    clamp();
}
#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
BEGIN_SERIALIZATION(Ray_SubAnchor_Template)
    SERIALIZE_MEMBER("name", m_name);
    SERIALIZE_MEMBER("defaultPos", m_defaultPos);
    SERIALIZE_MEMBER("color", m_color);
    SERIALIZE_OBJECT("clampAABB", m_clampAABB);
END_SERIALIZATION()

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_SubAnchorComponent)
BEGIN_SERIALIZATION_CHILD(Ray_SubAnchorComponent)
    SERIALIZE_CONTAINER_OBJECT("subAnchors", m_subAnchors);
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_SubAnchorComponent)
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_SubAnchorComponent::Ray_SubAnchorComponent()
    : Super()
    , m_subAnchors()
{
}

//------------------------------------------------------------------------------
Ray_SubAnchorComponent::~Ray_SubAnchorComponent()
{
}

//------------------------------------------------------------------------------
void Ray_SubAnchorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    
    // init sub anchors
    // NB[LaurentCou]: re-uses previously saved positions when the name matches,
    // even if the list changed for the template
    Ray_SubAnchorList newSubAnchors;
    Ray_SubAnchor_TemplateList::const_iterator it;
    for (it = getTemplate()->getSubAnchors().begin(); it != getTemplate()->getSubAnchors().end(); ++it)
    {
        const Ray_SubAnchor_Template& subAnchorTemplate = *it;
        Ray_SubAnchor* oldSubAnchor = get(subAnchorTemplate.getName());
        Ray_SubAnchor newSubAnchor;
        newSubAnchor.init(oldSubAnchor, m_actor, &subAnchorTemplate);
        newSubAnchors.push_back(newSubAnchor);
    }
    m_subAnchors = newSubAnchors; // copy
}

//------------------------------------------------------------------------------
const Vec3d Ray_SubAnchorComponent::getPos(const StringID& _name) const
{
    const Ray_SubAnchor* subAnchor = get(_name);
    if (!subAnchor)
    {
        ITF_ASSERT_MSG(bfalse, "Sub anchor %s not found", _name.getDebugString());
        return Vec3d::Zero;
    }
    return subAnchor->getPos();
}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void Ray_SubAnchorComponent::editor_setPos(const StringID& _name, const Vec3d& _val)
{
    Ray_SubAnchor* subAnchor = get(_name);
    if (!subAnchor)
    {
        ITF_ASSERT_MSG(bfalse, "Sub anchor %s not found", _name.getDebugString());
        return;
    }
    subAnchor->editor_setPos(_val);
}
#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
Ray_SubAnchor* Ray_SubAnchorComponent::get(const StringID& _name)
{
    // find given sub anchor
    Ray_SubAnchorList::iterator it;
    for (it = m_subAnchors.begin(); it != m_subAnchors.end(); ++it)
    {
        Ray_SubAnchor& subAnchor = *it;
        if (subAnchor.getName() == _name)
        {
            return &subAnchor;
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------
const Ray_SubAnchor* Ray_SubAnchorComponent::get(const StringID& _name) const
{
    // find given sub anchor
    Ray_SubAnchorList::const_iterator it;
    for (it = m_subAnchors.begin(); it != m_subAnchors.end(); ++it)
    {
        const Ray_SubAnchor& subAnchor = *it;
        if (subAnchor.getName() == _name)
        {
            return &subAnchor;
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void Ray_SubAnchorComponent::drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const
{
    Super::drawEdit(_drawInterface, _flags);

    // debug draw sub anchors so they won't get lost
    Ray_SubAnchorList::const_iterator it;
    for (it = m_subAnchors.begin(); it != m_subAnchors.end(); ++it)
    {
        const Ray_SubAnchor& subAnchor = *it;

        // line from actor
        DebugDraw::line3D(
            m_actor->getPos(), subAnchor.getPos(),
            subAnchor.getTemplate()->getColor());

#ifdef ITF_DEBUG_STRINGID
        // name
        DebugDraw::text(
            subAnchor.getPos(),
            subAnchor.getTemplate()->getColor(),
            subAnchor.getName().getDebugString());
#endif
    }
}
#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_SubAnchorComponent_Template)
BEGIN_SERIALIZATION(Ray_SubAnchorComponent_Template)
    SERIALIZE_CONTAINER_OBJECT("subAnchors", m_subAnchors);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_SubAnchorComponent_Template::Ray_SubAnchorComponent_Template()
    : Super()
    , m_subAnchors()
{
}

//------------------------------------------------------------------------------
Ray_SubAnchorComponent_Template::~Ray_SubAnchorComponent_Template()
{
}

} // namespace ITF

#ifndef _ITF_BASEAICONTROLLERCOMPONENT_H_
#define _ITF_BASEAICONTROLLERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

class BaseAIControllerComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(BaseAIControllerComponent,ActorComponent,3592661810);

public:

    BaseAIControllerComponent();
    virtual ~BaseAIControllerComponent();

    virtual bbool       needsUpdate() const { return btrue; }
    virtual bbool       needsDraw() const { return bfalse; }
    virtual bbool       needsDraw2D() const { return bfalse; }

    virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void        Update( f32 _dt ) = 0;

    void                setMoveDir( const Vec2d& _dir ) { m_moveDirThisFrame = m_moveDir = _dir; }
    const Vec2d&        getMoveDir() const { return m_moveDir; }
    const Vec2d&        getMoveDirThisFrame() const { return m_moveDirThisFrame; }

protected:

    const class BaseAIControllerComponent_Template* getTemplate() const;

    class PhysComponent*                            m_phys;
    Vec2d                                           m_moveDir;
    Vec2d                                           m_moveDirThisFrame;
};

class BaseAIControllerComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(BaseAIControllerComponent_Template,TemplateActorComponent,3202376644);

public:

    BaseAIControllerComponent_Template();
    ~BaseAIControllerComponent_Template();

private:

};

ITF_INLINE const class BaseAIControllerComponent_Template* BaseAIControllerComponent::getTemplate() const
{
    return static_cast<const class BaseAIControllerComponent_Template*>(m_template);
}

}

#endif // _ITF_BASEAICONTROLLERCOMPONENT_H_
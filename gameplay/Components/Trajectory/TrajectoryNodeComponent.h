#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#define _ITF_TRAJECTORYNODECOMPONENT_H_

#ifndef _ITF_TRAJECTORYDATA_H_
#include "gameplay/Components/Trajectory/TrajectoryData.h"
#endif //_ITF_TRAJECTORYDATA_H_

namespace ITF {

class TrajectoryNodeComponent_Template;

//------------------------------------------------------------------------------
class TrajectoryNodeComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(TrajectoryNodeComponent,ActorComponent,1976963548);
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    TrajectoryNodeComponent();
    virtual ~TrajectoryNodeComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void Update( f32 _dt );

    bbool useCurvedEnd() const;

    typedef FixedArray<TrajectoryNodeComponent*, 64> NodeList;
    bbool hasParentNode() const;
    void getParentNodes(NodeList& _nodes, bbool _bOnlyFirst = bfalse) const;
    bbool hasChildNode() const;
    void getChildNodes(NodeList& _nodes, bbool _bOnlyFirst = bfalse) const;

    bbool move(
        TrajectoryData& _data,
        const bbool& _isWaiting,
        Vec3d& _newPos,
        Vec3d & _tangent,
        bbool& _flip
        );

    const StringID& getName() const;
        
#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const;
    virtual void onEditorCreated( Actor* _original );
#endif // ITF_SUPPORT_EDITOR

#ifdef ITF_SUPPORT_DEBUGFEATURE
    ITF_INLINE static bbool getDrawDebug() { return s_drawDebug; }
    ITF_INLINE static void setDrawDebug(bbool _value) { s_drawDebug = _value; }
#endif // ITF_SUPPORT_DEBUGFEATURE

private:
    ITF_INLINE const TrajectoryNodeComponent_Template* getTemplate() const;

    class LinkComponent* m_linkComponent;

#ifdef ITF_SUPPORT_DEBUGFEATURE
    static bbool s_drawDebug;
    static const f32 s_lineWidth;
    static const f32 s_corridorWidth;

    typedef FixedArray<const TrajectoryNodeComponent*, 128> ProcessedNodeList;
    void drawDebugRoot( ProcessedNodeList& _processedNodes ) const;
    void drawChildren( const TrajectoryNodeComponent* n0, ProcessedNodeList& _processedNodes ) const;

    void drawSegment(
        const TrajectoryNodeComponent* n0,
        const TrajectoryNodeComponent* n1,
        const TrajectoryNodeComponent* n2,
        const TrajectoryNodeComponent* n3
        ) const;
#endif // ITF_SUPPORT_DEBUGFEATURE
};

//------------------------------------------------------------------------------
class TrajectoryNodeComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(TrajectoryNodeComponent_Template,TemplateActorComponent,67924776);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(TrajectoryNodeComponent);

public:
    TrajectoryNodeComponent_Template();
    ~TrajectoryNodeComponent_Template() {}

    ITF_INLINE bbool useCurvedEnd() const { return m_useCurvedEnd; }
    ITF_INLINE const StringID& getName() const { return m_name; }

private:
    bbool m_useCurvedEnd;
    StringID m_name;
};

//------------------------------------------------------------------------------
ITF_INLINE
const TrajectoryNodeComponent_Template*
TrajectoryNodeComponent::getTemplate() const
{
    return static_cast<const TrajectoryNodeComponent_Template*>(m_template);
}
    
} // namespace ITF

#endif //_ITF_TRAJECTORYNODECOMPONENT_H_

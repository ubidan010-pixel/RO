#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOSSMORAYNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossMorayNodeComponent.h"
#endif //_ITF_RAY_BOSSMORAYNODECOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BossMorayNodeComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BossMorayNodeComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BossMorayNodeComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "Ray_BossMorayNodeComponent requires a LinkComponent");
        VALIDATE_COMPONENT_PARAM("debugCorridorWidth", getTemplate()->getDebugCorridorWidth() > 0, "must be positive!");
    END_VALIDATE_COMPONENT()


    Ray_BossMorayNodeComponent::Ray_BossMorayNodeComponent()
        : m_linkComponent(NULL)
    {
    }

    Ray_BossMorayNodeComponent::~Ray_BossMorayNodeComponent()
    {
    }


    void Ray_BossMorayNodeComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();   // mandatory, validated
    }

    bbool Ray_BossMorayNodeComponent::move( const Actor* _prevNode, const Actor* _nextNode, f32& _dist, Vec3d& _newPos )
    {
        const Segment segment(_prevNode, m_actor, _nextNode);
        return segment.getPosAtDist(_dist, _newPos);
    }

    void Ray_BossMorayNodeComponent::onNodeReached( Actor* _activator )
    {
        Ray_EventBossMorayNodeReached eventNodeReached = getTemplate()->getEventNodeReached(); // copy from template
        eventNodeReached.setSender(m_actor->getRef());
        eventNodeReached.setActivator(_activator->getRef());
        eventNodeReached.setActivated(btrue);

        //m_actor->onEvent(&eventNodeReached);
        _activator->onEvent(&eventNodeReached);
        m_linkComponent->sendEventToChildren(&eventNodeReached);
    }

    bbool Ray_BossMorayNodeComponent::hasParentNode() const
    {
        return LINKMANAGER->getParents(m_actor->getRef()) != NULL;
    }

    void Ray_BossMorayNodeComponent::getParentNodes( NodeList& _nodes ) const
    {
        if ( ObjectRefList* parents = LINKMANAGER->getParents(m_actor->getRef()) )
        {
            for (u32 i = 0; i < parents->size(); i++)
            {
                if ( BaseObject* parentObj = (*parents)[i].getObject() )
                {
                    if ( Actor* parent = parentObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                    {
                        if ( Ray_BossMorayNodeComponent* parentNode = parent->GetComponent<Ray_BossMorayNodeComponent>() )
                        {
                            _nodes.push_back(parentNode);
                        }
                    }
                }
            }
        }
    }

    void Ray_BossMorayNodeComponent::getChildNodes( const LinkComponent* _linkComponent, NodeList& _nodes )
    {
#ifndef ITF_CONSOLE_FINAL
        // in case the node is in data error (RO-10339)
        if (!_linkComponent)
        {
            return;
        }
#endif // ITF_CONSOLE_FINAL

        AIUtils::LinkIterator iterator(_linkComponent);
        while ( Actor* child = iterator.getNextRelativeActor() )
        {
            if ( Ray_BossMorayNodeComponent* childNode = child->GetComponent<Ray_BossMorayNodeComponent>() )
            {
                _nodes.push_back(childNode);
            }
        }
    }

    void Ray_BossMorayNodeComponent::getChildNodes( const LinkComponent* _linkComponent, NodeDataList& _nodes )
    {
#ifndef ITF_CONSOLE_FINAL
        // in case the node is in data error (RO-10339)
        if (!_linkComponent)
        {
            return;
        }
#endif // ITF_CONSOLE_FINAL

        const Actor* parent = _linkComponent->GetActor();

        const LinkComponent::ChildrenList& children = _linkComponent->getChildren();
        const u32 childrenCount = children.size();

        for (u32 i = 0; i < childrenCount; i++)
        {
            const ChildEntry& childEntry = children[i];

            if ( Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(parent, childEntry.getPath()) )
            {
#ifndef ITF_CONSOLE_FINAL
                if ( pickable->hasDataError() )
                {
                    continue;
                }
#endif
                if ( pickable->getObjectType() == BaseObject::eActor )
                {
                    Actor* child = static_cast<Actor*>(pickable);
                    if ( Ray_BossMorayNodeComponent* childNode = child->GetComponent<Ray_BossMorayNodeComponent>() )
                    {
                        _nodes.push_back(NodeData());
                        NodeData& node = _nodes[_nodes.size()-1];
                        node.m_node = childNode;
                        node.m_linkData = &childEntry;
                    }
                }
            }
        }
    }

#ifdef ITF_SUPPORT_EDITOR

    void Ray_BossMorayNodeComponent::onEditorCreated( Actor* _original )
    {
        Super::onEditorCreated(_original);

        if (_original)
        {
            // Clear links towards this
            if(ObjectRefList* pParentFromRefList = LINKMANAGER->getParents(m_actor->getRef()))
            {
                SafeArray<BaseObject*> pParentsFrom;
                ID_SERVER->getObjectListNoNull(*pParentFromRefList, pParentsFrom);

                for(u32 iP = 0; iP < pParentsFrom.size(); ++iP)
                {
                    Actor* pParent = static_cast<Actor*>(pParentsFrom[iP]);
                    if(LinkComponent* pLC = pParent->GetComponent<LinkComponent>())
                    {
                        ObjectPath path;
                        if(SceneObjectPathUtils::getRelativePathFromObject(pParent, m_actor, path))
                            pLC->removeChild(path, btrue);
                    }
                }
            }

            if (m_linkComponent)
            {
                m_linkComponent->clearLinks();
            }

            if ( Ray_BossMorayNodeComponent* parent = _original->GetComponent<Ray_BossMorayNodeComponent>() )
            {
                ObjectPath relPath;
                SceneObjectPathUtils::getRelativePathFromObject(_original,m_actor,relPath);
                if ( relPath.isValid() && parent->m_linkComponent )
                {
                    parent->m_linkComponent->addChild(relPath);
                }
            }

            m_actor->replaceReinitDataByCurrent();
        }
    }


    bbool Ray_BossMorayNodeComponent::s_drawDebug(bfalse);

    void Ray_BossMorayNodeComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (s_drawDebug)
        {
            // NOTE: this method fails if the root is not updated.
            drawDebug();
        }
    }

    void Ray_BossMorayNodeComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
        {
            return;
        }

        drawDebug();
    }

    static const f32 s_lineWidth = 5.f;

    void Ray_BossMorayNodeComponent::drawDebug() const
    {
        DebugSegmentList segments;

        ProcessedNodeList processedNodes;
        buildNodeList(processedNodes, segments, NULL);

        if (segments.size() == 0)
        {
            // no nodes, not even ourself: this means we're looping;
            // -> we can build the list with our children directly

            processedNodes.clear();

            NodeList parents;
            getParentNodes(parents);

            for (u32 i = 0; i < parents.size(); i++)
            {
                buildChildrenList(processedNodes, segments, parents[i]);
            }
        }

        bbool oneSelected = bfalse;
        for (u32 i = 0; i < processedNodes.size(); i++)
        {
            if (processedNodes[i]->GetActor()->isSelected())
            {
                oneSelected = btrue;
                break;
            }
        }

        for (u32 i = 0; i < segments.size(); i++)
        {
            const DebugSegment& segment = segments[i];
            drawSegment(segment.m_n0, segment.m_n1, segment.m_n2, *segment.m_linkData, segment.m_isEnd,
                oneSelected, oneSelected);
        }

#ifdef DEVELOPER_BEN
        DebugDraw::text("drawing %d segments (%d nodes)", segments.size(), processedNodes.size());
#endif // DEVELOPER_BEN
    }

    void Ray_BossMorayNodeComponent::buildNodeList( ProcessedNodeList& _processedNodes, DebugSegmentList& _segments, const Node* _prev ) const
    {
        if ( _processedNodes.find(this) != -1 )
        {
            // already processed
            return;
        }
        _processedNodes.push_back(this);


        NodeList parents;
        getParentNodes(parents);

        if (parents.size())
        {
            for (u32 i = 0; i < parents.size(); i++)
            {
                parents[i]->buildNodeList(_processedNodes, _segments, NULL);
            }
        }
        else
        {
            ProcessedNodeList processedNodes;
            buildChildrenList(processedNodes, _segments, _prev);
        }
    }

    bbool Ray_BossMorayNodeComponent::buildChildrenList( ProcessedNodeList& _processedNodes, DebugSegmentList& _segments, const Node* _prev ) const
    {
        const bbool alreadyProcessed = ( _processedNodes.find(this) != -1 );
        if (!alreadyProcessed)
        {
            _processedNodes.push_back(this);
        }


        NodeDataList children;
        getChildNodes(children);

        for (u32 i = 0; i < children.size(); i++)
        {
            const NodeData& child = children[i];

            DebugSegment seg;
            seg.m_n0 = _prev;
            seg.m_n1 = this;
            seg.m_n2 = child.m_node;
            seg.m_linkData = child.m_linkData;

            if (!alreadyProcessed)
            {
                const bbool hasChildren = child.m_node->buildChildrenList(_processedNodes, _segments, this);
                seg.m_isEnd = !hasChildren;
            }

            if (_segments.full())
            {
                ITF_WARNING_CATEGORY(LD, m_actor, 0, "Draw limit reached! Seriously, your network has more than 1024 segments... YES I'M TALKING TO YOU PIERRE-SIMON!!!");
                return btrue;
            }

            _segments.push_back(seg);
        }

        return children.size() > 0;
    }


    void Ray_BossMorayNodeComponent::drawSegment(
        const Ray_BossMorayNodeComponent* n0,   // prev; can be null
        const Ray_BossMorayNodeComponent* n1,   // cur: never null
        const Ray_BossMorayNodeComponent* n2,   // next: never null
        const ChildEntry& _linkData,
        const bbool _isEnd,
        const bbool _drawCorridor,
        const bbool _drawCircle ) const
    {
        Color colorStart = Color::green();
        Color colorEnd = Color::magenta();
        Color colorStraight = Color::white();
        Color colorCurve = Color::yellow();
        if (_linkData.hasTag(s_Disabled))
        {
            colorStart = Color::grey();
            colorEnd = Color::grey();
            colorStraight = Color::grey();
            colorCurve = Color::grey();
        }
        const Color colorError = Color::black();

        const f32 debugCorridorWidth = getTemplate()->getDebugCorridorWidth();


        const Segment segment(n0 ? n0->GetActor() : NULL, n1->GetActor(), n2->GetActor());

        if (segment.p23Length < 0)
        {
            // error: overlap
            colorStraight = colorCurve = colorError;
        }

        if (n0)
        {
            // n0-to-n2 curve
            static const f32 stepCount = 10;
            static const f32 timeStep = 1.f / stepCount;

            Vec3d prevPos = segment.p0;
            Vec3d prevNormal = (segment.p1 - segment.p0).normalize().getPerpendicular();

            for ( f32 i = 1; i <= stepCount; i++ )
            {
                Vec3d pos;
                segment.getCurvePosAtT(i * timeStep, pos);

                DebugDraw::line3D(prevPos, pos, colorCurve, s_lineWidth);

                if (_drawCorridor)
                {
                    Vec3d normal = (i < stepCount) ?
                        (pos - prevPos).normalize().getPerpendicular() :
                    (segment.p2 - segment.p1).normalize().getPerpendicular();
                    DebugDraw::line3D(prevPos + prevNormal * debugCorridorWidth, pos + normal * debugCorridorWidth, colorCurve);
                    DebugDraw::line3D(prevPos - prevNormal * debugCorridorWidth, pos - normal * debugCorridorWidth, colorCurve);
                    prevNormal = normal;
                }

                prevPos = pos;
            }
        }
        else
        {
            // n1-to-n2 start line
            DebugDraw::line3D(segment.p1, segment.p2, colorStart, s_lineWidth);

            if (_drawCorridor)
            {
                Vec3d normal = (segment.p2 - segment.p1).normalize().getPerpendicular();
                DebugDraw::line3D(segment.p1 + normal * debugCorridorWidth, segment.p2 + normal * debugCorridorWidth, colorStart);
                DebugDraw::line3D(segment.p1 - normal * debugCorridorWidth, segment.p2 - normal * debugCorridorWidth, colorStart);
            }
        }

        {
            // n1-to-n2 arrow
            DebugDraw::arrow3D(segment.p2, segment.p3, colorStraight);

            if (_drawCorridor)
            {
                Vec3d normal = (segment.p3 - segment.p2).normalize().getPerpendicular();
                DebugDraw::line3D(segment.p2 + normal * debugCorridorWidth, segment.p3 + normal * debugCorridorWidth, colorStraight);
                DebugDraw::line3D(segment.p2 - normal * debugCorridorWidth, segment.p3 - normal * debugCorridorWidth, colorStraight);
            }
        }

        if (_drawCircle)
        {
            // n1 circle
            DebugDraw::circle(segment.p1, n1->GetActor()->getScale().m_x);
        }


        if (_isEnd)
        {
            // n1-to-n2 end line
            const Vec3d p4 = n2->GetActor()->getPos();
            DebugDraw::line3D(segment.p3, p4, colorEnd, s_lineWidth);

            // n2 circle
            DebugDraw::circle(p4, n2->GetActor()->getScale().m_x);
        }
    }
#endif // ITF_SUPPORT_EDITOR

    Ray_BossMorayNodeComponent::Segment::Segment(
        const Actor* _prev,
        const Actor* _current,
        const Actor* _next )
        : hasPrev(_prev != NULL)
        , p12Length(0.f)
        , p23Length(0.f)
    {
        ITF_ASSERT_CRASH(_current, "current point is mandatory!");
        ITF_ASSERT_CRASH(_next, "next point is mandatory!");


        const Vec3d& n1Pos = _current->getPos();
        const f32 n1Scale = _current->getScale().m_x;

        const Vec3d& n2Pos = _next->getPos();
        const f32 n2Scale = _next->getScale().m_x;

        const Vec3d v12 = (n2Pos - n1Pos).truncateTo2D().to3d();
        const f32 len12 = v12.norm();
        ITF_ASSERT_MSG(len12 != 0, "points shouldn't be at the same position");
        dir12 = len12 ? v12 / len12 : Vec3d::Zero;

        p1 = n1Pos;
        p2 = n1Pos + dir12 * n1Scale;
        p3 = n2Pos - dir12 * n2Scale;

        p12Length = n1Scale;
        p23Length = len12 - n1Scale - n2Scale;

        if (hasPrev)
        {
            const Vec3d& n0Pos = _prev->getPos();
            Vec3d dir01 = (n1Pos - n0Pos).truncateTo2D().to3d().normalize();

            p0 = n1Pos - dir01 * n1Scale;
        }
    }

    void Ray_BossMorayNodeComponent::Segment::getCurvePosAtT( const f32 _t, Vec3d& _pos ) const
    {
        if (hasPrev)
        {
            // p0 to p2 part
            const f32 tInv = 1.f - _t;
            _pos = ( p0 * tInv * tInv ) 
                 + ( p1 * 2.f * _t * tInv ) 
                 + ( p2 * _t * _t );
        }
        else
        {
            // p1 to p2
            _pos = p1 + _t * dir12 * p12Length;
        }
    }

    // If the position is on the Segment: affect _pos and return false.
    // Otherwise: remove the Segment length from _dist (don't affect _pos), and return true.
    bbool Ray_BossMorayNodeComponent::Segment::getPosAtDist( f32& _dist, Vec3d& _pos ) const
    {
        static const f32 s_sampleCount = 1000;
        static const f32 s_sampleTime = 1.f / s_sampleCount;

        if (hasPrev)
        {
            // p0 to p2 part

            Vec3d prevPos = p0;
            f32 curveLength = 0.f;

            for ( f32 i = 1; i <= s_sampleCount; i++ )
            {
                const f32 t = i * s_sampleTime;
                const f32 tInv = 1.f - t;
                ITF_ASSERT(tInv <= 1.f);

                const Vec3d pos =
                      ( p0 * tInv * tInv ) 
                    + ( p1 * 2.f * t * tInv ) 
                    + ( p2 * t * t );

                const f32 sampleLength = (pos - prevPos).norm();

                if ( _dist <= (curveLength + sampleLength) )
                {
                    const f32 sampleRatio = (_dist - curveLength) / sampleLength;
                    const f32 posT = (i - 1 + sampleRatio) * s_sampleTime;
                    getCurvePosAtT(posT, _pos);
                    return bfalse;
                }

                curveLength += sampleLength;
                prevPos = pos;
            }


            // if we got here, it means that dist is longer than the curve length


            // p2 to p3 part
            if ( (_dist - curveLength) < p23Length )
            {
                _pos = p2 + dir12 * (_dist - curveLength);
                return bfalse;
            }


            // _dist is longer than this Segment
            _dist -= (curveLength + p23Length);
            return btrue;
        }
        else
        {
            // p1 to p3 part

            if ( _dist < (p12Length + p23Length) )
            {
                _pos = p1 + dir12 * _dist;
                return bfalse;
            }


            // _dist is longer than this Segment
            _dist -= (p12Length + p23Length);
            return btrue;
        }
    }


    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BossMorayNodeComponent_Template)
    BEGIN_SERIALIZATION(Ray_BossMorayNodeComponent_Template)
        SERIALIZE_OBJECT("triggerEvent", m_eventNodeReached);
        SERIALIZE_MEMBER("debugCorridorWidth", m_debugCorridorWidth);
    END_SERIALIZATION()

    Ray_BossMorayNodeComponent_Template::Ray_BossMorayNodeComponent_Template()
        : m_debugCorridorWidth(1.f)
    {
    }
};
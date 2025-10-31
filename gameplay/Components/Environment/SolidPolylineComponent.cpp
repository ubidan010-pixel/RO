#include "precompiled_gameplay.h"

#ifndef _ITF_SOLIDPOLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/SolidPolylineComponent.h"
#endif //_ITF_SOLIDPOLYLINECOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(SolidPolylineComponent)

    BEGIN_SERIALIZATION_CHILD(SolidPolylineComponent)
    END_SERIALIZATION()

    SolidPolylineComponent::SolidPolylineComponent()
        : Super()
        , m_stimPoints(4)
    {
        m_stimPoints.resize(4);
    }

    SolidPolylineComponent::~SolidPolylineComponent()
    {
    }

    void SolidPolylineComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_solidEdges.resize(getTemplate()->getSolidEdges().size());
    }

    void SolidPolylineComponent::readModel()
    {
        Super::readModel();

        if (m_ready)
        {
            SolidEdgeList::iterator it = m_solidEdges.begin();
            SolidPolylineComponent_Template::SolidEdgeList::const_iterator itTemplate = getTemplate()->getSolidEdges().begin();

            for ( ; it != m_solidEdges.end() && itTemplate != getTemplate()->getSolidEdges().end(); ++it, ++itTemplate )
            {
                SolidEdgeData& data = *it;
                const SolidPolylineComponent_Template::SolidEdgeData& dataTemplate = *itTemplate;

                data.m_boneIndex = m_animComponent->getBoneIndex(dataTemplate.m_boneName);
                ITF_ASSERT(data.m_boneIndex != U32_INVALID);
                if (data.m_boneIndex != U32_INVALID)
                {
                    AnimBoneDyn* bone = m_animComponent->getBone(data.m_boneIndex);
                    data.m_prevPos = bone->m_Pos;
                    data.m_prevPosEnd = bone->m_PosEnd;
                }
            }
        }
    }

    void SolidPolylineComponent::processAnimUpdated( class EventAnimUpdated* _animUpdated )
    {
        Super::processAnimUpdated(_animUpdated);

        SolidEdgeList::iterator it = m_solidEdges.begin();
        SolidPolylineComponent_Template::SolidEdgeList::const_iterator itTemplate = getTemplate()->getSolidEdges().begin();

        for ( ; it != m_solidEdges.end() && itTemplate != getTemplate()->getSolidEdges().end(); ++it, ++itTemplate )
        {
            SolidEdgeData& data = *it;
            const SolidPolylineComponent_Template::SolidEdgeData& dataTemplate = *itTemplate;

            if (data.m_boneIndex == U32_INVALID)
                continue;

            AnimBoneDyn* bone = m_animComponent->getBone(data.m_boneIndex);
            Vec2d pos = bone->m_Pos;
            Vec2d posEnd = bone->m_PosEnd;

            // determine the direction of the movement
            Vec2d movement = pos - data.m_prevPos;
            Vec2d normal = (posEnd - pos).getPerpendicular();
            bbool forward = (movement.dot(normal) > 0);

            //GFX_ADAPTER->drawDBGCircle(pos, 0.1f, forward ? Color::blue() : Color::red());
            //GFX_ADAPTER->drawDBGLine(pos, posEnd, forward ? Color::blue() : Color::red());
            //GFX_ADAPTER->drawDBGLine(pos, data.m_prevPos, Color::green());

            f32 delta = movement.norm() * (forward ? 1 : -1);
            if (delta > dataTemplate.m_minDelta)
            {
                // we've moving fast: send a stim

                // make sure the points are clockwise
                if (forward)
                {
                    m_stimPoints[0] = data.m_prevPos;
                    m_stimPoints[1] = data.m_prevPosEnd;
                    m_stimPoints[2] = posEnd;
                    m_stimPoints[3] = pos;
                }
                else
                {
                    m_stimPoints[0] = pos;
                    m_stimPoints[1] = posEnd;
                    m_stimPoints[2] = data.m_prevPosEnd;
                    m_stimPoints[3] = data.m_prevPos;
                }

#ifndef ITF_FINAL
                //DebugDraw::pointList(Vec2d::Zero, 0, m_stimPoints, forward ? Color::white() : Color::black(), 1);
#endif // ITF_FINAL

                m_stimShape.setPoints(m_stimPoints);
                SolidStim* stim = STIMSMANAGER->requestStim<SolidStim>(&m_stimShape);

#if defined(ITF_FINAL)
                if(stim)
#endif // ITF_FINAL
                {
                    stim->setPrevPos(Vec2d::Zero);
                    stim->setPos(Vec2d::Zero);
                    stim->setAngle(0);
                    stim->setDepth(m_actor->getDepth());
                    stim->setSender(m_actor->getRef());
                    stim->setDirection(movement.normalize());

                    STIMSMANAGER->sendStim(stim);
                }
            }

            data.m_prevPos = pos;
            data.m_prevPosEnd = posEnd;
        }
    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(SolidPolylineComponent_Template)
    BEGIN_SERIALIZATION_CHILD(SolidPolylineComponent_Template)

        SERIALIZE_CONTAINER_OBJECT("solidEdges", m_solidEdges);

    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(SolidPolylineComponent_Template, SolidEdgeData)
        SERIALIZE_MEMBER("bone", m_boneName);
        SERIALIZE_MEMBER("minDelta", m_minDelta);
    END_SERIALIZATION()
};
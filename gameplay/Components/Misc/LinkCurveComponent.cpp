#include "precompiled_gameplay.h"

#ifndef _ITF_LINKCURVECOMPONENT_H_
#include "gameplay/Components/Misc/LinkCurveComponent.h"
#endif //_ITF_LINKCURVECOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(LinkCurveComponent)

    BEGIN_SERIALIZATION_CHILD(LinkCurveComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(LinkCurveComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "LinkCurveComponent requires a LinkComponent");
        VALIDATE_COMPONENT_PARAM("childOrientationInfluence", 0 <= getTemplate()->getChildOrientationInfluence() && getTemplate()->getChildOrientationInfluence() <= 1, "should in the [0,1] range");
    END_VALIDATE_COMPONENT()


    LinkCurveComponent::LinkCurveComponent()
        : m_linkComponent(NULL)
#ifdef ITF_WINDOWS
        , m_warningSent(bfalse)
#endif // ITF_WINDOWS
    {
    }

    LinkCurveComponent::~LinkCurveComponent()
    {
    }


    void LinkCurveComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();   // mandatory, validated
    }

    void LinkCurveComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        AIUtils::growAABB(m_linkComponent);
    }

    void LinkCurveComponent::Draw()
    {
        Super::Draw();

        if (!m_texture.getResource())
        {
            return;
        }


        #define MAXLINKS 32
        FixedArray<Actor*, MAXLINKS> children;
        FixedArray<ObjectRef, MAXLINKS> childrenRef;

        const LinkComponent::ChildrenList& links = m_linkComponent->getChildren();
        for (u32 i = 0; i < links.size(); i++)
        {
            Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, links[i].getPath());
            if ( !pickable )
            {
                continue;
            }

            Actor* child = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            if ( !child )
            {
                continue;
            }

            StringID valueExcludeTag; 
            if (links[i].getTagValue(ITF_GET_STRINGID_CRC(NoTree,3919588772), valueExcludeTag))
            {
                continue;
            }


#ifdef ITF_WINDOWS
            if (children.size() == MAXLINKS)
            {
                if (!m_warningSent)
                {
                    m_warningSent = btrue;
                    ITF_ERROR("Too many links! Limited to %d at the moment, if you need more, call a GPP.", MAXLINKS);
                    ITF_WARNING(m_actor, 0, "Too many links! Limited to %d at the moment, if you need more, call a GPP.", MAXLINKS);
                }
                break;
            }
#endif // ITF_WINDOWS

            children.push_back(child);
            childrenRef.push_back(child->getRef());
        }



        // remove dead children
        for (ChildDataMap::iterator it = m_childDataMap.begin(); it != m_childDataMap.end();)
        {
            if ( childrenRef.find(it->first) == -1 )
            {
#ifdef ITF_CTR  //CASA::ToCheck erase function
                ChildDataMap::iterator it0=it;
                m_childDataMap.erase(it0);
                it++;
#else
                it = m_childDataMap.erase(it);
#endif
            }
            else
            {
                it++;
            }
        }


        const Vec2d parentPos = getParentAttachPosition();
        const Vec2d parentNormal = getParentAttachNormal();

        for (u32 i = 0; i < children.size(); i++)
        {
            Actor* child = children[i];
            ActorRef childRef = child->getRef();

            ChildDataMap::iterator it = m_childDataMap.find(childRef);
            if ( it == m_childDataMap.end() )
            {
                it = m_childDataMap.insert( std::pair<ActorRef, ChildData>(childRef, ChildData()) ).first;
            }

            ChildData& childData = it->second;
            if (childData.m_alpha)
            {
                const Vec2d childPos = getChildAttachPosition(child);
                const Vec2d childNormal = getChildAttachNormal(child, parentPos, childPos);
                drawLink( parentPos, parentNormal,
                          childPos, childNormal,
                          childData.m_patchCurve, childData.m_alpha);
            }
        }

#ifndef ITF_FINAL
        /*for (u32 i = 0; i < children.size(); i++)
        {
            Actor* child = children[i];

            const Vec2d& p0 = m_actor->get2DPos();
            const Vec2d& p3 = child->get2DPos();
            Vec2d n0 = Vec2d::Up.Rotate(m_actor->getAngle());
            Vec2d n1 = (p0 - p3).normalize();
            f32 lengthBy2 = (p3 - p0).norm() * 0.5f;
            Vec2d p1 = p0 + n0 * lengthBy2;
            Vec2d p2 = p3 + n1 * lengthBy2;

            f32 z = m_actor->getDepth();
            DebugDraw::arrow2D(p0, p1, z);
            DebugDraw::arrow2D(p3, p2, z);


            static const f32 stepCount = 10;
            static const f32 timeStep = 1.f / stepCount;

            Vec2d prevPos = p0;
            for ( f32 i = 1; i <= stepCount; i++ )
            {
                Vec2d pos = getBezierPosition(p0, p1, p2, p3, i * timeStep);
                DebugDraw::line2D(prevPos, pos, z, Color::red());
                prevPos = pos;
            }
        }*/
#endif // ITF_FINAL
    }


    f32 LinkCurveComponent::getAlpha( Actor* _child )
    {
        ChildDataMap::iterator it = m_childDataMap.find(_child->getRef());
        if ( it == m_childDataMap.end() )
        {
            //ITF_ASSERT_MSG(0, "not a registered child!");
            return 0.f;
        }

        return it->second.m_alpha;
    }

    void LinkCurveComponent::setAlpha( Actor* _child, f32 _value )
    {
        ChildDataMap::iterator it = m_childDataMap.find(_child->getRef());
        if ( it == m_childDataMap.end() )
        {
            //ITF_ASSERT_MSG(0, "not a registered child!");
            return;
        }

        it->second.m_alpha = _value;
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(LinkCurveComponent_Template)
    BEGIN_SERIALIZATION_CHILD(LinkCurveComponent_Template)
    END_SERIALIZATION()

    LinkCurveComponent_Template::LinkCurveComponent_Template()
    {
    }

};

#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOSSPLANTNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossPlantNodeComponent.h"
#endif //_ITF_RAY_BOSSPLANTNODECOMPONENT_H_

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
    IMPLEMENT_OBJECT_RTTI(Ray_BossPlantNodeComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BossPlantNodeComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BossPlantNodeComponent)
        //VALIDATE_COMPONENT_PARAM("anim", m_anim.isValid(), "please set an anim");
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "Ray_BossPlantNodeComponent requires a LinkComponent");
    END_VALIDATE_COMPONENT()


    Ray_BossPlantNodeComponent::Ray_BossPlantNodeComponent()
        : m_linkComponent(NULL)
    {
    }

    Ray_BossPlantNodeComponent::~Ray_BossPlantNodeComponent()
    {
    }

    void Ray_BossPlantNodeComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();   // mandatory, validated
    }

    Ray_BossPlantNodeComponent* Ray_BossPlantNodeComponent::getNextNode() const
    {
        AIUtils::LinkIterator iterator(m_linkComponent, btrue);
        while ( Actor* child = iterator.getNextRelativeActor() )
        {
            if ( Ray_BossPlantNodeComponent* node = child->GetComponent<Ray_BossPlantNodeComponent>() )
            {
                return node;
            }
        }

        return NULL;
    }

    Ray_BossPlantNodeComponent* Ray_BossPlantNodeComponent::getPrevNode() const
    {
        if ( ObjectRefList* parents = LINKMANAGER->getParents(m_actor->getRef()) )
        {
            for (u32 i = 0; i < parents->size(); i++)
            {
                if ( BaseObject* parentObj = (*parents)[i].getObject() )
                {
                    if( Actor* parent = parentObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                    {
                        if ( Ray_BossPlantNodeComponent* node = parent->GetComponent<Ray_BossPlantNodeComponent>() )
                        {
                            return node;
                        }
                    }
                }
            }
        }

        return NULL;
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    bbool Ray_BossPlantNodeComponent::s_drawDebug(bfalse);
#endif // ITF_SUPPORT_DEBUGFEATURE

    void Ray_BossPlantNodeComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

#ifdef ITF_SUPPORT_DEBUGFEATURE
        if (s_drawDebug)
        {
            // NOTE: this method fails if the root is not updated.
            //if ( !getPrevNode() )
            //{
            //    drawDebug();
            //}

            // NOTE: this will cause one complete draw per node... never mind.
            drawDebugRoot();
        }
#endif // ITF_SUPPORT_DEBUGFEATURE
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_BossPlantNodeComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        drawDebugRoot();
    }

    void Ray_BossPlantNodeComponent::onEditorCreated( Actor* _original )
    {
        Super::onEditorCreated(_original);

        if (_original)
        {
            m_linkComponent->clearLinks();
        }
    }
#endif // ITF_SUPPORT_EDITOR

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void Ray_BossPlantNodeComponent::drawDebugRoot() const
    {
        ProcessedNodeList parentNodes;

        const Ray_BossPlantNodeComponent* root = this;
        while ( const Ray_BossPlantNodeComponent* tmp = root->getPrevNode() )
        {
            if (parentNodes.find(tmp))  // TODO: quick fix to avoid infinite loops; we should handle multi-parent cases properly like for the BossMorayNodes
            {
                break;
            }
            parentNodes.push_back(tmp);

            root = tmp;
        }

        ProcessedNodeList processedNodes;
        root->drawDebug(processedNodes);
    }

    void Ray_BossPlantNodeComponent::drawDebug( ProcessedNodeList& _processedNodes ) const
    {
        if ( _processedNodes.find(this) != -1 )
        {
            return;
        }
        _processedNodes.push_back(this);


        bbool stationary = isStationary();
        const Vec2d& cycleVector = getTemplate()->getCycleVector();

        Ray_BossPlantNodeComponent* next = getNextNode();
        if (next)
        {
            if (!stationary)
            {
                Vec2d vecToNext = next->m_actor->get2DPos() - m_actor->get2DPos();
                bbool exact(btrue);
                f32 rawCycleCount = getRawCycleCount(vecToNext, &exact);

                if (rawCycleCount < 0)
                {
                    DebugDraw::arrow3D(m_actor->getPos(), next->m_actor->getPos(), Color::red());
                    drawEmptyCircles();
                }
                else
                {
                    u32 cycleCount = getCycleCount(rawCycleCount);
                    if (cycleCount == 0)
                    {
                        // non-stationary nodes play at least one cycle
                        cycleCount = 1;
                        exact = bfalse;
                    }

                    if (exact)
                    {
                        DebugDraw::arrow3D(m_actor->getPos(), next->m_actor->getPos(), Color::green());
                    }
                    else
                    {
                        Vec3d cyclesEnd = m_actor->getPos() + cycleVector.to3d() * (f32)cycleCount;
                        if (next->isStationary())
                        {
                            DebugDraw::arrow3D(m_actor->getPos(), cyclesEnd, Color::green());
                            DebugDraw::arrow3D(cyclesEnd, next->m_actor->getPos(), Color::white());
                        }
                        else
                        {
                            DebugDraw::arrow3D(m_actor->getPos(), cyclesEnd, Color::yellow());
                            DebugDraw::arrow3D(cyclesEnd, next->m_actor->getPos(), Color::orange());
                        }
                    }

                    for (f32 i = 1; i <= cycleCount; i++)
                    {
                        GFX_ADAPTER->drawDBGCircle(
                            m_actor->get2DPos() + i * cycleVector,
                            0.3f, Color::green());
                    }
                    GFX_ADAPTER->drawDBGCircle(
                        m_actor->get2DPos() + (f32)(cycleCount + 1) * cycleVector,
                        0.3f, Color::white());
                }
            }
            else
            {
                // stationary node
                DebugDraw::arrow3D(m_actor->getPos(), next->m_actor->getPos(), Color::white());
            }


            next->drawDebug(_processedNodes);
        }
        else if (!stationary)
        {
            // non-stationary nodes play at least one cycle
            DebugDraw::arrow3D(m_actor->getPos(), m_actor->getPos() + cycleVector.to3d(), Color::cyan());
            drawEmptyCircles();
        }
    }

    void Ray_BossPlantNodeComponent::drawEmptyCircles() const
    {
        for (f32 i = 1; i <= 5; i++)
        {
            GFX_ADAPTER->drawDBGCircle(
                m_actor->get2DPos() + i * getTemplate()->getCycleVector(),
                0.3f, Color::white());
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    static const f32 k_cycleTolerance = 0.03f;
    static const f32 k_otherAxisTolerance = 0.1f;

    f32 Ray_BossPlantNodeComponent::getRawCycleCount( const Vec2d& _movement, bbool* _exact /*= NULL*/ ) const
    {
        const Vec2d& cycleVector = getTemplate()->getCycleVector();
        f32 cycleCount(0);
        bbool exact(btrue);

        if (cycleVector.m_x != 0.f && cycleVector.m_y != 0.f)
        {
            f32 xCount = _movement.m_x / cycleVector.m_x;
            f32 yCount = _movement.m_y / cycleVector.m_y;
            cycleCount = Min(xCount, yCount);
            exact = ( f32_Abs(xCount - yCount) < k_cycleTolerance && f32_Modulo(xCount, 1.f) < k_cycleTolerance );
        }
        else if (cycleVector.m_x != 0.f)
        {
            f32 xCount = _movement.m_x / cycleVector.m_x;
            cycleCount = xCount;
            exact = ( f32_Modulo(xCount, 1.f) < k_cycleTolerance ) && ( f32_Abs(_movement.m_y) < k_otherAxisTolerance );
            //f32 rest = f32_Modulo(xCount, 1.f);
            //exact = rest < k_cycleTolerance;
            //exact &= f32_Abs(_movement.m_y) < k_otherAxisTolerance;
        }
        else if (cycleVector.m_y != 0.f)
        {
            f32 yCount = _movement.m_y / cycleVector.m_y;
            cycleCount = yCount;
            exact = ( f32_Modulo(yCount, 1.f) < k_cycleTolerance ) && ( f32_Abs(_movement.m_x) < k_otherAxisTolerance );
            //f32 rest = f32_Modulo(yCount, 1.f);
            //exact = rest < k_cycleTolerance;
            //exact &= f32_Abs(_movement.m_x) < k_otherAxisTolerance;
        }

        if (_exact)
        {
            *_exact = exact;
        }

        return cycleCount;
    }

    u32 Ray_BossPlantNodeComponent::getCycleCount( f32 _rawCycleCount ) const
    {
        _rawCycleCount = f32_Abs(_rawCycleCount);
        f32 rest = f32_Modulo(_rawCycleCount, 1.f);
        if (rest > 1.f - k_cycleTolerance)
        {
            return (u32)ceil(_rawCycleCount);
        }
        else
        {
            return (u32)floor(_rawCycleCount);
        }
    }

    void Ray_BossPlantNodeComponent::onNodeReached( Actor* _activator )
    {
        Ray_EventNodeReached eventNodeReached = getTemplate()->getEventNodeReached(); // copy from template
        eventNodeReached.setSender(m_actor->getRef());
        eventNodeReached.setActivator(_activator->getRef());
        eventNodeReached.setActivated(btrue);

        //m_actor->onEvent(&eventNodeReached);
        _activator->onEvent(&eventNodeReached);
        m_linkComponent->sendEventToChildren(&eventNodeReached);
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BossPlantNodeComponent_Template)
    BEGIN_SERIALIZATION(Ray_BossPlantNodeComponent_Template)
        SERIALIZE_MEMBER("anim", m_anim);
        SERIALIZE_MEMBER("animPlayRate", m_animPlayRate);
        SERIALIZE_MEMBER("cycleVector", m_cycleVector);
        SERIALIZE_OBJECT("triggerEvent", m_eventNodeReached);
    END_SERIALIZATION()

    Ray_BossPlantNodeComponent_Template::Ray_BossPlantNodeComponent_Template()
        : m_animPlayRate(1.f)
        , m_cycleVector(Vec2d::Zero)
    {
    }

};
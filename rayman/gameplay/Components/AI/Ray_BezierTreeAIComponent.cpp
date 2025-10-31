#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BEZIERTREEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BezierTreeAIComponent.h"
#endif //_ITF_RAY_BEZIERTREEAICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#include "engine/actors/components/SoundComponent.h"


/**********************************************
                AI STUFF
 *********************************************/

#ifndef _ITF_RAY_PLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_PlatformAIComponent.h"
#endif //_ITF_RAY_PLATFORMAICOMPONENT_H_

#ifndef _ITF_PADRUMBLEMANAGER_H_
#include "gameplay/managers/PadRumbleManager.h"
#endif //_ITF_PADRUMBLEMANAGER_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_


namespace ITF
{
#ifdef ITF_WII
    #define SAMPLE_COUNT 5
#else
    #define SAMPLE_COUNT 10
#endif // ITF_WII
    #define MIN_EDGE_LENGTH 0.1f

    IMPLEMENT_OBJECT_RTTI(Ray_BezierTreeAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_BezierTreeAIComponent_Template)
        SERIALIZE_MEMBER("lockFirstNode", m_lockFirstNode);
        SERIALIZE_MEMBER("lockLastNodeScale", m_lockLastNodeScale);
        SERIALIZE_MEMBER("attachBone", m_attachBone);
        SERIALIZE_MEMBER("gameMaterial", m_gameMaterial);
        SERIALIZE_POLYLINEMODE("polylineMode", m_polylineMode);
        SERIALIZE_MEMBER("polylineBeginLength", m_polylineBeginLength);
        SERIALIZE_MEMBER("polylineEndLength", m_polylineEndLength);
        SERIALIZE_MEMBER("polylineBeginWidth", m_polylineBeginWidth);
        SERIALIZE_MEMBER("polylineMidWidth", m_polylineMidWidth);
        SERIALIZE_MEMBER("polylineEndWidth", m_polylineEndWidth);
        SERIALIZE_MEMBER("polylineStartOffset", m_polylineStartOffset);
        SERIALIZE_MEMBER("polylineEndOffset", m_polylineEndOffset);
        SERIALIZE_MEMBER("polylineTessellationLength", m_polylineTessellationLength);
        SERIALIZE_OBJECT("bezierRenderer", m_bezierRenderer);
        SERIALIZE_MEMBER("mainSpriteIndex", m_mainSpriteIndex);
        SERIALIZE_MEMBER("startSpriteIndex", m_startSpriteIndex);
        SERIALIZE_MEMBER("startSpriteLength", m_startSpriteLength);
        SERIALIZE_MEMBER("endSpriteIndex", m_endSpriteIndex);
        SERIALIZE_MEMBER("endSpriteLength", m_endSpriteLength);
        SERIALIZE_MEMBER("uvStretch", m_uvStretch);
        SERIALIZE_MEMBER("uvAttachToHead", m_uvAttachToHead);
        SERIALIZE_MEMBER("uvScrollSpeed", m_uvScrollSpeed);
        SERIALIZE_OBJECT("tweenInterpreter", m_tweenInterpreter);
        SERIALIZE_MEMBER("headActor", m_headActor);
        SERIALIZE_MEMBER("headAttachOffset", m_headAttachOffset);
        SERIALIZE_CONTAINER_OBJECT("spawnables", m_spawnables);
        SERIALIZE_MEMBER("fx", m_fxName);
        SERIALIZE_OBJECT("ai", m_ai);
        SERIALIZE_MEMBER("polylineDisableOnTransition", m_polylineDisableOnTransition);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(Spawnable)
        SERIALIZE_MEMBER("name", m_name);
        SERIALIZE_MEMBER("actorPath", m_actorPath);
        SERIALIZE_MEMBER("useNodeScale", m_useNodeScale);
        SERIALIZE_MEMBER("posOffset", m_posOffset);
        SERIALIZE_MEMBER("angleOffset", m_angleOffset);
    END_SERIALIZATION()

    void Ray_BezierTreeAIComponent_Template::onTemplateLoaded()
    {
        Super::onTemplateLoaded();

        m_tweenInterpreter.onTemplateLoaded();

        ITF_WARNING_CATEGORY(LD, NULL, m_polylineTessellationLength, "polylineTessellationLength cannot be 0");
        m_polylineTessellationRatio = f32_Inv(m_polylineTessellationLength);
    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_BezierTreeAIComponent)

    BEGIN_SERIALIZATION_SUBCLASS(Ray_BezierTreeAIComponent, Node)
        SERIALIZE_MEMBER("pos", m_initialPos);
        SERIALIZE_MEMBER("angle", m_initialAngle);
        SERIALIZE_MEMBER("scale", m_initialScale);
        SERIALIZE_MEMBER("bezierTension", m_bezierTension);
        SERIALIZE_MEMBER("tweenSet", m_tweenSet);
        SERIALIZE_MEMBER("tweenOffset", m_tweenOffset);
        SERIALIZE_MEMBER("spawnableName", m_spawnableName);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_BezierTreeAIComponent, Branch)
        SERIALIZE_CONTAINER_OBJECT("nodes", m_nodes);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_CHILD(Ray_BezierTreeAIComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("startActivated", m_ai.m_startActivated);
            SERIALIZE_MEMBER("branchSpeed", m_ai.m_branchSpeed);
            SERIALIZE_POLYLINEMODE("polylineMode", m_polylineMode);
            SERIALIZE_MEMBER("flipTexture", m_flipTexture);
            SERIALIZE_CONTAINER_OBJECT("branches", m_branches);
            SERIALIZE_FUNCTION(postChangeProperties, ESerialize_PropertyEdit_Load);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BezierTreeAIComponent)
    END_VALIDATE_COMPONENT()

    Ray_BezierTreeAIComponent::Ray_BezierTreeAIComponent()
        : m_polylineMode(PolylineMode_Invalid)
        , m_polylineDisabled(bfalse)
        , m_spriteMode(SpriteMode_Main)
        , m_uvOffset(0.f)
        , m_flipTexture(bfalse)
        , m_animComponent(NULL)
        , m_attachBoneIndex(U32_INVALID)

        , m_fxController(NULL)
        , m_soundComponent(NULL)
        , m_speedInputIndex(U32_INVALID)
        , m_speedInputValue(0.f)
    {
    }

    Ray_BezierTreeAIComponent::~Ray_BezierTreeAIComponent()
    {
        clear();
    }

    void Ray_BezierTreeAIComponent::clear()
    {
        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            Branch& branch = m_branches[iB];
            branch.clear();
        }
    }

    void Ray_BezierTreeAIComponent::Branch::clear()
    {
        SF_DEL(m_polyline);

        for (u32 iN = 0; iN < m_nodes.size(); iN++)
        {
            Node& node = m_nodes[iN];
            node.clear();
        }

        if ( Actor* spawn = m_headActor.getActor() )
        {
            spawn->requestDestruction();
        }
        m_headActor.invalidate();


        // clear this so that, if we get reinit by the editor, draw() won't access crappy stuff
        m_initialBezierCurve.clear();
        m_bezierCurve.clear();
        m_oneOverTotalInitialLength = 0.f;
        m_oneOverTotalLength = 0.f;
        m_time = 1.f;
        m_currentLength = 0.f;
    }

    void Ray_BezierTreeAIComponent::Node::clear()
    {
        if ( Actor* spawn = m_spawn.getActor() )
        {
            spawn->requestDestruction();
        }
        m_spawn.invalidate();
        m_tplSpawnable = NULL;
    }

    void Ray_BezierTreeAIComponent::onBecomeActive()
    {
        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            Branch& branch = m_branches[iB];

            if(Actor* pHead = branch.m_headActor.getActor())
            {
                AIUtils::setAlwaysActive(pHead, btrue);
            }
        }
    }

    void Ray_BezierTreeAIComponent::onBecomeInactive()
    {
        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            Branch& branch = m_branches[iB];

            if (branch.m_polyline)
            {
                branch.m_polyline->deactivate();
            }

            if(Actor* pHead = branch.m_headActor.getActor())
            {
                AIUtils::setAlwaysActive(pHead, bfalse);
            }
        }
    }

    void Ray_BezierTreeAIComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        m_fxController = m_actor->GetComponent<FXControllerComponent>();
        m_soundComponent = m_actor->GetComponent<SoundComponent>();


        if (m_polylineMode == PolylineMode_Invalid)
        {
            m_polylineMode = getTemplate()->getPolylineMode();
        }

        if ( m_polylineMode != PolylineMode_Invalid &&
             m_polylineMode != PolylineMode_None )
        {
            m_actor->setObjectGroupIndex(ObjectGroup::GroupIndex_Collision);
        }

        if ( getTemplate()->getGameMaterial().getStringID() != GameMaterial_Template::nullMatName.getStringID() )
        {
            World::addGameMaterialFromFile(getTemplate()->getGameMaterial(), m_actor->getResourceGroup());
        }


        // init spawners
        if (!getTemplate()->getHeadActor().isEmpty())
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_headSpawner, getTemplate()->getHeadActor());
        }

        m_spawners.resize(getTemplate()->getSpawnables().size());
        for (u32 i = 0; i < getTemplate()->getSpawnables().size(); i++)
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_spawners[i], getTemplate()->getSpawnables()[i].m_actorPath);
        }


        // init renderer
        m_bezierRenderer.onLoaded( &getTemplate()->getBezierRenderer(), m_actor->getResourceGroup() );
        m_bezierRenderer.setColor( getColorFactor() );
        m_bezierRenderer.setFogColor( getColorFog() );


        m_ai.onActorLoaded(this);
    }

    void Ray_BezierTreeAIComponent::onResourceReady()
    {
        Super::onResourceReady();

        if (m_animComponent)
        {
            m_attachBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getAttachBone());
        }

        if ( Texture* texture = (Texture*)m_bezierRenderer.getTextureID().getResource() )
        {
            if ( const UVAtlas* atlas = texture->getUVAtlas() )
            {
                if ( getTemplate()->getMainSpriteIndex() < atlas->getNumberUV() &&
                     getTemplate()->getEndSpriteIndex() < atlas->getNumberUV() &&
                     getTemplate()->getEndSpriteLength() > 0 )
                {
                    m_spriteMode = SpriteMode_MainEnd;
                    m_mainSprite = atlas->getUVDataAt(getTemplate()->getMainSpriteIndex());
                    m_endSprite = atlas->getUVDataAt(getTemplate()->getEndSpriteIndex());

                    if (getTemplate()->getStartSpriteIndex() < atlas->getNumberUV() &&
                        getTemplate()->getStartSpriteLength() > 0 )
                    {
                        m_spriteMode = SpriteMode_StartMainEnd;
                        m_startSprite = atlas->getUVDataAt(getTemplate()->getStartSpriteIndex());
                    }
                }
            }
        }

        if (m_soundComponent)
        {
            if (m_speedInputIndex == U32_INVALID)
            {
                m_speedInputIndex = m_soundComponent->findInputIndex(ITF_GET_STRINGID_CRC(Speed,4218325830));
                if (m_speedInputIndex != U32_INVALID)
                {
                    m_soundComponent->setUpdateSoundInput(this);
                }
            }
        }

        init();
    }

    void Ray_BezierTreeAIComponent::postChangeProperties()
    {
        clear();
        init();
    }

    void Ray_BezierTreeAIComponent::init()
    {
        const Transform3d transform = getTransform();

        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            Branch& branch = m_branches[iB];
            branch.init(this, transform);
        }

        // branches need to be initialized before we init the AI (we need the total length)
        m_ai.initAI();

        if (m_ai.isOpen())
        {
            initAABB(transform);
        }
    }

    void Ray_BezierTreeAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        m_ai.onEvent(_event);

        //if ( PunchStim* _stim = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        //{
        //    processHit(_stim);
        //}
    }

    void Ray_BezierTreeAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        // no data yet, no need to update
        if (!m_branches.size() ||
            !m_branches[0].checkNodeCount(this))
        {
            return;
        }


        const Transform3d transform = getTransform();

        // update branches
        BezierCurve::Point attachPoint;
        getAttachPointWorld(transform, attachPoint);
        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            Branch& branch = m_branches[iB];
            branch.update(this, transform, _dt, attachPoint);
        }


        const f32 prevBranchTime = m_branches[0].getT();

        m_ai.update(_dt);

        updateFx(_dt, prevBranchTime);

        m_uvOffset += getTemplate()->getUvScrollSpeed() * _dt;



        // update AABB

        const Node& firstNode = m_branches[0].m_nodes[0];
        AABB aabb(firstNode.getPos());

        const f32 maxWidth = Max(getTemplate()->getBezierRenderer().getMidWidth(), getTemplate()->getPolylineMidWidth());
        const f32 extrusion = maxWidth * transform.getFlipSign() * 0.5f;

        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            Branch& branch = m_branches[iB];
            branch.updateAABB(this, extrusion, aabb);
        }

        m_actor->growRelativeAABBFromAbsolute(aabb);
        //DebugDraw::AABB(Vec2d::Zero, m_actor->getDepth(), 0, aabb);
    }

    void Ray_BezierTreeAIComponent::initAABB( const Transform3d& _transform )
    {
        if (!m_branches.size() ||
            !m_branches[0].checkNodeCount(this))
        {
            // no data
            return;
        }


        const Node& firstNode = m_branches[0].m_nodes[0];
        AABB localAABB(firstNode.getInitialTransform().getPos());

        const f32 maxWidth = Max(getTemplate()->getBezierRenderer().getMidWidth(), getTemplate()->getPolylineMidWidth());
        const f32 extrusion = maxWidth * _transform.getFlipSign() * 0.5f;

        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            Branch& branch = m_branches[iB];
            branch.initAABB(this, extrusion, localAABB);
        }

        // AABB was made using the initial curve: transform it
        AABB aabb( _transform.transformPos(localAABB.getMin()) );
        aabb.grow( _transform.transformPos(localAABB.getMinXMaxY()) );
        aabb.grow( _transform.transformPos(localAABB.getMaxXMinY()) );
        aabb.grow( _transform.transformPos(localAABB.getMax()) );

        m_actor->growRelativeAABBFromAbsolute(aabb);
        m_actor->updateAbsoluteAABBFromRelative();
        //DebugDraw::AABB(Vec2d::Zero, m_actor->getDepth(), 0, aabb, Color::white(), 10, 5);
    }

    #define AABB_SAMPLE_T 0.34f     // we will sample 0, 0.34, 0.68, but not 1 (next edge)

    void Ray_BezierTreeAIComponent::Branch::initAABB( Ray_BezierTreeAIComponent* _owner, const f32 _extrusion, AABB& _aabb )
    {
        // TODO: we should extrapolate (in case of polyline offset)

        const BezierCurve::EdgeList& edges = m_initialBezierCurve.getEdges();
        for (u32 i = 0; i < edges.size(); i++)
        {
            const BezierCurve::Edge& edge = edges[i];

            // better to be over-generous: we take the biggest scale
            const f32 scale = Max(m_scales[i], m_scales[i + 1]);

            for (f32 t = 0.f; t < 1.f; t += AABB_SAMPLE_T)
            {
                const Vec2d pos = edge.m_bezier.getPos(t).truncateTo2D();
                const Vec2d tan = edge.m_bezier.getTangent(t).truncateTo2D();
                const Vec2d normal = tan.getPerpendicular().normalize() * _extrusion * scale;

                _aabb.grow(pos + normal);
                _aabb.grow(pos - normal);
            }
        }

        // add the tip
        {
            const BezierCurve::PointList& points = m_initialBezierCurve.getPoints();
            const BezierCurve::Point& tip = points[points.size()-1];

            const f32 scale = m_scales[m_scales.size()-1];
            const Vec2d pos = tip.m_pos.truncateTo2D();
            const Vec2d normal = tip.m_tanA.truncateTo2D().getPerpendicular().normalize() * _extrusion * scale;

            _aabb.grow(pos + normal);
            _aabb.grow(pos - normal);
        }
    }

    void Ray_BezierTreeAIComponent::Branch::updateAABB( Ray_BezierTreeAIComponent* _owner, const f32 _extrusion, AABB& _aabb )
    {
        // TODO: we should extrapolate (in case of polyline offset)

        const BezierCurve::EdgeList& edges = m_bezierCurve.getEdges();
        for (u32 i = 0; i < edges.size(); i++)
        {
            const BezierCurve::Edge& edge = edges[i];

            // better to be over-generous: we take the biggest scale,
            // and process the whole edge even if we should cut in-between

            const f32 scale = Max(m_scales[i], m_scales[i + 1]);    // TODO: scale should be maintained from the tip

            for (f32 t = 0.f; t < 1.f; t += AABB_SAMPLE_T)
            {
                const Vec2d pos = edge.m_bezier.getPos(t).truncateTo2D();
                const Vec2d tan = edge.m_bezier.getTangent(t).truncateTo2D();
                const Vec2d normal = tan.getPerpendicular().normalize() * _extrusion * scale;

                _aabb.grow(pos + normal);
                _aabb.grow(pos - normal);

                //DebugDraw::line2D(pos - normal, pos + normal, _owner->GetActor()->getDepth());
                //DebugDraw::circle(pos - normal, _owner->GetActor()->getDepth(), 0.1f);
                //DebugDraw::circle(pos + normal, _owner->GetActor()->getDepth(), 0.1f);
            }

            //// TODO: add spawns
            //const Node& node = m_nodes[i];
            //if ( Actor* spawn = m_nodes[i].m_spawn.getActor() )
            //{
            //    _aabb.grow(spawn->getAABB());
            //}

            if (edge.m_edgeStart + edge.m_length >= m_currentLength - MTH_EPSILON)
            {
                // add the tip
                const Vec2d pos = edge.m_bezier.getPos(1.f).truncateTo2D();
                const Vec2d tan = edge.m_bezier.getTangent(1.f).truncateTo2D();
                const Vec2d normal = tan.getPerpendicular().normalize() * _extrusion * scale;

                _aabb.grow(pos + normal);
                _aabb.grow(pos - normal);

                //DebugDraw::line2D(pos - normal, pos + normal, _owner->GetActor()->getDepth());
                //DebugDraw::circle(pos - normal, _owner->GetActor()->getDepth(), 0.1f);
                //DebugDraw::circle(pos + normal, _owner->GetActor()->getDepth(), 0.1f);

                break;
            }
        }

        if ( Actor* head = m_headActor.getActor() )
        {
            _aabb.grow(head->getAABB());
        }
    }

    void Ray_BezierTreeAIComponent::Draw()
    {
        Super::Draw();

        Texture* texture = (Texture*)m_bezierRenderer.getTextureID().getResource();
        const bbool flipUV = m_actor->isFlipped() ^ m_flipTexture;

        switch (m_spriteMode)
        {
        case SpriteMode_Main:
            for (u32 iB = 0; iB < m_branches.size(); iB++)
            {
                m_branches[iB].draw(this, texture, flipUV);
            }
            break;

        case SpriteMode_MainEnd:
            for (u32 iB = 0; iB < m_branches.size(); iB++)
            {
                m_branches[iB].draw_end(this, texture, flipUV);
            }
            break;

        case SpriteMode_StartMainEnd:
            for (u32 iB = 0; iB < m_branches.size(); iB++)
            {
                m_branches[iB].draw_startEnd(this, texture, flipUV);
            }
            break;
        }
    }

    void Ray_BezierTreeAIComponent::processHit( PunchStim* _stim )
    {
        if (!_stim->getContacts().size())
        {
            return;
        }

        Actor* owner = IRTTIObject::SafeDynamicCast<Actor>(_stim->getContacts()[0].m_collidableUserData.getObject(),ITF_GET_STRINGID_CRC(Actor,2546623115));
        ITF_ASSERT_MSG(owner, "received stim from unknown owner");
        if (!owner)
        {
            return;
        }

        if (owner == m_actor)
        {
            return;
        }

        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            Branch& branch = m_branches[iB];
            if (branch.m_headActor == owner->getRef())
            {
                branch.close();
                break;
            }
        }
    }


    bbool Ray_BezierTreeAIComponent::Branch::checkNodeCount( Ray_BezierTreeAIComponent* _owner ) const
    {
        const u32 minNodeCount = _owner->getTemplate()->getLockFirstNode() ? 1 : 2;
        ITF_ASSERT(m_nodes.size() >= minNodeCount);
        return (m_nodes.size() >= minNodeCount);
    }

    void Ray_BezierTreeAIComponent::Branch::init( Ray_BezierTreeAIComponent* _owner, const Transform3d& _transform )
    {
        if (!checkNodeCount(_owner))
        {
            return;
        }


        // if last node scale is locked, ensure it's 1 (~ postLoadProperties)
        if (_owner->getTemplate()->getLockLastNodeScale())
        {
            m_nodes[m_nodes.size()-1].setInitialScale(1.f);
        }


        // init scales array
        if (_owner->getTemplate()->getLockFirstNode())
        {
            m_scales.resize(m_nodes.size() + 1);
            m_scales[0] = 1.f;
            for (u32 i = 0; i < m_nodes.size(); i++)
            {
                m_scales[i+1] = m_nodes[i].getInitialScale();
            }
        }
        else
        {
            m_scales.resize(m_nodes.size());
            for (u32 i = 0; i < m_nodes.size(); i++)
            {
                m_scales[i] = m_nodes[i].getInitialScale();
            }
        }

        m_useSharpTip = m_nodes[m_nodes.size()-1].getInitialScale() < 0.2f;



        // init nodes
        Scene* scene = _owner->GetActor()->getScene();
        for (u32 i = 0; i < m_nodes.size(); i++)
        {
            Node& node = m_nodes[i];
            node.initTransform();
            node.initTween(_owner);
            node.initSpawn(_owner, _transform, scene);
        }
        // spawn head   -- TODO: make it a normal node spawn?
        initSpawn(_owner, _transform, scene);


        // init bezier curve    --we need the node transforms to be set: done in node.initTransform()
        m_initialBezierCurve.resize(m_scales.size());   // using m_scales.size() because +1 in case of lockFirstNode
        m_bezierCurve.resize(m_scales.size());

        BezierCurve::Point attachPoint;
        _owner->getAttachPointLocal(_transform, attachPoint);
        buildCurve(_owner, m_initialBezierCurve, attachPoint);
        m_initialBezierCurve.buildEdges(SAMPLE_COUNT);

        m_oneOverTotalInitialLength = f32_Inv(m_initialBezierCurve.getTotalLength());


        // init polyline
        if (_owner->m_polylineMode != PolylineMode_None)
        {
            if (!m_polyline)
            {
                m_polyline = newAlloc(mId_Gameplay, ProceduralPolyline);
                m_polyline->init(_owner->GetActor(), _owner);
            }

            const f32 initialPolylineLength = m_initialBezierCurve.getTotalLength() - _owner->getTemplate()->getPolylineStartOffset() - _owner->getTemplate()->getPolylineEndOffset();
            m_polyEdgeCount = (u32)(initialPolylineLength * _owner->getTemplate()->getPolylineTessellationRatio());
            m_polyEdgeCount = Max((u32)1, m_polyEdgeCount);

            PolyLine& poly = m_polyline->getPolyline();
            poly.resize(getPolylinePointCount(_owner));
            poly.setGameMaterial(_owner->getTemplate()->getGameMaterial().getStringID());
        }


        // init patches
        m_patches.resize(m_initialBezierCurve.getNumEdges());
        if (_owner->useStartSprite())
        {
            m_startPatches.resize(m_initialBezierCurve.getNumEdges());
        }
        if (_owner->useEndSprite())
        {
            m_endPatches.resize(m_initialBezierCurve.getNumEdges());
        }
    }

    void Ray_BezierTreeAIComponent::Node::initTransform()
    {
        m_initialTransform = Transform3d(m_initialPos, m_initialAngle);
        // these values are local, only used once to build m_initialCurve
        m_pos = m_initialPos;
        m_angle = m_initialAngle;
        m_tangent = Vec3d(m_bezierTension, 0, 0);
        Vec3d_Rotate(&m_tangent, m_angle);
        m_scale = m_initialScale;
    }

    void Ray_BezierTreeAIComponent::Node::initTween( Ray_BezierTreeAIComponent* _owner )
    {
        if (m_tweenSet.isValid())
        {
            const TweenInterpreter_Template& tplInterpreter = _owner->getTemplate()->getTweenInterpreter();
            u32 setIndex = tplInterpreter.getSetIndex(m_tweenSet);
            if (setIndex != U32_INVALID)
            {
                const TweenInstructionSet_Template& tplSet = tplInterpreter.getInstructionSets()[setIndex];
                m_tween.setInstructionSet(&tplSet);
                m_tween.setSyncOffset(m_tweenOffset);
            }
        }
    }

    void Ray_BezierTreeAIComponent::Node::initSpawn( Ray_BezierTreeAIComponent* _owner, const Transform3d& _transform, Scene* _scene )
    {
        if (!m_spawnableName.isValid())
        {
            return;
        }

        // quick editor reinit: don't respawn
        if ( m_spawn.getActor() )
        {
            return;
        }


        const SpawnableList& tplSpawnables = _owner->getTemplate()->getSpawnables();
        for (u32 i = 0; i < tplSpawnables.size(); i++)
        {
            const Spawnable& tplSpawnable = tplSpawnables[i];
            if (tplSpawnable.m_name != m_spawnableName)
            {
                continue;
            }

            SpawneeGenerator& spawner = _owner->m_spawners[i];
            if (!spawner.isInitialized())
            {
                break;
            }

            const Vec3d pos = _transform.transformPos(getInitialTransform().getPos());
            const f32 angle = _transform.transformAngle(getInitialTransform().getAngle() + MTH_PIBY2);
            if ( Actor* spawn = spawner.getSpawnee(_scene, pos, angle) )
            {
                m_tplSpawnable = &tplSpawnable;
                m_spawn = spawn->getRef();
            }

            break;
        }
    }

    void Ray_BezierTreeAIComponent::Branch::initSpawn( Ray_BezierTreeAIComponent* _owner, const Transform3d& _transform, Scene* _scene )
    {
        if (!_owner->m_headSpawner.isInitialized())
        {
            return;
        }

        // quick editor reinit: don't respawn
        if ( m_headActor.getActor() )
        {
            return;
        }


        Node& headNode = m_nodes[m_nodes.size()-1];
        const Vec3d pos = _transform.transformPos(headNode.getInitialTransform().getPos());
        const f32 angle = _transform.transformAngle(headNode.getInitialTransform().getAngle() + MTH_PIBY2);

        if ( Actor* spawn = _owner->m_headSpawner.getSpawnee(_scene, pos, angle) )
        {
            m_headActor = spawn->getRef();

            //spawn->registerEvent(PunchStim::GetClassNameStatic(), _owner);
        }
    }

    void Ray_BezierTreeAIComponent::Branch::update( Ray_BezierTreeAIComponent* _owner,
        const Transform3d& _transform, const f32 _dt,
        const BezierCurve::Point& _attachPoint )
    {
        if (!checkNodeCount(_owner))
        {
            return;
        }


        Actor* headActor = m_headActor.getActor();

        updateAI(_owner, _dt, headActor);


        // update nodes
        const u32 scaleCountOffset = _owner->getTemplate()->getLockFirstNode() ? 1 : 0;
        for (u32 i = 0; i < m_nodes.size(); i++)
        {
            Node& node = m_nodes[i];
            node.update(_transform, _dt);
            m_scales[i + scaleCountOffset] = node.getScale();   // scale can be tweened
        }


        // update bezier curve
        buildCurve(_owner, m_bezierCurve, _attachPoint);
        m_bezierCurve.buildEdges(SAMPLE_COUNT);
        m_oneOverTotalLength = f32_Inv(m_bezierCurve.getTotalLength());
        m_currentLength = m_time * m_bezierCurve.getTotalLength();


        updatePolyline(_owner, _dt);


        if (headActor)
        {
            updateHead(_owner, headActor);
        }
    }

    void Ray_BezierTreeAIComponent::Node::update( const Transform3d& _transform, const f32 _dt )
    {
        Vec3d localPos;
        const f32 localAngle = m_initialAngle;

        if (m_tween.isInitialized())
        {
            m_tween.update(_dt);
            localPos = m_initialTransform.transformPos( m_tween.getCurrentCoordinates().m_pos );
            m_scale = m_initialScale * m_tween.getCurrentCoordinates().m_scaleMultiplier.m_x;
        }
        else
        {
            localPos = m_initialPos;
        }

        m_pos = _transform.transformPos(localPos);
        m_angle = _transform.transformAngle(localAngle);

        Vec3d localTangent(m_bezierTension, 0, 0);
        Vec3d_Rotate(&localTangent, localAngle);
        m_tangent = _transform.transformDir(localTangent);


        if (Actor* spawn = m_spawn.getActor())
        {
            Vec3d offset = m_tplSpawnable->m_posOffset * m_scale;
            Vec3d_Rotate(&offset, m_angle);
            spawn->setPos(_transform.transformPos( localPos + offset ));

            spawn->setAngle(_transform.transformAngle( localAngle + m_tplSpawnable->m_angleOffset.ToRadians() ));

            if (m_tplSpawnable->m_useNodeScale)
            {
                spawn->setScale( spawn->getLocalInitialScale() * m_scale );
            }
        }
    }


    u32 Ray_BezierTreeAIComponent::Branch::getPolylinePointCount( Ray_BezierTreeAIComponent* _owner ) const
    {
        switch (_owner->m_polylineMode)
        {
        case PolylineMode_Left:
        case PolylineMode_Right:
            {
                return m_polyEdgeCount + 1;
            }
            break;

        case PolylineMode_DoubleSided:
            const bbool extrude = _owner->getTemplate()->getPolylineMidWidth() != 0;
            if (extrude)
            {
                if (useSharpTip())
                {
                    return (m_polyEdgeCount + 1) * 2;
                }
                else
                {
                    return (m_polyEdgeCount + 1) * 2 + 1;
                }
            }
            else
            {
                return m_polyEdgeCount * 2 + 1;
            }
            break;
        }

        ITF_ASSERT_MSG(0, "unsupported mode: %d", _owner->m_polylineMode);
        return 0;
    }

    void Ray_BezierTreeAIComponent::Branch::updatePolyline( Ray_BezierTreeAIComponent* _owner, const f32 _dt )
    {
        if ( !m_polyline )
        {
            ITF_ASSERT_MSG(_owner->m_polylineMode == PolylineMode_None, "if not in PolylineMode_None, we should have a polyline!");
            return;
        }


        const f32 polylineLength = m_currentLength - _owner->getTemplate()->getPolylineStartOffset() - _owner->getTemplate()->getPolylineEndOffset();

        if ( _owner->m_polylineDisabled ||
             polylineLength < (m_polyEdgeCount * MIN_EDGE_LENGTH) )
        {
            m_polyline->deactivate();
            return;
        }

        m_polyline->activate();


        const f32 edgeLength = polylineLength / m_polyEdgeCount;
        const bbool extrude = _owner->getTemplate()->getPolylineMidWidth() != 0;

        switch (_owner->m_polylineMode)
        {
        case PolylineMode_Left:
            if (extrude)
                updatePolyline_leftExtruded(_owner, edgeLength);
            else
                updatePolyline_left(_owner, edgeLength);
            break;

        case PolylineMode_Right:
            if (extrude)
                updatePolyline_rightExtruded(_owner, edgeLength);
            else
                updatePolyline_right(_owner, edgeLength);
            break;

        case PolylineMode_DoubleSided:
            if (extrude)
            {
                if (useSharpTip())
                {
                    updatePolyline_doubleSidedExtruded_sharpTip(_owner, edgeLength);
                }
                else
                {
                    updatePolyline_doubleSidedExtruded(_owner, edgeLength);
                }
            }
            else
            {
                updatePolyline_doubleSided(_owner, edgeLength);
            }
            break;

        default:
            ITF_ASSERT_MSG(0, "unsupported mode: %d", _owner->m_polylineMode);
            break;
        }


        // points are set, update the polyline
        PolyLine& poly = m_polyline->getPolyline();
        poly.setDepth(_owner->GetActor()->getDepth());
        poly.update(_dt);
    }

    void Ray_BezierTreeAIComponent::Branch::updatePolyline_doubleSided(
        Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength )
    {
        PolyLine& poly = m_polyline->getPolyline();
        const u32 pointCount = poly.getPosCount();
        const f32 startOffset = _owner->getTemplate()->getPolylineStartOffset();
        const f32 endOffset = _owner->getTemplate()->getPolylineEndOffset();

        u32 ie = 0;  // edge index
        u32 it = 0;  // sample index


        // base points
        const Vec2d basePos = getPosExtrapolated(ie, it, startOffset);
        poly.setPosAt(basePos, 0);               // first point
        poly.setPosAt(basePos, pointCount-1);    // last point: close

        for (u32 i = 1; i < m_polyEdgeCount; i++)
        {
            const Vec2d pos = getPosExtrapolated(ie, it, i * _edgeLength + startOffset);
            poly.setPosAt(pos, i);
            poly.setPosAt(pos, pointCount-1 - i);
        }

        // tip point
        const Vec2d tipPos = getPosExtrapolated(ie, it, m_currentLength - endOffset);
        poly.setPosAt(tipPos, m_polyEdgeCount);
    }

    void Ray_BezierTreeAIComponent::Branch::updatePolyline_left(
        Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength )
    {
        PolyLine& poly = m_polyline->getPolyline();
        const f32 startOffset = _owner->getTemplate()->getPolylineStartOffset();
        const f32 endOffset = _owner->getTemplate()->getPolylineEndOffset();

        u32 ie = 0;  // edge index
        u32 it = 0;  // sample index


        // base point
        poly.setPosAt(getPosExtrapolated(ie, it, startOffset), 0);

        for (u32 i = 1; i < m_polyEdgeCount; i++)
        {
            poly.setPosAt(getPosExtrapolated(ie, it, i * _edgeLength + startOffset) , i);
        }

        // tip point
        poly.setPosAt(getPosExtrapolated(ie, it, m_currentLength - endOffset), m_polyEdgeCount);
    }

    void Ray_BezierTreeAIComponent::Branch::updatePolyline_right(
        Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength )
    {
        PolyLine& poly = m_polyline->getPolyline();
        const f32 startOffset = _owner->getTemplate()->getPolylineStartOffset();
        const f32 endOffset = _owner->getTemplate()->getPolylineEndOffset();

        u32 ie = 0;  // edge index
        u32 it = 0;  // sample index


        // tip point
        poly.setPosAt(getPosExtrapolated(ie, it, startOffset), m_polyEdgeCount);

        for (u32 i = 1; i < m_polyEdgeCount; i++)
        {
            poly.setPosAt(getPosExtrapolated(ie, it, i * _edgeLength + startOffset) , m_polyEdgeCount - i);
        }

        // base point
        poly.setPosAt(getPosExtrapolated(ie, it, m_currentLength - endOffset), 0);
    }

    void Ray_BezierTreeAIComponent::Branch::updatePolyline_doubleSidedExtruded( 
        Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength )
    {
        PolyLine& poly = m_polyline->getPolyline();
        const u32 pointCount = poly.getPosCount();
        const f32 startOffset = _owner->getTemplate()->getPolylineStartOffset();
        const f32 endOffset = _owner->getTemplate()->getPolylineEndOffset();

        u32 ie = 0;  // edge index
        u32 it = 0;  // sample index


        // base points
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, startOffset, pos, normal);

            poly.setPosAt(pos + normal, 0);              // first point
            poly.setPosAt(pos - normal, pointCount-2);   // opposite to first point
            poly.setPosAt(pos + normal, pointCount-1);   // last point: close
        }

        for (u32 i = 1; i < m_polyEdgeCount; i++)
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, i * _edgeLength + startOffset, pos, normal);

            poly.setPosAt(pos + normal, i);
            poly.setPosAt(pos - normal, pointCount-2 - i);
        }

        // tip points
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, m_currentLength - endOffset, pos, normal);

            poly.setPosAt(pos + normal, m_polyEdgeCount);
            poly.setPosAt(pos - normal, m_polyEdgeCount+1);
        }
    }

    void Ray_BezierTreeAIComponent::Branch::updatePolyline_doubleSidedExtruded_sharpTip( 
        Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength )
    {
        PolyLine& poly = m_polyline->getPolyline();
        const u32 pointCount = poly.getPosCount();
        const f32 startOffset = _owner->getTemplate()->getPolylineStartOffset();
        const f32 endOffset = _owner->getTemplate()->getPolylineEndOffset();

        u32 ie = 0;  // edge index
        u32 it = 0;  // sample index


        // base points
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, startOffset, pos, normal);

            poly.setPosAt(pos + normal, 0);              // first point
            poly.setPosAt(pos - normal, pointCount-2);   // opposite to first point
            poly.setPosAt(pos + normal, pointCount-1);   // last point: close
        }

        for (u32 i = 1; i < m_polyEdgeCount; i++)
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, i * _edgeLength + startOffset, pos, normal);

            poly.setPosAt(pos + normal, i);
            poly.setPosAt(pos - normal, pointCount-2 - i);
        }

        // tip point
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, m_currentLength - endOffset, pos, normal);

            poly.setPosAt(pos, m_polyEdgeCount);
        }
    }

    void Ray_BezierTreeAIComponent::Branch::updatePolyline_leftExtruded(
        Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength )
    {
        PolyLine& poly = m_polyline->getPolyline();
        const f32 startOffset = _owner->getTemplate()->getPolylineStartOffset();
        const f32 endOffset = _owner->getTemplate()->getPolylineEndOffset();

        u32 ie = 0;  // edge index
        u32 it = 0;  // sample index


        // base point
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, startOffset, pos, normal);

            poly.setPosAt(pos + normal, 0);
        }

        for (u32 i = 1; i < m_polyEdgeCount; i++)
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, i * _edgeLength + startOffset, pos, normal);

            poly.setPosAt(pos + normal, i);
        }

        // tip point
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, m_currentLength - endOffset, pos, normal);

            poly.setPosAt(pos + normal, m_polyEdgeCount);
        }
    }

    void Ray_BezierTreeAIComponent::Branch::updatePolyline_rightExtruded(
        Ray_BezierTreeAIComponent* _owner, const f32 _edgeLength )
    {
        PolyLine& poly = m_polyline->getPolyline();
        const f32 startOffset = _owner->getTemplate()->getPolylineStartOffset();
        const f32 endOffset = _owner->getTemplate()->getPolylineEndOffset();

        u32 ie = 0;  // edge index
        u32 it = 0;  // sample index


        // tip point
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, startOffset, pos, normal);

            poly.setPosAt(pos - normal, m_polyEdgeCount);
        }

        for (u32 i = 1; i < m_polyEdgeCount; i++)
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, i * _edgeLength + startOffset, pos, normal);

            poly.setPosAt(pos - normal, m_polyEdgeCount - i);
        }

        // base point
        {
            Vec2d pos, normal;
            getPosAndNormalExtruded(_owner, ie, it, m_currentLength - endOffset, pos, normal);

            poly.setPosAt(pos - normal, 0);
        }
    }

    Vec2d Ray_BezierTreeAIComponent::Branch::getPosExtrapolated( u32& _edgeIndex, u32& _sampleIndex, const f32 _dist ) const
    {
        if (_dist < 0)
        {
            // extrapolate (negative start offset)
            Vec3d pos3d, tan3d;
            m_bezierCurve.getPosTanAtDistance(_edgeIndex, _sampleIndex, 0, pos3d, tan3d);
            tan3d.normalize();
            return (pos3d + tan3d * _dist).truncateTo2D();
        }
        else if (_dist <= m_bezierCurve.getTotalLength())
        {
            return m_bezierCurve.getPosAtDistance(_edgeIndex, _sampleIndex, _dist).truncateTo2D();
        }
        else
        {
            // extrapolate (negative end offset)
            Vec3d pos3d, tan3d;
            m_bezierCurve.getPosTanAtDistance(_edgeIndex, _sampleIndex, m_bezierCurve.getTotalLength(), pos3d, tan3d);
            tan3d.normalize();
            return (pos3d + tan3d * (_dist - m_bezierCurve.getTotalLength())).truncateTo2D();
        }
    }

    void Ray_BezierTreeAIComponent::Branch::getPosAndNormalExtruded( 
        Ray_BezierTreeAIComponent* _owner,
        u32& _edgeIndex, u32& _sampleIndex, const f32 _dist,
        Vec2d& _posOut, Vec2d& _normalOut ) const
    {
        const f32 polylineWidth = _owner->getTemplate()->interpolatePolylineWidth(_dist, m_currentLength);
        const f32 scale = getScale(_dist);
        const f32 extrusion = polylineWidth * scale * 0.5f;

        Vec3d pos3d, tan3d;
        if (_dist < 0)
        {
            // extrapolate (negative start offset)
            m_bezierCurve.getPosTanAtDistance(_edgeIndex, _sampleIndex, 0, pos3d, tan3d);
            tan3d.normalize();
            pos3d += tan3d * _dist;
        }
        else if (_dist <= m_bezierCurve.getTotalLength())
        {
            m_bezierCurve.getPosTanAtDistance(_edgeIndex, _sampleIndex, _dist, pos3d, tan3d);
            tan3d.normalize();
        }
        else
        {
            // extrapolate (negative end offset)
            m_bezierCurve.getPosTanAtDistance(_edgeIndex, _sampleIndex, m_bezierCurve.getTotalLength(), pos3d, tan3d);
            tan3d.normalize();
            pos3d += tan3d * (_dist - m_bezierCurve.getTotalLength());
        }

        _posOut = pos3d.truncateTo2D();

        _normalOut = tan3d.getPerpendicular().truncateTo2D() * extrusion;
    }

    f32 Ray_BezierTreeAIComponent::Branch::getScale( const f32 _curDist ) const
    {
        // maintain scale from the tip
        const f32 invDist = m_currentLength - _curDist;
        const f32 initialDist = Clamp(m_initialBezierCurve.getTotalLength() - invDist, 0.f, m_initialBezierCurve.getTotalLength());
        return getScaleFromInitialDist(initialDist);
    }

    f32 Ray_BezierTreeAIComponent::Branch::getScaleFromInitialDist( const f32 _initialDist ) const
    {
        const u32 i = m_initialBezierCurve.getEdgeIndexAtDistance(_initialDist);
        const f32 t = m_initialBezierCurve.getTAtDistance(_initialDist, i);

        return Interpolate(m_scales[i], m_scales[i + 1], t);
    }

    f32 Ray_BezierTreeAIComponent_Template::interpolatePolylineWidth( const f32 _dist, const f32 _totalLength ) const
    {
        if (_dist < m_polylineBeginLength)
        {
            if (m_polylineBeginLength > 0)
            {
                return m_polylineBeginWidth + (_dist / m_polylineBeginLength) * (m_polylineMidWidth - m_polylineBeginWidth);
            }
            return m_polylineBeginWidth;
        }
        else if (_dist <= _totalLength - m_polylineEndLength)
        {
            return m_polylineMidWidth;
        }
        else
        {
            if (m_polylineEndLength > 0)
            {
                const f32 factor = 1 - (_totalLength - _dist) / m_polylineEndLength;
                const f32 width = m_polylineMidWidth + factor * (m_polylineEndWidth - m_polylineMidWidth);
                return width;
            }
            return m_polylineEndWidth;
        }
    }


    void Ray_BezierTreeAIComponent::Branch::updateHead( Ray_BezierTreeAIComponent* _owner, Actor* _headActor )
    {
        Vec3d pos, tan;
        m_bezierCurve.getPosTanAtDistance(m_currentLength + _owner->getTemplate()->getHeadAttachOffset(), pos, tan);
        f32 angle = Vec3d_getAngle(tan) - MTH_PIBY2;

        _headActor->setPos(pos);
        _headActor->setAngle(angle);
    }

    void Ray_BezierTreeAIComponent::getAttachPointLocal( const Transform3d& _transform, BezierCurve::Point& _point ) const
    {
        if (!getTemplate()->getLockFirstNode())
        {
            // we don't use the attach pos in that mode
            return;
        }


        if (m_attachBoneIndex != U32_INVALID)
        {
            Vec3d worldPos;
            if (m_animComponent->getBonePos(m_attachBoneIndex, worldPos))
            {
                _point.m_pos = _transform.inverseTransformPos(worldPos);
            }
        }

        _point.m_tanB = _point.m_pos + Vec3d::Up;
    }

    void Ray_BezierTreeAIComponent::getAttachPointWorld( const Transform3d& _transform, BezierCurve::Point& _point ) const
    {
        if (!getTemplate()->getLockFirstNode())
        {
            // we don't use the attach pos in that mode
            return;
        }


        _point.m_pos = m_actor->getPos();

        if (m_attachBoneIndex != U32_INVALID)
        {
            Vec3d worldPos;
            if (m_animComponent->getBonePos(m_attachBoneIndex, worldPos))
            {
                _point.m_pos = worldPos;
            }
        }

        _point.m_tanB = _point.m_pos + _transform.transformDir(Vec3d::Up);
    }

    // build the curve foot to head; if we want to attach to head, we just offset the UV
    void Ray_BezierTreeAIComponent::Branch::buildCurve( Ray_BezierTreeAIComponent* _owner, BezierCurve& _bezierCurve,
        const BezierCurve::Point& _firstPoint )
    {
        u32 offset = 0;
        if (_owner->getTemplate()->getLockFirstNode())
        {
            // use actor as first point (tangA doesn't matter)
            _bezierCurve.getPointAt(0) = _firstPoint;
            offset = 1;
        }
        // else: all nodes count; must have at least 2

        for (u32 i = 0; i < m_nodes.size(); i++)
        {
            const Node& node = m_nodes[i];
            BezierCurve::Point& point = _bezierCurve.getPointAt(i + offset);
            point.m_pos  = node.getPos();
            point.m_tanA = node.getPos() - node.getTangent();
            point.m_tanB = node.getPos() + node.getTangent();
        }
    }


    void Ray_BezierTreeAIComponent::Branch::draw( Ray_BezierTreeAIComponent* _owner, Texture* _texture, const bbool _flipUV )
    {
        if (!checkNodeCount(_owner))
        {
            return;
        }

        if (m_currentLength < MTH_EPSILON)
        {
            return;
        }


        BezierCurveRenderer& renderer = _owner->m_bezierRenderer;

        const f32 tplTileLength = _owner->getTemplate()->getBezierRenderer().getTileLength();
        const f32 tileLength = _owner->getTemplate()->getUvStretch() ? m_bezierCurve.getTotalLength() * m_oneOverTotalInitialLength * tplTileLength : tplTileLength;
        const f32 uvLength = m_currentLength / tileLength;

        f32 startUV, endUV;
        if (_owner->getTemplate()->getUvAttachToHead())
        {
            startUV = -uvLength;
            endUV = 0;
        }
        else
        {
            startUV = 0;
            endUV = uvLength;
        }

        renderer.setDepth(_owner->GetActor()->getDepth());

        renderer_draw(renderer,
            0.f, m_currentLength,
            Vec2d(startUV - _owner->m_uvOffset, 0), Vec2d(endUV - _owner->m_uvOffset, 1),
            _texture, m_patches,
            m_bezierCurve, _flipUV);
    }

    void Ray_BezierTreeAIComponent::Branch::draw_end( Ray_BezierTreeAIComponent* _owner, Texture* _texture, const bbool _flipUV )
    {
        if (!checkNodeCount(_owner))
        {
            return;
        }

        if (m_currentLength < MTH_EPSILON)
        {
            return;
        }


        BezierCurveRenderer& renderer = _owner->m_bezierRenderer;

        renderer.setColor(_owner->getColorFactor());
        renderer.setDepth(_owner->GetActor()->getDepth());


        const f32 endLength = _owner->getTemplate()->getEndSpriteLength();

        // main texture
        if (m_currentLength > endLength)
        {
            const f32 tplTileLength = _owner->getTemplate()->getBezierRenderer().getTileLength();
            const f32 tileLength = _owner->getTemplate()->getUvStretch() ? m_bezierCurve.getTotalLength() * m_oneOverTotalInitialLength * tplTileLength : tplTileLength;

            // In this mode, we always want to attach the UVs to the tip (head)
            const UVdata& sprite = _owner->m_mainSprite;
            const f32 uvLength = (m_currentLength - endLength) / (tileLength * (sprite.getUV1().m_x - sprite.getUV0().m_x));

            renderer_draw(renderer,
                0.f, m_currentLength - endLength,
                Vec2d(sprite.getUV0().m_x - uvLength, sprite.getUV0().m_y),
                Vec2d(sprite.getUV0().m_x           , sprite.getUV1().m_y),
                _texture, m_patches,
                m_bezierCurve, _flipUV);
        }


        // end texture
        {
            const UVdata& sprite = _owner->m_endSprite;
            Vec2d startUV = sprite.getUV0();
            Vec2d endUV = sprite.getUV1();

            f32 startDist = m_currentLength - endLength;
            if (startDist < 0)
            {
                startUV.m_x = startUV.m_x + (endUV.m_x - startUV.m_x) * (-startDist / endLength);
                startDist = 0;
            }

            renderer_draw(renderer,
                startDist, m_currentLength,
                startUV, endUV,
                _texture, m_endPatches,
                m_bezierCurve, _flipUV);
        }
    }

    void Ray_BezierTreeAIComponent::Branch::draw_startEnd( Ray_BezierTreeAIComponent* _owner, Texture* _texture, const bbool _flipUV )
    {
        if (!checkNodeCount(_owner))
        {
            return;
        }

        if (m_currentLength < MTH_EPSILON)
        {
            return;
        }


        BezierCurveRenderer& renderer = _owner->m_bezierRenderer;

        renderer.setColor(_owner->getColorFactor());
        renderer.setDepth(_owner->GetActor()->getDepth());


        const f32 startLength = _owner->getTemplate()->getStartSpriteLength();
        const f32 endLength = _owner->getTemplate()->getEndSpriteLength();
        const f32 midLength = m_initialBezierCurve.getTotalLength() - startLength - endLength;



        // main texture
        if (m_currentLength > (startLength + endLength))
        {
            const f32 tplTileLength = _owner->getTemplate()->getBezierRenderer().getTileLength();

            /* In this mode, we always want:
                - a round number of UVs in the middle, to match start & end
                - to attach the UVs to the tip (head)
            */
            const UVdata& sprite = _owner->m_mainSprite;
            const f32 uvLength = roundFloat( midLength / (tplTileLength * (sprite.getUV1().m_x - sprite.getUV0().m_x)) );

            renderer_draw(renderer,
                startLength,
                m_currentLength - endLength,
                Vec2d(sprite.getUV0().m_x - uvLength, sprite.getUV0().m_y),
                Vec2d(sprite.getUV0().m_x           , sprite.getUV1().m_y),
                _texture, m_patches,
                m_bezierCurve, _flipUV);
        }


        // start texture
        if (m_currentLength > endLength)
        {
            const UVdata& sprite = _owner->m_startSprite;
            Vec2d startUV = sprite.getUV0();
            Vec2d endUV = sprite.getUV1();

            f32 length = startLength;
            if (m_currentLength < (startLength + endLength))
            {
                length = m_currentLength - endLength;
                startUV.m_x = startUV.m_x + (endUV.m_x - startUV.m_x) * (1 - length / startLength);
            }

            renderer_draw(renderer,
                0.f, length,
                startUV, endUV,
                _texture, m_startPatches,
                m_bezierCurve, _flipUV);
        }


        // end texture
        {
            const UVdata& sprite = _owner->m_endSprite;
            Vec2d startUV = sprite.getUV0();
            Vec2d endUV = sprite.getUV1();

            f32 startDist = m_currentLength - endLength;
            if (startDist < 0)
            {
                startUV.m_x = startUV.m_x + (endUV.m_x - startUV.m_x) * (-startDist / endLength);
                startDist = 0;
            }

            renderer_draw(renderer,
                startDist, m_currentLength,
                startUV, endUV,
                _texture, m_endPatches,
                m_bezierCurve, _flipUV);
        }
    }


    void Ray_BezierTreeAIComponent::Branch::renderer_draw(
        const BezierCurveRenderer& _renderer,
        const f32 _startDist, const f32 _endDist,
        const Vec2d& _startUV, const Vec2d& _endUV,
        Texture* _texture, BezierPatchList& _patches,
        const BezierCurve& _curve, const bbool _flipUV /*= bfalse*/ )
    {
        ITF_ASSERT(_texture);
        if (!_texture)
        {
            return;
        }

        const u32 patchCount = _curve.getNumEdges();
        ITF_ASSERT_CRASH(patchCount <= _patches.size(), "you need to resize your patches yourself! I don't do it because it can cause z-fighting when drawing only a portion of a curve.");
        ITF_ASSERT(patchCount);
        if (patchCount == 0)
        {
            return;
        }

        ITF_ASSERT_MSG(_curve.getTotalLength(), "you need to sample your curve!");
        ITF_ASSERT_MSG(_endDist <= _curve.getTotalLength(), "trying to draw after the curve!");
        ITF_ASSERT_MSG(_startDist >= 0.f, "trying to draw before the curve!");
        ITF_ASSERT(_startDist < _endDist);
        const f32 totalLength = (_endDist - _startDist);
        if (totalLength < MTH_EPSILON)
        {
            return;
        }



        BezierPatchParams patchParams;

        // params shared by all the patches
        patchParams.m_z = _renderer.getDepth();
        patchParams.m_texture = _texture;
        patchParams.m_uvMode = _renderer.getUvMode();
        patchParams.m_startColor = patchParams.m_endColor = _renderer.getColor();
        patchParams.m_fogColor = _renderer.getFogColor();
        patchParams.m_startUV = _startUV;
        patchParams.m_endUV = _endUV;
        if (_flipUV)
        {
            const f32 temp = patchParams.m_startUV.m_y;
            patchParams.m_startUV.m_y = patchParams.m_endUV.m_y;
            patchParams.m_endUV.m_y = temp;
        }


        const f32 uvRatio = (_endUV.m_x - _startUV.m_x) / totalLength;
        const f32 beginLength = _renderer.getBeginLength();
        const f32 endLength = _renderer.getEndLength();

        //f32 curWidth = (beginLength ? _renderer.getBeginWidth() : _renderer.getMidWidth()) * getScale(0);
        //f32 curAlpha = beginLength ? _renderer.getBeginAlpha() : _renderer.getMidAlpha();

        for (u32 i = 0; i < patchCount; ++i) 
        {
            const BezierCurve::Edge* edge = _curve.getEdgeAt(i);

            if (edge->m_length < MTH_EPSILON)
            {
                continue;
            }


            f32 edgeStart = edge->m_edgeStart;
            f32 edgeEnd = edge->m_edgeStart + edge->m_length;

            if (edgeEnd <= _startDist)
            {
                continue;
            }
            if (edgeStart >= _endDist)
            {
                break;
            }


            BezierCurve::Edge segment;
            if (edgeStart < _startDist && _endDist < edgeEnd )
            {
                // this edge starts AND stops further than we want to draw, split it
                edge->getSegment(_startDist, _endDist, segment);
                edge = &segment;

                edgeStart = _startDist;
                edgeEnd = Min(_startDist + edge->m_edgeStart + edge->m_length, _endDist);  // could be != _endDist due to sampling
            }
            else if (edgeStart < _startDist)
            {
                // this edge starts further than we want to draw, split it
                edge->getSegmentRight(_startDist, segment);
                edge = &segment;

                edgeStart = _startDist;
            }
            else if (edgeEnd > _endDist)
            {
                // this edge stops further than we want to draw, split it
                edge->getSegmentLeft(_endDist, segment);
                edge = &segment;

                edgeEnd = Min(_startDist + edge->m_edgeStart + edge->m_length, _endDist);  // could be != _endDist due to sampling
            }
            // else : the edge is fully drawn


            // edgeStart/end : relative to the whole curve
            // dist0/1 are relative to the portion we're drawing
            const f32 dist0 = edgeStart - _startDist;
            const f32 dist1 = edgeEnd - _startDist;


            // bezier
            patchParams.m_p0 = edge->m_bezier.m_p0.truncateTo2D();
            patchParams.m_p1 = edge->m_bezier.m_p1.truncateTo2D();
            patchParams.m_p2 = edge->m_bezier.m_p2.truncateTo2D();
            patchParams.m_p3 = edge->m_bezier.m_p3.truncateTo2D();


            // UV
            patchParams.m_startUV.m_x = _startUV.m_x + dist0 * uvRatio;
            patchParams.m_endUV.m_x   = _startUV.m_x + dist1 * uvRatio;


            // width
            patchParams.m_widthLimitA = 0.f;
            patchParams.m_widthLimitB = 1.f;
            //patchParams.m_widthValueA = curWidth;
            patchParams.m_widthValueA = _renderer.interpolateWidth(dist0, totalLength) * getScale(edgeStart);
            patchParams.m_widthValueB = _renderer.interpolateWidth(dist1, totalLength) * getScale(edgeEnd);
            patchParams.m_startWidth = patchParams.m_widthValueA;
            patchParams.m_endWidth   = patchParams.m_widthValueB;
            //curWidth = patchParams.m_widthValueB;


            // alpha
            patchParams.m_alphaLimitA = 0.f;
            patchParams.m_alphaLimitB = 1.f;
            //patchParams.m_alphaValueA = curAlpha;
            patchParams.m_alphaValueA = _renderer.interpolateAlpha(dist0, totalLength);
            patchParams.m_alphaValueB = _renderer.interpolateAlpha(dist1, totalLength);
            patchParams.m_startColor.m_a = patchParams.m_alphaValueA;
            patchParams.m_endColor.m_a   = patchParams.m_alphaValueB;
            //curAlpha = patchParams.m_alphaValueB;


            if (dist0 < beginLength && beginLength < dist1)
            {
                // OVERLAPS BEGIN SECTION
                const f32 t = edge->getTAtDistance(_startDist + beginLength);  // caution: the shader works with t, not d

                patchParams.m_widthLimitA = t;
                patchParams.m_startWidth = _renderer.interpolateWidth(beginLength, totalLength) * getScale(_startDist + beginLength);

                patchParams.m_alphaLimitA = t;
                patchParams.m_startColor.m_a = _renderer.interpolateAlpha(beginLength, totalLength);
            }
            // no 'else'; small (eg single-patch) curves can overlap both the begin and end sections
            if (dist0 < totalLength - endLength && totalLength - endLength < dist1)
            {
                // OVERLAPS END SECTION
                const f32 t = edge->getTAtDistance(_startDist + totalLength - endLength);  // caution: the shader works with t, not d

                patchParams.m_widthLimitB = t;
                patchParams.m_endWidth = _renderer.interpolateWidth(totalLength - endLength, totalLength) * getScale(_startDist + totalLength - endLength);

                patchParams.m_alphaLimitB = t;
                patchParams.m_endColor.m_a = _renderer.interpolateAlpha(totalLength - endLength, totalLength);
            }


            if ( _renderer.getTessellationRatio() > 0 )
            {
                patchParams.m_vdivToDraw = Max((u32)1, (u32)((edgeEnd - edgeStart) * _renderer.getTessellationRatio()));
            }


            _patches[i].drawPatch(patchParams);
        }
    }



#ifdef ITF_SUPPORT_EDITOR
    void Ray_BezierTreeAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            const Branch& branch = m_branches[iB];
            if (branch.m_polyline)
            {
                branch.m_polyline->drawEdit(_drawInterface, _flags);
            }
        }

        m_ai.drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
        {
            return;
        }

        const Transform3d transform = getTransform();

        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            const Branch& branch = m_branches[iB];
            if (branch.m_polyline)
            {
                DebugDraw::bezierCurve(branch.m_bezierCurve, Color::red(), 3.f);
                if (m_actor->isSelected())
                {
                    DebugDraw::bezierCurve(transform, branch.m_initialBezierCurve, Color::green(), 3.f);
                }
            }
        }

/*
        // draw node normals
        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            const Branch& branch = m_branches[iB];

            for (u32 iN = 0; iN < branch.m_nodes.size(); iN++)
            {
                const Node& node = branch.m_nodes[iN];

                DebugDraw::line3D( transform.transformPos(node.getInitialTransform().getPos()),
                    transform.transformPos(node.getInitialTransform().transformPos(Vec2d::Up)));
                //DebugDraw::line3D( transform.transformPos(node.getInitialTransform().getPos()),
                //                   transform.transformPos(node.getPos()) );
                //DebugDraw::circle( transform.transformPos(node.getPos()), 0.1f );
            }
        }
*/
    }
#endif // ITF_SUPPORT_EDITOR



    /**********************************************
                    AI STUFF
     *********************************************/

    #define OPEN_THRESHOLD 0.99f
    #define CLOSE_THRESHOLD 0.01f

    //BEGIN_SERIALIZATION(Ray_AnemoneAI)
    //    SERIALIZE_MEMBER("startActivated", m_startActivated);
    //END_SERIALIZATION()

    //BEGIN_VALIDATE_COMPONENT(Ray_AnemoneAI)
    //    VALIDATE_COMPONENT_PARAM("childMoveSpeed", getTemplate()->getChildMoveSpeed() > 0, "must be positive!");
    //    VALIDATE_COMPONENT_PARAM("", getTemplate()->getChildOpenDist() > getTemplate()->getChildAppearDist(), "childOpenDist needs to be greater than childAppearDist");
    //    VALIDATE_COMPONENT_PARAM("", getTemplate()->getChildCloseDist() > getTemplate()->getChildDisappearDist(), "childCloseDist needs to be greater than childDisappearDist");
    //END_VALIDATE_COMPONENT()

    #define s_Opening ITF_GET_STRINGID_CRC(Opening,133555280)
    #define s_HideTransition ITF_GET_STRINGID_CRC(HideTransition,3719224146)
    #define s_HideIdle ITF_GET_STRINGID_CRC(HideIdle,8755637)

    Ray_BezierTreeAIComponent::Ray_AnemoneAI::Ray_AnemoneAI()
        : m_actor(NULL)
        , m_owner(NULL)
        , m_template(NULL)
        , m_animComponent(NULL)
        , m_animatedComponent(NULL)
        , m_playerDetector(NULL)
        , m_startActivated(btrue)
        , m_branchSpeed(UNINIT_F32)
        , m_state(State_Invalid)
        , m_timer(0.f)
        , m_playersInsideCount(0)
    {
    }

    Ray_BezierTreeAIComponent::Ray_AnemoneAI::~Ray_AnemoneAI()
    {
    }

    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::onActorLoaded( Ray_BezierTreeAIComponent* _owner )
    {
        m_actor = _owner->GetActor();
        m_owner = _owner;
        m_template = &m_owner->getTemplate()->getAI();


        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), m_owner);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), m_owner);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();   // not mandatory
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_playerDetector = m_actor->GetComponent<PlayerDetectorComponent>(); // optional

        m_softCollision.init(&getTemplate()->getSoftCollision(), m_owner);

        if (m_branchSpeed == UNINIT_F32)
        {
            m_branchSpeed = m_template->getChildMoveSpeed();
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::editor_setActivated( bbool _value )
    {
        m_startActivated = _value;
        applyStateForced(m_startActivated);
    }
#endif // ITF_SUPPORT_EDITOR

    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::initAI()
    {
        if (m_state == State_Invalid)   // we may have been retriggered between onActorLoaded and onCheckpointLoaded...
        {
            applyStateForced(m_startActivated);
        }

        // init branches
        for (u32 iB = 0; iB < m_owner->m_branches.size(); iB++)
        {
            Branch& branch = m_owner->m_branches[iB];
            branch.initAI(this);
        }
    }

    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::applyStateForced( const bbool _open )
    {
        forceChangeState(_open ? State_Open : State_Closed);

        if (_open)
        {
            for (u32 iB = 0; iB < m_owner->m_branches.size(); iB++)
            {
                Branch& branch = m_owner->m_branches[iB];
                branch.forceOpen();
            }
        }
        else
        {
            for (u32 iB = 0; iB < m_owner->m_branches.size(); iB++)
            {
                Branch& branch = m_owner->m_branches[iB];
                branch.forceClose();
            }
        }
    }


    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::onEvent( Event* _event )
    {
        if ( EventGeneric* genericEvent = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)) )
        {
            if (genericEvent->getId() == ITF_GET_STRINGID_CRC(Open,812432275))
            {
                tryOpen();
            }
            else if (genericEvent->getId() == ITF_GET_STRINGID_CRC(ForceOpen,410328037))
            {
                applyStateForced(btrue);
            }
            else if (genericEvent->getId() == ITF_GET_STRINGID_CRC(Close,3883599349))
            {
                tryClose();
            }
            else if (genericEvent->getId() == ITF_GET_STRINGID_CRC(ForceClose,3309581368))
            {
                applyStateForced(bfalse);
            }
        }
        else if ( EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            if (trigger->getActivated())
            {
                if (!trigger->getFromRetriggerOnCheckpoint())
                {
                    // toggle
                    tryOpen();
                    tryClose();
                }
                else
                {
                    // force toggle
                    applyStateForced(!m_startActivated);
                }
            }
        }
        else
        {
            m_softCollision.onEvent(_event);
        }
    }

    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::tryOpen()
    {
        ITF_ASSERT(m_state != State_Invalid);
        if (m_state == State_Closed ||
            m_state == State_Closed_Wiggling ||
            m_state == State_Invalid)
        {
            changeState(State_Opening_In);
        }
    }

    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::tryClose()
    {
        ITF_ASSERT(m_state != State_Invalid);
        if (m_state == State_Open ||
            m_state == State_Open_Wiggling ||
            m_state == State_Invalid)
        {
            changeState(State_Closing_In);
        }
    }

    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::update( const f32 _dt )
    {
        updateState(_dt);

        m_softCollision.update();
    }

    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::changeState( const State _newState )
    {
        ITF_ASSERT(_newState != m_state);
        m_state = _newState;

        switch (m_state)
        {
        case State_Opening_In:
            {
                setCollisionDisabled(btrue);

                if (m_owner->getTemplate()->getPolylineDisableOnTransition())
                {
                    m_owner->m_polylineDisabled = bfalse;
                }

                for (u32 iB = 0; iB < m_owner->m_branches.size(); iB++)
                {
                    Branch& branch = m_owner->m_branches[iB];
                    branch.open();
                }

                if ( !playAnim(getTemplate()->getAnimDisappear()) )
                {
                    changeState(State_Opening_Loop1);
                }
            }
            break;

        case State_Opening_Loop1:
            {
                playAnim(getTemplate()->getAnimHidden());
            }
            break;

        case State_Opening_Loop2:
            {
                m_timer = 0.f;
            }
            break;

        case State_Opening_Out:
            {
                if ( !playAnim(getTemplate()->getAnimAppear()) )
                {
                    changeState(State_Open);
                }
            }
            break;


        case State_Closing_In:
            {
                setCollisionDisabled(btrue);

                if (m_owner->getTemplate()->getPolylineDisableOnTransition())
                {
                    m_owner->m_polylineDisabled = btrue;
                }

                for (u32 iB = 0; iB < m_owner->m_branches.size(); iB++)
                {
                    Branch& branch = m_owner->m_branches[iB];
                    branch.close();
                }

                if ( !playAnim(getTemplate()->getAnimDisappear()) )
                {
                    changeState(State_Closing_Loop1);
                }
            }
            break;

        case State_Closing_Loop1:
            {
                playAnim(getTemplate()->getAnimHidden());
            }
            break;

        case State_Closing_Loop2:
            {
                m_timer = 0.f;
            }
            break;

        case State_Closing_Out:
            {
                if ( !playAnim(getTemplate()->getAnimAppear()) )
                {
                    changeState(State_Closed);
                }
            }
            break;


        case State_Open:
            {
                playAnim(getTemplate()->getAnimOpened());

                setCollisionDisabled(bfalse);

                if (m_owner->getTemplate()->getPolylineDisableOnTransition())
                {
                    m_owner->m_polylineDisabled = bfalse;
                }
            }
            break;

        case State_Closed:
            {
                playAnim(getTemplate()->getAnimClosed());

                setCollisionDisabled(bfalse);

                if (m_owner->getTemplate()->getPolylineDisableOnTransition())
                {
                    m_owner->m_polylineDisabled = btrue;
                }
            }
            break;

        case State_Open_Wiggling:
            {
                playAnim(getTemplate()->getAnimWiggleOpened());
            }
            break;

        case State_Closed_Wiggling:
            {
                playAnim(getTemplate()->getAnimWiggleClosed());
            }
            break;
        }
    }

    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::updateState( const f32 _dt )
    {
        switch (m_state)
        {
        case State_Opening_In:
            {
                if ( AIUtils::isAnimFinished(m_animComponent, m_animatedComponent) )
                {
                    changeState(State_Opening_Loop1);
                }
            }
            break;

        case State_Opening_Loop1:
            {
                if ( allChildrenOpen() )
                {
                    changeState(State_Opening_Loop2);
                }
            }
            break;

        case State_Opening_Loop2:
            {
                m_timer += _dt;
                if (m_timer > getTemplate()->getOpeningDelay())
                {
                    changeState(State_Opening_Out);
                }
            }
            break;

        case State_Opening_Out:
            {
                if ( AIUtils::isAnimFinished(m_animComponent, m_animatedComponent) )
                {
                    changeState(State_Open);
                }
            }
            break;


        case State_Closing_In:
            {
                if ( AIUtils::isAnimFinished(m_animComponent, m_animatedComponent) )
                {
                    changeState(State_Closing_Loop1);
                }
            }
            break;

        case State_Closing_Loop1:
            {
                if ( allChildrenClosed() )
                {
                    changeState(State_Closing_Loop2);
                }
            }
            break;

        case State_Closing_Loop2:
            {
                m_timer += _dt;
                if (m_timer > getTemplate()->getClosingDelay())
                {
                    changeState(State_Closing_Out);
                }
            }
            break;

        case State_Closing_Out:
            {
                if ( AIUtils::isAnimFinished(m_animComponent, m_animatedComponent) )
                {
                    changeState(State_Closed);
                }
            }
            break;


        case State_Open:
            {
                detectWiggle();
            }
            break;

        case State_Closed:
            {
                detectWiggle();
            }
            break;

        case State_Open_Wiggling:
            {
                if (AIUtils::isAnimFinished(m_animComponent, m_animatedComponent))
                {
                    changeState(State_Open);
                }
            }
            break;

        case State_Closed_Wiggling:
            {
                if (AIUtils::isAnimFinished(m_animComponent, m_animatedComponent))
                {
                    changeState(State_Closed);
                }
            }
            break;
        }
    }

    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::detectWiggle()
    {
        if ( !getTemplate()->getCanWiggle() ||
             !m_playerDetector ||
             !m_animComponent )
        {
            return;
        }


        // wiggle when player detected
        if ( m_playerDetector->getActorsInside().size() > m_playersInsideCount )
        {
            changeState((m_state == State_Open) ? State_Open_Wiggling : State_Closed_Wiggling);

            // pad rumble
            if (getTemplate()->getPadRumbleWiggle().isValid())
            {
                ActorRef playerRef = m_playerDetector->getActorsInside().back();
                if (Player* player = GAMEMANAGER->getPlayerFromActor(playerRef))
                {
                    PADRUMBLEMANAGER->startRumble(
                        getTemplate()->getPadRumbleWiggle(),
                        player->getIndex()); 
                }
            }
        }

        // remember actors inside count
        m_playersInsideCount = m_playerDetector->getActorsInside().size();
    }


    bbool Ray_BezierTreeAIComponent::Ray_AnemoneAI::allChildrenOpen() const
    {
        for (u32 iB = 0; iB < m_owner->m_branches.size(); iB++)
        {
            Branch& branch = m_owner->m_branches[iB];
            if (branch.getT() < OPEN_THRESHOLD)
            {
                return bfalse;
            }
        }

        return btrue;
    }

    bbool Ray_BezierTreeAIComponent::Ray_AnemoneAI::allChildrenClosed() const
    {
        for (u32 iB = 0; iB < m_owner->m_branches.size(); iB++)
        {
            Branch& branch = m_owner->m_branches[iB];
            if (branch.getT() > CLOSE_THRESHOLD)
            {
                return bfalse;
            }
        }

        return btrue;
    }

    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::setCollisionDisabled( const bbool _disable )
    {
        if (m_softCollision.getDisabled() != _disable)
        {
            m_softCollision.setDisabled(_disable);
            AIUtils::disableCollision(m_actor, _disable);
        }
    }

    bbool Ray_BezierTreeAIComponent::Ray_AnemoneAI::playAnim( const StringID _anim )
    {
        if (m_animComponent)
        {
            ITF_ASSERT(_anim.isValid());
            if (_anim.isValid())
            {
                m_animComponent->setAnim(_anim);
                return btrue;
            }
        }

        return bfalse;
    }



    void Ray_BezierTreeAIComponent::Branch::initAI( const Ray_AnemoneAI* _ai )
    {
        const f32 randomAmount = _ai->getBranchSpeedRandom();
        const f32 randomSpeedMultiplier = Seeder::getSharedSeeder().GetFloat(1 - randomAmount, 1 + randomAmount);
        m_timeRatio = _ai->getBranchSpeed() * randomSpeedMultiplier / m_initialBezierCurve.getTotalLength();
    }

    void Ray_BezierTreeAIComponent::Branch::updateAI( Ray_BezierTreeAIComponent* _owner, const f32 _dt, Actor* _headActor )
    {
        const bbool activated = (m_state == State_Open);
        const Ray_PlatformTreeComponent_Template* aiTemplate = &_owner->getTemplate()->getAI();


        // time is lerped; distance is smoothed by a curve
        static LogisticParams timeCurve;

        // compute desired dist; this may affect the child's state, who can then decide if he accepts the move or not
        const f32 relDT = (activated ? _dt : -_dt) * m_timeRatio;
        const f32 t = Clamp(m_time + relDT, 0.f, 1.f);

        f32 dist = 0.f;
        if (aiTemplate->getFastMode())
        {
            if (activated)
            {
                // use only the second half of the curve for a fast exit
                dist = timeCurve.getValue(0.5f + t * 0.5f) * 2.f - 1.f;
            }
            else
            {
                // use only the first half of the curve for a fast reentry
                dist = timeCurve.getValue(t * 0.5f) * 2.f;
            }
        }
        else
        {
            dist = timeCurve.getValue(t);
        }


        // depending on distance, set state on childAI OR enable/disable childActor

        Ray_PlatformAIComponent* headAI = _headActor ? _headActor->GetComponent<Ray_PlatformAIComponent>() : NULL;
        if (headAI)
        {
            headAI->setState(activated, dist, bfalse, aiTemplate);
            if (!headAI->canMove(activated))
            {
                return;
            }

            // no need to update fully closed child
            if (t <= 0.f)
            {
                _headActor->disable();
            }
            else
            {
                _headActor->enable();
            }
        }

        // move accepted, save the time
        m_time = t;
    }


#ifdef ITF_SUPPORT_EDITOR
    void Ray_BezierTreeAIComponent::Ray_AnemoneAI::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        //Super::drawEdit(_drawInterface,_flags);

        m_softCollision.drawEdit(_drawInterface, _flags);
    }
#endif // ITF_SUPPORT_EDITOR


    void Ray_BezierTreeAIComponent::updateFx( const f32 _dt, const f32 _prevBranchTime )
    {
        if (!m_fxController)
        {
            return;
        }

        if (!getTemplate()->getFxName().isValid())
        {
            return;
        }


        for (u32 iB = 0; iB < m_branches.size(); iB++)
        {
            Branch& branch = m_branches[iB];
            branch.updateFx(this);
        }


        Branch& branch = m_branches[0];
        m_speedInputValue = (branch.getT() - _prevBranchTime) * branch.m_initialBezierCurve.getTotalLength() / _dt;
    }

    void Ray_BezierTreeAIComponent::Branch::updateFx( Ray_BezierTreeAIComponent* _owner )
    {
        if (m_state == State_Open)
        {
            if (m_fxHandle == U32_INVALID)
            {
                if (m_time < OPEN_THRESHOLD)
                {
                    m_fxHandle = _owner->m_fxController->playFX(_owner->getTemplate()->getFxName());
                }
            }
            else
            {
                if (m_time > OPEN_THRESHOLD)
                {
                    _owner->m_fxController->stopFX(m_fxHandle);
                    m_fxHandle = U32_INVALID;
                }
            }
        }
        else
        {
            if (m_fxHandle == U32_INVALID)
            {
                if (m_time > CLOSE_THRESHOLD)
                {
                    m_fxHandle = _owner->m_fxController->playFX(_owner->getTemplate()->getFxName());
                }
            }
            else
            {
                if (m_time < CLOSE_THRESHOLD)
                {
                    _owner->m_fxController->stopFX(m_fxHandle);
                    m_fxHandle = U32_INVALID;
                }
            }
        }
    }

    void Ray_BezierTreeAIComponent::updateSoundInput()
    {
        m_soundComponent->setInputFromIndex(m_speedInputIndex, m_speedInputValue);
    }

};

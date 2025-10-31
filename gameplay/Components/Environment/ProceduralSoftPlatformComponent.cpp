#include "precompiled_gameplay.h"

#ifndef _ITF_PROCEDURALSOFTPLATFORMCOMPONENT_H_
#include "gameplay/components/Environment/ProceduralSoftPlatformComponent.h"
#endif //_ITF_PROCEDURALSOFTPLATFORMCOMPONENT_H_

#ifndef _ITF_SWINGSOFTPLATFORM_H_
#include "gameplay/components/Environment/SwingSoftPlatform.h"
#endif //_ITF_SWINGSOFTPLATFORM_H_

#ifndef _ITF_SOFTPLATFORM_H_
#include "gameplay/components/Environment/SoftPlatform.h"
#endif //_ITF_SOFTPLATFORM_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_

#ifndef _ITF_BEZIERCURVECOMPONENT_H_
#include "gameplay/Components/Misc/BezierCurveComponent.h"
#endif //_ITF_BEZIERCURVECOMPONENT_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_


#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(ProceduralSoftPlatformComponent)

    BEGIN_SERIALIZATION_CHILD(ProceduralSoftPlatformComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("endPos", m_endPos);
            SERIALIZE_MEMBER("useBeginTexture", m_useBeginTexture);
            SERIALIZE_MEMBER("useEndTexture", m_useEndTexture);
            SERIALIZE_MEMBER("initIteration", m_initIteration);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()


    ProceduralSoftPlatformComponent::ProceduralSoftPlatformComponent()
        : m_softPlatform(NULL)
        , m_useBeginTexture(btrue)
        , m_useEndTexture(btrue)
        , m_initIteration(bfalse)
    {
    }

    ProceduralSoftPlatformComponent::~ProceduralSoftPlatformComponent()
    {
        SF_DEL(m_softPlatform);
        if (m_beginTextureID.isValidResourceId())
        {
            m_actor->removeResource(m_beginTextureID);
            m_beginTextureID.invalidateResourceId();
        }
        if (m_endTextureID.isValidResourceId())
        {
            m_actor->removeResource(m_endTextureID);
            m_endTextureID.invalidateResourceId();
        }
    }

    void ProceduralSoftPlatformComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);

        m_bezierRenderer.onLoaded(&getTemplate()->getBezierRenderer(), m_actor->getResourceGroup());
        m_bezierRenderer.setColor(getColorFactor());
        m_bezierRenderer.setFogColor(getColorFog());

        if (!getTemplate()->getBeginTextureFile().isEmpty())
        {
            m_beginTextureID = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getBeginTextureFile());
        }
        if (!getTemplate()->getEndTextureFile().isEmpty())
        {
            m_endTextureID = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getEndTextureFile());
        }
        init();
    }




    void ProceduralSoftPlatformComponent::onBecomeActive()
    {
        m_softPlatform->registerPhysics(btrue);

        Super::onBecomeActive();
    }

    void ProceduralSoftPlatformComponent::onBecomeInactive()
    {
        m_softPlatform->registerPhysics(bfalse);

        Super::onBecomeInactive();
    }

    void ProceduralSoftPlatformComponent::Update( f32 _deltaTime )
    {
        m_softPlatform->update(_deltaTime);

        SoftPlatform::BodiesArray & bodies = m_softPlatform->getBodyList();
        if(bodies.size() != m_curve.getNumPoints())
            return;

        if (!bodies.size())
            return;

        Vec2d prevDir(0.0f,0.0f);
        f32 factor = 1.0f;

        const Transform3d transform = getWorldTransform(m_actor,bfalse);

        AABB aabb(m_actor->getPos());


        if (bodies.size())
        {
            SoftPlatform::BodyData& softBodyData = m_softPlatform->getBodyList()[0];
            softBodyData.m_body->setPos(m_actor->get2DPos());

            SoftPlatform::BodyData & bodyA = bodies[0];
            Vec2d pA = bodyA.m_body->getPos();
            if (m_useBeginTexture && m_beginTextureID.isValidResourceId())
            {
                AABB quad(pA);
                quad.setMin(quad.getMin() - Vec2d::One);
                quad.setMax(quad.getMax() + Vec2d::One);
                aabb.grow(quad);
            }
            else
            {
                aabb.grow(pA);
            }

            SoftPlatform::BodyData & bodyB = bodies[bodies.size()-1];
            Vec2d pB = bodyB.m_body->getPos();
            if ( m_useEndTexture && m_endTextureID.isValidResourceId())
            {
                AABB quad(pB);
                quad.setMin(quad.getMin() - Vec2d::One);
                quad.setMax(quad.getMax() + Vec2d::One);
                aabb.grow(quad);
            }
            else
            {
                aabb.grow(pB);
            }

        }
        if (bodies.size() > 1 && !getTemplate()->getLiana())
        {
            SoftPlatform::BodyData& softBodyData = m_softPlatform->getBodyList()[bodies.size()-1];
            softBodyData.m_body->setPos(transform.transformPos(m_endPos).truncateTo2D());
        }

        for (u32 i = 0; i < bodies.size()-1; ++i)
        {
            SoftPlatform::BodyData & bodyA = bodies[i];
            BezierCurve::Point & pointA = m_curve.getPointAt(i);
            SoftPlatform::BodyData & bodyB = bodies[i+1];

            Vec2d pA = bodyA.m_body->getPos();
            Vec2d pB = bodyB.m_body->getPos();

            Vec2d dir = pB - pA;
            factor = Min(1.0f,dir.norm() *0.5f);
            dir.normalize();


            aabb.grow(pA); 

            if (prevDir != Vec2d::Zero)
            {
                Vec2d newDir = dir + prevDir;
                newDir.normalize();
                Vec2d tanA = pA - newDir * factor;
                Vec2d tanB = pA + newDir * factor;
                aabb.grow(tanA); 
                aabb.grow(tanB); 
                pointA.m_pos = transform.inverseTransformPos(bodyA.m_body->getPos());
                pointA.m_tanA = transform.inverseTransformPos(tanA);
                pointA.m_tanB = transform.inverseTransformPos(tanB);
            }
            else
            {
                Vec2d tanA = pA - dir * factor;
                Vec2d tanB = pA + dir * factor;
                aabb.grow(tanA); 
                aabb.grow(tanB); 
                pointA.m_pos = transform.inverseTransformPos(bodyA.m_body->getPos());
                pointA.m_tanA = transform.inverseTransformPos(tanA);
                pointA.m_tanB = transform.inverseTransformPos(tanB);
            }

            prevDir = dir;
        }

        //last point
        SoftPlatform::BodyData & bodyA = bodies[bodies.size()-1];
        BezierCurve::Point & pointA = m_curve.getPointAt(bodies.size()-1);
        pointA.m_pos = transform.inverseTransformPos(bodyA.m_body->getPos());
        Vec2d tanA = bodyA.m_body->getPos() - prevDir * factor;
        Vec2d tanB = bodyA.m_body->getPos() + prevDir * factor;
        pointA.m_tanA = transform.inverseTransformPos(tanA);
        pointA.m_tanB = transform.inverseTransformPos(tanB);

        m_curve.buildEdges(getTemplate()->getBezierSampling());

        m_actor->growRelativeAABBFromAbsolute(aabb);
    }

    void ProceduralSoftPlatformComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        if ( m_softPlatform)
        {
            m_softPlatform->onEvent(_event);
        }
    }

    void ProceduralSoftPlatformComponent::Draw()
    {
        Super::Draw();

        const Transform3d transform = getWorldTransform(m_actor,bfalse);
        const f32 depth = m_actor->getDepth();

        m_bezierRenderer.setDepth(m_actor->getDepth());
        m_bezierRenderer.draw(m_curve, &transform,m_actor->isFlipped());

        if (m_curve.getNumEdges() > 0)
        {
            if (m_useBeginTexture && m_beginTextureID.isValidResourceId())
            {
                Texture * texture = static_cast<Texture*>(m_beginTextureID.getResource());

                if (texture)
                {
                    Vec3d beginPos = transform.transformPos( m_curve.getPosAtT(0.0f,0) );

                    m_beginQuad.m_color          = getColorFactor().getAsU32();
                    m_beginQuad.m_size           = Vec2d::One;
                    m_beginQuad.m_pos            = beginPos;
                    m_beginQuad.m_blendMode      = GFX_BLEND_ALPHA;
                    m_beginQuad.m_rotation = Vec3d::Zero;
                    GFX_ADAPTER->addPrimitiveInZList((void*)&m_beginQuad, GFX_ITF_3DQUAD, texture, depth, bfalse,m_actor->getRef());
                }
            }

            if ( m_useEndTexture && m_endTextureID.isValidResourceId())
            {
                Texture * texture = static_cast<Texture*>(m_endTextureID.getResource());

                if (texture)
                {
                    Vec3d endPos = transform.transformPos( m_curve.getPosAtT(1.0f,m_curve.getNumEdges()-1) );

                    m_endQuad.m_color          = getColorFactor().getAsU32();
                    m_endQuad.m_size           = Vec2d::One;
                    m_endQuad.m_pos            = endPos;
                    m_endQuad.m_blendMode      = GFX_BLEND_ALPHA;
                    m_endQuad.m_rotation = Vec3d::Zero;
                    GFX_ADAPTER->addPrimitiveInZList((void*)&m_endQuad, GFX_ITF_3DQUAD, texture, depth, bfalse,m_actor->getRef());
                }
            }
        }
    }

    void ProceduralSoftPlatformComponent::setEndPos( ITF::Vec3d val )
    {
        m_endPos = val;
        SoftPlatform::BodiesArray & bodies = m_softPlatform->getBodyList();
        if (bodies.size())
        {
            SoftPlatform::BodyData & body = bodies.back();

            const Transform3d transform = getWorldTransform(m_actor,bfalse);
            body.m_body->setPos3D(transform.transformPos(m_endPos));
        }
    }

    void ProceduralSoftPlatformComponent::init()
    {
        SF_DEL(m_softPlatform);
        if (getTemplate()->getSwing())
        {
            m_softPlatform = newAlloc(mId_Gameplay,SwingSoftPlatform());
        }
        else 
        {
            m_softPlatform = newAlloc(mId_Gameplay, SoftPlatform());
        }
        m_softPlatform->setOwner( m_actor);

        if ( getTemplate()->getGameMaterial().getStringID() != GameMaterial_Template::nullMatName.getStringID() )
        {
            World::addGameMaterialFromFile(getTemplate()->getGameMaterial(), m_actor->getResourceGroup());
        }

        SoftPlatform::SoftPlatformInitData initData;
        initData.m_depth = m_actor->getDepth();
        initData.m_movingPolyForce = getTemplate()->getMovingPolyForce();
        initData.m_weightMultiplier = getTemplate()->getWeightMultiplier();
        initData.m_landSpeedMultiplier = getTemplate()->getLandSpeedMultiplier();
        initData.m_hitForceMultiplier = getTemplate()->getHitForceMultiplier();
        initData.m_impulseMultiplier = getTemplate()->getImpulseMultiplier();
        initData.m_gameMaterial = getTemplate()->getGameMaterial().getStringID();
        initData.m_phantom = getTemplate()->getPhantom();


        m_curve.clear();

        const f32 totalLength = m_endPos.norm() * getTemplate()->getLengthFactor();
        const f32 edgeLength = getTemplate()->getEdgeLength();
        const u32 numSegments = (u32)(totalLength/edgeLength);

        Vec3d pos;
        for (u32 i = 0; i < numSegments; ++i)
        {
            m_curve.addPoint(pos,Vec3d::Zero,Vec3d::Zero);
            pos.m_x += edgeLength; 
        }

        m_curve.buildEdges(getTemplate()->getBezierSampling());


        initData.m_bodies.resize(m_curve.getNumPoints());
        initData.m_constraints.resize(m_curve.getNumEdges());

        const Transform3d transform = getWorldInitTransform(m_actor,bfalse);

        const f32 gravityMultiplier = getTemplate()->getBodyGravityMultiplier();
        const f32 windMultiplier = getTemplate()->getBodyWindMultiplier();

        for (u32 i = 0; i < m_curve.getNumPoints(); ++i)
        {
            const BezierCurve::Point & point = m_curve.getPointAt(i);
            SoftPlatform::BodyInitData & body = initData.m_bodies[i];

            body.m_pos = transform.transformPos(point.m_pos).truncateTo2D();
            body.m_static = bfalse;
            body.m_gravityMultiplier = gravityMultiplier;
            body.m_windMultiplier = windMultiplier;
        }

        if (initData.m_bodies.size())
        {
            initData.m_bodies[0].m_static = btrue;
            if (!getTemplate()->getLiana())
                initData.m_bodies[initData.m_bodies.size()-1].m_static = btrue;
        }

        const Angle minAngle = getTemplate()->getConstraintMinAngle();
        const Angle maxAngle = getTemplate()->getConstraintMaxAngle();
        const f32 minLength = getTemplate()->getConstraintMinLength();
        const f32 maxLength = getTemplate()->getConstraintMaxLength();
        const f32 stiff = getTemplate()->getConstraintStiff();
        const f32 damp = getTemplate()->getConstraintDamp();
        bbool limitAngle = getTemplate()->getConstraintLimitAngle();
        bbool relaxLength = getTemplate()->getConstraintRelaxLength();

        for (u32 i = 0; i < m_curve.getNumEdges(); ++i)
        {
            SoftPlatform::ConstraintInitData & constraint = initData.m_constraints[i];
            constraint.m_bodyAIndex = i;
            constraint.m_bodyBIndex = i+1;
            constraint.m_minAngle = minAngle;
            constraint.m_maxAngle = maxAngle;
            constraint.m_minLength = minLength;
            constraint.m_maxLength = maxLength;
            constraint.m_stiff = stiff;
            constraint.m_damp = damp;
            constraint.m_limitAngle = limitAngle;
            constraint.m_relaxLength = relaxLength;
        }

        m_softPlatform->registerPhysics(btrue);
        m_softPlatform->init( initData);

        setEndPos(m_endPos);

        if ( m_initIteration )
        {
            PHYSWORLD->iterateSystem(m_actor->getRef());
        }

        m_softPlatform->registerPhysics(bfalse);
    }

#ifdef ITF_SUPPORT_EDITOR
    void ProceduralSoftPlatformComponent::editor_setPos(Vec3d _pos)
    {
        setEndPos(_pos);
        init();
        m_softPlatform->registerPhysics(btrue);
    }

    void ProceduralSoftPlatformComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags );

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        m_softPlatform->drawEdit(_drawInterface,_flags);
    }

    void ProceduralSoftPlatformComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        init();
        m_softPlatform->registerPhysics(btrue);
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(ProceduralSoftPlatformComponent_Template)
    BEGIN_SERIALIZATION(ProceduralSoftPlatformComponent_Template)
    SERIALIZE_MEMBER("isSwing",m_isSwing);
    SERIALIZE_MEMBER("gameMaterial",m_gameMaterial);
    SERIALIZE_MEMBER("isLiana",m_isLiana);
    SERIALIZE_MEMBER("movingPolyForce",m_movingPolyForce);
    SERIALIZE_MEMBER("weightMultiplier",m_weightMultiplier);
    SERIALIZE_MEMBER("landSpeedMultiplier",m_landSpeedMultiplier);
    SERIALIZE_MEMBER("hitForceMultiplier",m_hitForceMultiplier);
    SERIALIZE_MEMBER("impulseMultiplier",m_impulseMultiplier);
    SERIALIZE_MEMBER("phantom",m_phantom);
    SERIALIZE_MEMBER("bodyWindMultiplier",m_bodyWindMultiplier);
    SERIALIZE_MEMBER("bodyGravityMultiplier",m_bodyGravityMultiplier);
    SERIALIZE_MEMBER("constraintMinAngle",m_constraintMinAngle);
    SERIALIZE_MEMBER("constraintMaxAngle",m_constraintMaxAngle);
    SERIALIZE_MEMBER("constraintMinLength",m_constraintMinLength);
    SERIALIZE_MEMBER("constraintMaxLength",m_constraintMaxLength);
    SERIALIZE_MEMBER("constraintStiff",m_constraintStiff);
    SERIALIZE_MEMBER("constraintDamp",m_constraintDamp);
    SERIALIZE_MEMBER("constraintLimitAngle",m_constraintLimitAngle);
    SERIALIZE_MEMBER("constraintRelaxLength",m_constraintRelaxLength);
    SERIALIZE_MEMBER("lengthFactor",m_lengthFactor);
    SERIALIZE_MEMBER("edgeLength",m_edgeLength);
    SERIALIZE_OBJECT("bezierRenderer",m_bezierRenderer);
    SERIALIZE_MEMBER("beginTexture",m_beginTextureFile);
    SERIALIZE_MEMBER("endTexture",m_endTextureFile);
    SERIALIZE_MEMBER("bezierSampling",m_bezierSampling);

    END_SERIALIZATION()

    ProceduralSoftPlatformComponent_Template::ProceduralSoftPlatformComponent_Template()
    : m_isSwing(bfalse)
    , m_isLiana(bfalse)
    , m_movingPolyForce(0.0f)
    , m_weightMultiplier(1.0f)
    , m_landSpeedMultiplier(1.0f)
    , m_hitForceMultiplier(1.0f)
    , m_impulseMultiplier(1.0f)
    , m_phantom(btrue)
    , m_bodyWindMultiplier(1.0f)
    , m_bodyGravityMultiplier(1.0f)
    , m_constraintMinAngle(btrue,0.0f)
    , m_constraintMaxAngle(btrue, 360.0f)
    , m_constraintMinLength(0.0f)
    , m_constraintMaxLength(1.0f)
    , m_constraintStiff(0.0f)
    , m_constraintDamp(0.0f)
    , m_constraintLimitAngle(bfalse)
    , m_constraintRelaxLength(bfalse)
    , m_lengthFactor(1.0f)
    , m_edgeLength(1.0f)
    , m_bezierSampling(1)
    {
    }




}


#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_ActivationSequenceVisualComponent.h"
#endif //_ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Ray_ActivationSequenceVisualComponent)

BEGIN_SERIALIZATION_CHILD(Ray_ActivationSequenceVisualComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_ActivationSequenceVisualComponent)
    VALIDATE_COMPONENT_PARAM("", m_animComponent, "Ray_ActivationSequenceVisualComponent requires an AnimatedComponent.");
END_VALIDATE_COMPONENT()

Ray_ActivationSequenceVisualComponent::Ray_ActivationSequenceVisualComponent()
: m_animComponent(NULL)
, m_linkPatchTarget(Vec2d::Zero)
, m_linkPatchStart(Vec2d::Zero)
, m_startOffset(0.f)
, m_targetOffset(0.f)
, m_uvStart(0.f)
, m_stoneRef(ITF_INVALID_OBJREF)
, m_user(ITF_INVALID_OBJREF)
, m_prevUser(ITF_INVALID_OBJREF)
, m_activateTime(0.f)
, m_activateTimeTotal(0.f)
, m_timer(0.f)
, m_activationFinished(bfalse)
, m_drawPatch(bfalse)
, m_isDisabled(btrue)
{
}

Ray_ActivationSequenceVisualComponent::~Ray_ActivationSequenceVisualComponent()
{
    if (m_texture.isValidResourceId())
    {
        m_actor->removeResource(m_texture);
        m_texture.invalidateResourceId();
    }
}

void Ray_ActivationSequenceVisualComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimatedComponent>();

    if ( m_animComponent )
    {
        m_animComponent->setUpdateAnimInput(this);
    }

    if (!getTemplate()->getPatchTextureFile().isEmpty())
    {
        m_texture = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getPatchTextureFile());
    }

    m_startOffset = Seeder::getSharedSeeder().GetFloat(-getTemplate()->getPatchStartTangeantRotationFrequency(),getTemplate()->getPatchStartTangeantRotationFrequency());
    m_targetOffset = Seeder::getSharedSeeder().GetFloat(-getTemplate()->getPatchTargetTangeantRotationFrequency(),getTemplate()->getPatchTargetTangeantRotationFrequency());
    m_uvStart = Seeder::getSharedSeeder().GetFloat(0.0f,getTemplate()->getPatchTileLength());

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryEmptyStone,343141899), this);
}

void Ray_ActivationSequenceVisualComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    updatePatch(_dt);

    if ( m_activationFinished )
    {
        return;
    }

    if ( m_user.isValid() )
    {
        m_activateTime = Min(m_activateTime+_dt,m_activateTimeTotal);
    }
    else
    {
        m_activateTime = 0.f;
    }

    m_prevUser = m_user;
}

void Ray_ActivationSequenceVisualComponent::setOccupiedState( ActorRef _stoneRef, ActorRef _user, f32 _activateTimeTotal )
{
    m_stoneRef = _stoneRef;
    m_user = _user;
    m_activateTimeTotal = _activateTimeTotal;
}

void Ray_ActivationSequenceVisualComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( Ray_EventQueryEmptyStone* queryEmpty = _event->DynamicCast<Ray_EventQueryEmptyStone>(ITF_GET_STRINGID_CRC(Ray_EventQueryEmptyStone,343141899)) )
    {
        processQueryEmpty(queryEmpty);
    }
}

void Ray_ActivationSequenceVisualComponent::updatePatch( f32 _dt )
{
    m_drawPatch = bfalse;

    Actor* stone = m_stoneRef.getActor();

    if ( !stone )
    {
        return;
    }

    Actor* user = m_user.getActor();

    if ( !user )
    {
        return;
    }

    m_linkPatchTarget = stone->get2DPos();
    m_linkPatchStart = m_actor->get2DPos();
    m_uvStart += getTemplate()->getPatchScrollSpeed();
    m_timer += _dt;
    m_drawPatch = btrue;
}

void Ray_ActivationSequenceVisualComponent::processQueryEmpty( Ray_EventQueryEmptyStone* _queryEmpty )
{
    ActorRef parentRef = m_actor->getParent();
    Actor* parentActor = parentRef.getActor();

    if ( parentActor )
    {
        parentActor->onEvent(_queryEmpty);
    }
}

void Ray_ActivationSequenceVisualComponent::Draw()
{
    Super::Draw();

    if ( !m_drawPatch )
    {
        return;
    }

    Vec2d dir = m_linkPatchTarget - m_linkPatchStart;
    dir.normalize();

    f32 targetDist = (m_linkPatchTarget - m_linkPatchStart).norm();
    f32 radius = getTemplate()->getPatchStartRadius();
    Vec2d startPoint = m_linkPatchStart + dir * radius;
    f32 cAngle = sinf((m_timer+ m_startOffset)/getTemplate()->getPatchStartTangeantRotationFrequency() ) * getTemplate()->getPatchStartTangeantRotationOffset().Radians();
    f32 uvStartEnd = m_uvStart;
    f32 alpha = m_activateTime/m_activateTimeTotal;

    Color colorA = getColorFactor();
    colorA.m_a = 0.f;
    Color colorB = getColorFactor();
    colorB.m_a = alpha;

    const Vec2d tP0 = startPoint;
    const Vec2d tP3 = m_linkPatchTarget;
    const f32 tAngle = sinf((m_timer + m_targetOffset)/getTemplate()->getPatchTargetTangeantRotationFrequency()) * getTemplate()->getPatchTargetTangeantRotationOffset().Radians();
    const Vec2d tP1 = tP0 + dir.Rotate(-cAngle) * 0.5f* (targetDist - radius);
    const Vec2d tP2 = tP3 -dir.Rotate(-tAngle) * 0.5f * radius;

    Bezier4<Vec2d> targetB(tP0,tP1,tP2,tP3);
    const f32 targetPatchLength = targetB.getLength(100);

    const f32 uvTargetEnd = uvStartEnd + (targetPatchLength / getTemplate()->getPatchTileLength());

    BezierPatchParams params;
    params.m_p0 = tP0;
    params.m_p1 = tP1;
    params.m_p2 = tP2;
    params.m_p3 = tP3;
    params.m_startUV.m_x = uvStartEnd;
    params.m_endUV.m_x = uvTargetEnd;
    params.m_startWidth = getTemplate()->getPatchStartWidth();
    params.m_endWidth = getTemplate()->getPatchTargetWidth();
    params.m_z = m_actor->getDepth();
    params.m_texture = (Texture*)m_texture.getResource();
    params.m_startColor = colorB;
    params.m_endColor = colorA;
    params.m_fogColor = getColorFog();
    params.m_blendMode = getTemplate()->getPatchBlendMode();

    m_targetPatch.drawPatch(params);
}

void Ray_ActivationSequenceVisualComponent::updateAnimInput()
{
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(Occupied,540639817),m_user.isValid());
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(ActivationPercent,2736565110),m_activateTime/m_activateTimeTotal);
}

//*****************************************************************************
// Template
//*****************************************************************************

IMPLEMENT_OBJECT_RTTI(Ray_ActivationSequenceVisualComponent_Template)

BEGIN_SERIALIZATION_CHILD(Ray_ActivationSequenceVisualComponent_Template)
    
    SERIALIZE_MEMBER("patchStartRadius",m_patchStartRadius);
    SERIALIZE_MEMBER("patchTileLength",m_patchTileLength);
    SERIALIZE_MEMBER("patchScrollSpeed",m_patchScrollSpeed);
    SERIALIZE_MEMBER("patchStartTangeantRotationOffset",m_patchStartTangeantRotationOffset);
    SERIALIZE_MEMBER("patchTargetTangeantRotationOffset",m_patchTargetTangeantRotationOffset);
    SERIALIZE_MEMBER("patchStartTangeantRotationFrequency",m_patchStartTangeantRotationFrequency);
    SERIALIZE_MEMBER("patchTargetTangeantRotationFrequency",m_patchTargetTangeantRotationFrequency);
    SERIALIZE_MEMBER("patchStartWidth",m_patchStartWidth);
    SERIALIZE_MEMBER("patchTargetWidth",m_patchTargetWidth);
    SERIALIZE_MEMBER("patchTexture",m_patchTextureFile);
    SERIALIZE_ENUM_BEGIN("patchBlendMode",m_patchBlendMode);
        SERIALIZE_ENUM_VAR(GFX_BLEND_UNKNOWN);
        SERIALIZE_ENUM_VAR(GFX_BLEND_COPY);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADEST);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADESTPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ADD);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ADDALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_SUBALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_SUB);
        SERIALIZE_ENUM_VAR(GFX_BLEND_MUL);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAMUL);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAMUL);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADEST);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADESTPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_MUL2X);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHATOCOLOR);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHATOCOLOR);
    SERIALIZE_ENUM_END();

END_SERIALIZATION()

Ray_ActivationSequenceVisualComponent_Template::Ray_ActivationSequenceVisualComponent_Template()
: m_patchStartRadius(0.1f)
, m_patchTileLength(16.0f)
, m_patchScrollSpeed(0.005f)
, m_patchStartTangeantRotationOffset(btrue,60.0f)
, m_patchTargetTangeantRotationOffset(btrue,30.0f)
, m_patchStartTangeantRotationFrequency(0.9f)
, m_patchTargetTangeantRotationFrequency(0.7f)
, m_patchStartWidth(0.75f)
, m_patchTargetWidth(4.f)
, m_patchBlendMode(GFX_BLEND_ALPHA)
{
}

Ray_ActivationSequenceVisualComponent_Template::~Ray_ActivationSequenceVisualComponent_Template()
{
}

};

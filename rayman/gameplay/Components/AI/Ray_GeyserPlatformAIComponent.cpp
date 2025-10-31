#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GeyserPlatformAIComponent.h"
#endif //_ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_FXBANKCOMPONENT_H_
#include "engine/actors/components/FxBankComponent.h"
#endif //_ITF_FXBANKCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

namespace ITF
{
    template <class T>
    ITF_INLINE void swap( T& _a, T& _b )
    {
        T tmp = _a;
        _a = _b;
        _b = tmp;
    }


    IMPLEMENT_OBJECT_RTTI(Ray_GeyserPlatformAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_GeyserPlatformAIComponent_Template)
        SERIALIZE_MEMBER("attachBone", m_attachBone);
        SERIALIZE_MEMBER("ignoreActorScale", m_ignoreActorScale);

        SERIALIZE_MEMBER("length", m_length);
        SERIALIZE_MEMBER("lockLength", m_lockLength);
        SERIALIZE_MEMBER("lockAngle", m_lockAngle);
        SERIALIZE_MEMBER("growTargetSmoothFactor", m_growTargetSmoothFactor);
        SERIALIZE_MEMBER("growSmoothFactor", m_growSmoothFactor);
        SERIALIZE_MEMBER("shrinkTargetSmoothFactor", m_shrinkTargetSmoothFactor);
        SERIALIZE_MEMBER("shrinkSmoothFactor", m_shrinkSmoothFactor);
        SERIALIZE_MEMBER("lengthVariationAmplitude", m_lengthVariationAmplitude);
        SERIALIZE_MEMBER("lengthVariationFrequency", m_lengthVariationFrequency);

        SERIALIZE_MEMBER("jumpThreshold", m_jumpThreshold);

        SERIALIZE_MEMBER("triggerOnHit", m_triggerOnHit);
        SERIALIZE_MEMBER("triggerOnHitDelay", m_triggerOnHitDelay);
        SERIALIZE_MEMBER("autoCloseDelay", m_autoCloseDelay);
        SERIALIZE_MEMBER("startOpen", m_startOpen);
        SERIALIZE_MEMBER("changeStateOnTrigger", m_changeStateOnTrigger );
        
        SERIALIZE_MEMBER("texture", m_textureFile);
        SERIALIZE_MEMBER("patchStartOffset", m_patchStartOffset);
        SERIALIZE_MEMBER("patchLengthOffset", m_patchLengthOffset);
        SERIALIZE_MEMBER("patchLengthMultiplier", m_patchLengthMultiplier);
        SERIALIZE_MEMBER("patchWidthBottom", m_patchWidthBottom);
        SERIALIZE_MEMBER("patchWidthTop", m_patchWidthTop);
        SERIALIZE_MEMBER("patchSwapXY", m_patchSwapXY);
        SERIALIZE_MEMBER("tileLength", m_tileLength);
        SERIALIZE_MEMBER("tileWidth", m_tileWidth);
        SERIALIZE_MEMBER("uvScrollSpeed", m_uvScrollSpeed);
        SERIALIZE_UVMODE("uvMode",m_uvMode);
        SERIALIZE_MEMBER("patchFadeInLength", m_patchFadeInLength);
        SERIALIZE_MEMBER("patchFadeOutLength", m_patchFadeOutLength);
        SERIALIZE_MEMBER("zOffset", m_zOffset);
        SERIALIZE_MEMBER("tessellationLength", m_tessellationLength);

        SERIALIZE_OBJECT("forceModifier", m_physModifier);
        SERIALIZE_MEMBER("forceDirection", m_forceDirection);
        SERIALIZE_MEMBER("forceLengthOffset", m_forceLengthOffset);
        SERIALIZE_MEMBER("forceLengthMultiplier", m_forceLengthMultiplier);
        SERIALIZE_MEMBER("forceWidthBottom", m_forceWidthBottom);
        SERIALIZE_MEMBER("forceWidthTop", m_forceWidthTop);
        SERIALIZE_MEMBER("forceFadeOutLength", m_forceFadeOutLength);

        SERIALIZE_MEMBER("usePlatform", m_usePlatform);
        SERIALIZE_MEMBER("platformMaterial", m_platformMaterial);
        SERIALIZE_MEMBER("platformWidth", m_platformWidth);
        SERIALIZE_MEMBER("platformBone", m_platformBone);
        SERIALIZE_MEMBER("platformPolyline", m_platformPolyline);
        SERIALIZE_MEMBER("platformLengthMultiplier", m_platformLengthMultiplier);
        SERIALIZE_MEMBER("platformLengthOffset", m_platformLengthOffset);
        SERIALIZE_MEMBER("platformHeightSink", m_platformHeightSink);

        SERIALIZE_MEMBER("fx", m_fxMain);
        SERIALIZE_MEMBER("fxUseTransform", m_fxUseTransform);
        SERIALIZE_GENERATORTYPE("fxGeneratorType", m_fxGeneratorType);
        SERIALIZE_MEMBER("fxGenerators", m_fxGenerators);
        SERIALIZE_MEMBER("fxLengthOffset", m_fxLengthOffset);
        SERIALIZE_MEMBER("fxLengthMultiplier", m_fxLengthMultiplier);
        SERIALIZE_MEMBER("fxWidth", m_fxWidth);
        SERIALIZE_MEMBER("fxLifetimeOffset", m_fxLifetimeOffset);
        SERIALIZE_MEMBER("fxLifetimeMultiplier", m_fxLifetimeMultiplier);
        SERIALIZE_MEMBER("fxCollision", m_fxCollision);
        SERIALIZE_MEMBER("fxOnClose", m_fxOnClose);
        SERIALIZE_MEMBER("fxOnCloseThreshold", m_fxOnCloseThreshold);

        SERIALIZE_MEMBER("animOpened", m_animOpened);
        SERIALIZE_MEMBER("animClosed", m_animClosed);

        SERIALIZE_MEMBER("useStim", m_useStim);
        SERIALIZE_MEMBER("jumpToPlatform", m_jumpToPlatform);
        SERIALIZE_MEMBER("faction", m_faction);
        SERIALIZE_MEMBER("windTunnelLevel", m_windTunnelLevel);
        SERIALIZE_CONTAINER_OBJECT("regions",m_regions);
        SERIALIZE_MEMBER("isGeyserBehavior", m_geyserBehavior);
        SERIALIZE_MEMBER("registerToAIManager", m_registerToAIManager);


        BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
#ifndef ITF_CONSOLE_FINAL

        SERIALIZE_MEMBER("minAngle", m_minAngle);
        SERIALIZE_MEMBER("maxAngle", m_maxAngle);

        SERIALIZE_MEMBER("debugForce", m_debugForce);
        SERIALIZE_MEMBER("debugPatch", m_debugPatch);
        SERIALIZE_MEMBER("debugFx", m_debugFx);
#endif // ITF_CONSOLE_FINAL
        END_CONDITION_BLOCK()

    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_GeyserPlatformAIComponent_Template, RegionData)
        SERIALIZE_MEMBER("widthOffset", m_widthOffset);
        SERIALIZE_MEMBER("heightOffset", m_heightOffset);
        SERIALIZE_MEMBER("id", m_id);
        SERIALIZE_MEMBER("gameMaterial", m_gameMaterial);
    END_SERIALIZATION()

    Ray_GeyserPlatformAIComponent_Template::Ray_GeyserPlatformAIComponent_Template()
        : m_ignoreActorScale(bfalse)
        , m_length(10.f)
        , m_lockLength(bfalse)
        , m_lockAngle(btrue)
        , m_growTargetSmoothFactor(0.03f)
        , m_growSmoothFactor(0.03f)
        , m_shrinkTargetSmoothFactor(0.03f)
        , m_shrinkSmoothFactor(0.03f)
        , m_lengthVariationAmplitude(0.f)
        , m_lengthVariationFrequency(1.f)

        , m_jumpThreshold(0.5f)

        , m_triggerOnHit(bfalse)
        , m_triggerOnHitDelay(0.f)
        , m_autoCloseDelay(0.f)
        , m_startOpen(btrue)
        , m_changeStateOnTrigger(btrue)

        , m_usePlatform(bfalse)
        , m_platformWidth(1.f)
        , m_platformLengthMultiplier(1.f)
        , m_platformLengthOffset(0.f)
        , m_platformHeightSink(2.f)

        , m_patchStartOffset(0.f)
        , m_patchLengthOffset(0.f)
        , m_patchLengthMultiplier(1.f)
        , m_patchWidthBottom(1.f)
        , m_patchWidthTop(2.f)
        , m_patchSwapXY(bfalse)
        , m_tileLength(0.f) // 0 = auto
        , m_tileWidth(0.f)  // 0 = auto
        , m_uvScrollSpeed(0.f, 0.f)
        , m_uvMode(UV_MODE_OPTIMUM)
        , m_patchFadeInLength(2.f)
        , m_patchFadeOutLength(2.f)
        , m_zOffset(0.f)
        , m_tessellationLength(1.f)
        , m_tessellationRatio(0.f)

        , m_forceDirection(Vec2d::Right)
        , m_forceLengthOffset(0.f)
        , m_forceLengthMultiplier(1.f)
        , m_forceWidthBottom(1.f)
        , m_forceWidthTop(2.f)
        , m_forceFadeOutLength(0.f)

        , m_fxUseTransform(bfalse)
        , m_fxGeneratorType(GeneratorType_Box)
        , m_fxLengthOffset(0.f)
        , m_fxLengthMultiplier(1.f)
        , m_fxWidth(1.f)
        , m_fxLifetimeOffset(0.f)
        , m_fxLifetimeMultiplier(1.f)
        , m_fxOnCloseThreshold(0.1f)

        , m_biggestWidth(0.f)
        , m_useStim(bfalse)
        , m_jumpToPlatform(bfalse)
        , m_faction(FACTION_UNKNOWN)

        , m_windTunnelLevel(U32_INVALID)
        , m_geyserBehavior(bfalse)
        , m_registerToAIManager(btrue)

#ifndef ITF_CONSOLE_FINAL
        , m_debugForce(btrue)
        , m_debugPatch(bfalse)
        , m_debugFx(bfalse)
#endif // ITF_CONSOLE_FINAL
    {
    }

    void Ray_GeyserPlatformAIComponent_Template::onTemplateLoaded()
    {
        Super::onTemplateLoaded();

        m_biggestWidth = Max(m_biggestWidth, m_platformWidth);

        m_biggestWidth = Max(m_biggestWidth, m_patchWidthBottom);
        m_biggestWidth = Max(m_biggestWidth, m_patchWidthTop);

        m_biggestWidth = Max(m_biggestWidth, m_forceWidthBottom);
        m_biggestWidth = Max(m_biggestWidth, m_forceWidthTop);

        if (m_tessellationLength > 0)
        {
            m_tessellationRatio = f32_Inv(m_tessellationLength);
        }
    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_GeyserPlatformAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_GeyserPlatformAIComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("startOpen", m_startOpen);
            SERIALIZE_MEMBER("platformHeight", m_length_instance);
            SERIALIZE_MEMBER("angle", m_angleOffset_instance);
            SERIALIZE_FUNCTION(postLoadProperties, ESerialize_PropertyEdit_Load);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_GeyserPlatformAIComponent)
    END_VALIDATE_COMPONENT()

    Ray_GeyserPlatformAIComponent::Ray_GeyserPlatformAIComponent()
        : m_startOpen(UNINIT_BBOOL)
        , m_state(State_Open)
        , m_angleOffset_instance(UNINIT_F32)
        , m_length_instance(UNINIT_F32)
        , m_length_target(0.f)
        , m_length_current(0.f)
        , m_lengthRatio(0.f)
        , m_length(0.f)
        , m_prevLength(0.f)
        , m_timer(0.f)
        , m_fxController(NULL)
        , m_fxDirty(btrue)
        , m_geyserFxHandle(U32_INVALID)
        , m_registered(bfalse)
        , m_animComponent(NULL)
        , m_attachBoneIndex(U32_INVALID)
        , m_platformBoneIndex(U32_INVALID)
        , m_polylineComponent(NULL)
        , m_windTunnelStartPoint(Vec2d::Zero)
        , m_windTunnelExitPoint(Vec2d::Zero)
        , m_triggerOnHitTimer(0.f)
        , m_autoCloseTimer(0.f)
        , m_patchStart(Vec2d::Zero)
        , m_patchEnd(Vec2d::Zero)
        , m_patchNormal(Vec2d::Zero)
    {
        m_physModifierPoints.resize(m_physModifierPoints.capacity());
    }

    Ray_GeyserPlatformAIComponent::~Ray_GeyserPlatformAIComponent()
    {
        clear();
    }

    void Ray_GeyserPlatformAIComponent::clear()
    {
        if (!m_actor)
        {
            return;
        }

        if (m_texture.isValidResourceId())
        {
            m_actor->removeResource(m_texture);
            m_texture.invalidateResourceId();
        }

        registerObjects(bfalse);

        u32 numRegions = m_regions.size();

        for ( u32 i = 0; i < numRegions; i++ )
        {
            SF_DEL(m_regions[i]);
        }

        m_regions.clear();
    }

    bbool Ray_GeyserPlatformAIComponent::useForce() const
    {
        return !getTemplate()->getUseStim() && !getTemplate()->getJumpToPlatform() && getTemplate()->getWindTunnelLevel() == U32_INVALID;
    }

    bbool Ray_GeyserPlatformAIComponent::useWindTunnel() const
    {
        return getTemplate()->getWindTunnelLevel() != U32_INVALID && getTemplate()->getPlatformPolyline().isValid();
    }

    bbool Ray_GeyserPlatformAIComponent::useReachPlatform() const
    {
        return getTemplate()->getJumpToPlatform() && getTemplate()->getPlatformPolyline().isValid();
    }

    void Ray_GeyserPlatformAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_fxController = m_actor->GetComponent<FXControllerComponent>();
        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        m_polylineComponent = m_actor->GetComponent<PolylineComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryCanJumpFromCurrent,1690201300), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryWindTunnelInfo,3603812346), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryGeyserBehavior,2840355382), this);
        if (getTemplate()->getTriggerOnHit())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796), this);
        }

        m_actor->setObjectGroupIndex(ObjectGroup::GroupIndex_Collision);


        // ProceduralPolyline
        if (getTemplate()->getUsePlatform())
        {
            m_polyline.init(m_actor, this);
            if ( getTemplate()->getPlatformMaterial().getStringID() != GameMaterial_Template::nullMatName.getStringID() )
            {
                World::addGameMaterialFromFile(getTemplate()->getPlatformMaterial(), m_actor->getResourceGroup());
                m_polyline.setGameMaterial(getTemplate()->getPlatformMaterial());
            }
            m_polylinePoints.resize(2);
        }


        // ProceduralPatch
        if (!getTemplate()->getTextureFile().isEmpty())
        {
            m_texture = m_actor->addResource(Resource::ResourceType_Texture,getTemplate()->getTextureFile());
        }


        if ( needUpdatePhysForceModifier() )
        {
            // PhysModifier
            m_physModifier.setScale(getScale());
            m_physModifier.setTemplate(&getTemplate()->getPhysModifier());
        }


        if ( getTemplate()->getLockLength() ||
             m_length_instance == UNINIT_F32 )
        {
            m_length_instance = getTemplate()->getLength();
        }

        if ( getTemplate()->getLockAngle() ||
             m_angleOffset_instance == UNINIT_F32 )
        {
            m_angleOffset_instance = getTemplate()->getAngleOffset();
        }


        if (m_startOpen == UNINIT_BBOOL)
        {
            m_startOpen = getTemplate()->getStartOpen();
        }

        if (m_startOpen)
        {
            forceOpen();
        }
        else
        {
            forceClose();
        }


        createRegions();

        // init AABB
        updatePatch(0);
        growAABB();
    }

    void Ray_GeyserPlatformAIComponent::postLoadProperties()
    {
        if ( getTemplate()->getLockLength() ||
             m_length_instance == UNINIT_F32 )
        {
            m_length_instance = getTemplate()->getLength();
        }

        if (m_startOpen)
        {
            forceOpen();
        }
        else
        {
            forceClose();
        }
    }

    void Ray_GeyserPlatformAIComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            if ( getTemplate()->getChangeStateOnTrigger() )
            {
                if (m_state == State_Closed)
                {
                    open();
                }
                else
                {
                    close();
                }
            }
        }
        else if ( EventGeneric* eventGeneric = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)) )
        {
            if (eventGeneric->getId() == ITF_GET_STRINGID_CRC(Open,812432275))
            {
                open();
            }
            else if (eventGeneric->getId() == ITF_GET_STRINGID_CRC(Close,3883599349))
            {
                close();
            }
        }
        else if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374)))
        {
            if ( needUpdatePhysForceModifier() )
            {
                m_physModifier.setScale(getScale());
            }

            m_fxDirty = btrue;
        }
        else if ( Ray_EventQueryCanJumpFromCurrent* query = _event->DynamicCast<Ray_EventQueryCanJumpFromCurrent>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanJumpFromCurrent,1690201300)) )
        {
            processQueryCanJump(query);
        }
        else if ( Ray_EventQueryWindTunnelInfo* queryWindTunnel = _event->DynamicCast<Ray_EventQueryWindTunnelInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryWindTunnelInfo,3603812346)))
        {
            processQueryWindTunnelInfo(queryWindTunnel);
        }
        else if ( Ray_EventQueryGeyserBehavior* queryGeyserBehavior = _event->DynamicCast<Ray_EventQueryGeyserBehavior>(ITF_GET_STRINGID_CRC(Ray_EventQueryGeyserBehavior,2840355382)))
        {
            queryGeyserBehavior->setHasGeyserBehavior(getTemplate()->getIsGeyserBehavior());
        }
        else if ( HitStim* hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)))
        {
            processHit(hit);
        }
        else
        {
            if (getTemplate()->getUsePlatform())
            {
                m_polyline.onEvent(_event);
            }
        }
    }

    void Ray_GeyserPlatformAIComponent::forceOpen()
    {
        open();

        m_length = m_length_target = m_length_current = m_length_instance;
        m_lengthRatio = 1.f;
        m_prevLength = m_length;
    }

    void Ray_GeyserPlatformAIComponent::forceClose()
    {
        close();

        m_length = m_length_target = m_length_current = 0.f;
        m_lengthRatio = 0.f;
        m_prevLength = m_length;
    }

    void Ray_GeyserPlatformAIComponent::open()
    {
        if (m_state == State_Open)
        {
            return;
        }

        m_state = State_Open;
        m_fxDirty = btrue;
        m_autoCloseTimer = 0.f;

        if (m_animComponent && getTemplate()->getAnimOpened().isValid())
        {
            m_animComponent->setAnim(getTemplate()->getAnimOpened());
        }
    }

    void Ray_GeyserPlatformAIComponent::close()
    {
        if (m_state == State_Closed)
        {
            return;
        }

        u32 numActorsJumping = m_jumpToActors.size();

        if ( numActorsJumping )
        {
            ObjectRef polyRef;

            if ( getReachPlatformData(polyRef) )
            {
                Ray_EventJumpToPos jumpToPos(polyRef,U32_INVALID,0.f);

                jumpToPos.setStop(btrue);

                for ( u32 i = 0; i < numActorsJumping; i++ )
                {
                    Actor* jumpingActor = m_jumpToActors[i].getActor();

                    if ( jumpingActor )
                    {
                        jumpingActor->onEvent(&jumpToPos);
                    }
                }
            }
        }

        m_state = State_Closed;
        m_fxDirty = btrue;

        if (m_animComponent && getTemplate()->getAnimClosed().isValid())
        {
            m_animComponent->setAnim(getTemplate()->getAnimClosed());
        }
    }

    Vec2d Ray_GeyserPlatformAIComponent::getScale() const
    {
        if (getTemplate()->getIgnoreActorScale())
        {
            return Vec2d::One;
        }
        else
        {
            return m_actor->getScale();
        }
    }

    Transform2d Ray_GeyserPlatformAIComponent::getWorldTransform2d( const bbool _useAngleOffset /*= btrue*/ ) const
    {
        Vec2d pos;
        f32 angle;

        AnimBoneDyn* bone = NULL;
        if (m_animComponent && m_attachBoneIndex != U32_INVALID)
        {
            bone = m_animComponent->getBone(m_attachBoneIndex);
        }

        if (bone)
        {
            pos = bone->m_Pos;
            angle = bone->getAngle();
            if (m_actor->isFlipped())
            {
                angle += MTH_PI;
            }
        }
        else
        {
            pos = m_actor->get2DPos();
            angle = m_actor->getAngle();
        }

        Transform2d tmp(Vec2d::Zero, angle, m_actor->isFlipped());
        pos += tmp.transformPos(getTemplate()->getPosOffset());

        if (_useAngleOffset)
        {
            angle += m_actor->isFlipped() ? -m_angleOffset_instance : m_angleOffset_instance;
        }

        return Transform2d(pos, angle, m_actor->isFlipped());
    }

    // _useFlip: true, get 'real' local position (local to the flipped actor).
    //           false: get the position local to the actor as if he was unflipped.
    Transform2d Ray_GeyserPlatformAIComponent::getLocalTransform2d( const bbool _useFlip /*= btrue*/ ) const
    {
        Vec2d pos(Vec2d::Zero);
        f32 angle = 0.f;

        const bbool counterFlip = !_useFlip && m_actor->isFlipped();

        if (m_animComponent && m_attachBoneIndex != U32_INVALID)
        {
            if ( AnimBoneDyn* bone = m_animComponent->getBone(m_attachBoneIndex) )
            {
                const Transform2d& actorTransform = ITF::getWorldTransform2d(m_actor, _useFlip);
                pos = actorTransform.inverseTransformPos(bone->m_Pos);
                angle = actorTransform.inverseTransformAngle(bone->getAngle());
                if (counterFlip)
                {
                    angle += MTH_PI;
                }
            }
        }

        if (!counterFlip)
        {
            pos += getTemplate()->getPosOffset();
            angle += m_angleOffset_instance;
        }
        else
        {
            pos.m_x -= getTemplate()->getPosOffset().m_x;
            pos.m_y += getTemplate()->getPosOffset().m_y;
            angle += MTH_PI - m_angleOffset_instance;
        }

        return Transform2d(pos, angle, bfalse);
    }

    Transform2d Ray_GeyserPlatformAIComponent::getLocalTransform2d_old() const
    {
        Vec2d pos;
        f32 angle;
        getPosAndAngleLocal(pos, angle);
        return Transform2d(pos, angle, bfalse);
    }

    void Ray_GeyserPlatformAIComponent::getPosAndAngleLocal( Vec2d& _pos, f32& _angle ) const
    {
        AnimBoneDyn* bone = NULL;
        if (m_animComponent && m_attachBoneIndex != U32_INVALID)
        {
            bone = m_animComponent->getBone(m_attachBoneIndex);
        }

        if (bone)
        {
            _pos = bone->m_Pos - m_actor->get2DPos();
            _pos = _pos.Rotate(-m_actor->getAngle());
            _angle = bone->getAngle() - m_actor->getAngle();
            _angle += m_actor->isFlipped() ? -m_angleOffset_instance : m_angleOffset_instance;
        }
        else
        {
            _pos = getTemplate()->getPosOffset();
            _angle = m_angleOffset_instance;
            if (m_actor->isFlipped())
            {
                _angle += MTH_PI;
            }
        }
    }

    f32 Ray_GeyserPlatformAIComponent::getForceLength() const
    {
        if (m_state == State_Open)
        {
            return m_length * getTemplate()->getForceLengthMultiplier() + getTemplate()->getForceLengthOffset();
        }

        return 0.f;
    }

    Vec3d Ray_GeyserPlatformAIComponent::getTarget() const
    {
        // DON'T scale unless length is locked, because m_length_instance is given already scaled
        const f32 scale = (getTemplate()->getLockLength() && !getTemplate()->getIgnoreActorScale()) ? m_actor->getScale().m_x : 1.f;
 
        const Vec2d localPos = Vec2d(m_length_instance * scale, 0).Rotate(m_angleOffset_instance);
        const Transform3d& transform = getTransform();
        return transform.transformPos(localPos);
    }

    void Ray_GeyserPlatformAIComponent::setTarget( const Vec3d& _worldPos )
    {
        if ( getTemplate()->getLockAngle() && getTemplate()->getLockLength() )
        {
            return;
        }

        const Transform3d& transform = getTransform();
        Vec2d localPos = transform.inverseTransformPos(_worldPos).truncateTo2D();

        if ( getTemplate()->getLockAngle() )
        {
            // if angle is locked then length isn't (tested above)
            localPos = localPos.Rotate(-m_angleOffset_instance);
            m_length_instance = Max(0.f, localPos.m_x);
        }
        else
        {
            m_angleOffset_instance = Vec2d(localPos).normalize().getAngle();
            m_length_instance = localPos.norm();
        }


        if (m_state == State_Open)
        {
            m_length = m_length_target = m_length_current = m_length_instance;
            m_prevLength = m_length;
        }
    }
    

#ifdef ITF_SUPPORT_EDITOR
    void Ray_GeyserPlatformAIComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        updateLength(0);
        updateForceOrWindTunnel(0);
        updatePatch(0);
        updatePolyline(0);
        updateRegions(0);

        m_fxDirty = btrue;
    }

    void Ray_GeyserPlatformAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        m_polyline.drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;


        if (m_state == State_Open)
        {
            if (getTemplate()->getDebugForce())
            {
                drawDebugForce(_drawInterface, _flags);
            }

            if (getTemplate()->getDebugPatch())
            {
                drawDebugPatch(_drawInterface, _flags);
            }

            if (getTemplate()->getDebugFx())
            {
                drawDebugFX();
            }
        }


        validateAngle();
    }

    void Ray_GeyserPlatformAIComponent::drawDebugForce( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        // draw physModifier
        if ( needUpdatePhysForceModifier() )
        {
            m_physModifier.drawDebug(m_actor->get2DPos(), m_actor->getAngle(), m_actor->getDepth());
        }

        // draw windTunnel
        if ( useWindTunnel() )
        {
            DebugDraw::circle(m_windTunnelStartPoint, m_actor->getDepth(), 0.1f, Color::white(), 0.f, "startPoint", 85 * MTH_DEGTORAD);
            DebugDraw::circle(m_windTunnelExitPoint,  m_actor->getDepth(), 0.1f, Color::white(), 0.f, "exitPoint", 85 * MTH_DEGTORAD);
        }

        // draw regions
        u32 numRegions = m_regions.size();
        for ( u32 i = 0; i < numRegions; i++ )
        {
            if ( m_regions[i] )
            {
                _drawInterface->drawPolyLine(m_regions[i], btrue);
            }
        }
    }

    void Ray_GeyserPlatformAIComponent::validateAngle() const
    {
        const f32 min = canonizeAngle(getTemplate()->getMinAngle());
        const f32 max = canonizeAngle(getTemplate()->getMaxAngle());
        if (min != max)
        {
            const f32 angle = canonizeAngle(m_actor->getAngle());
            //bbool valid = (min < max) ?
            //    ((min <= angle) && (angle <= max)) :
            //    ((max <= angle) && (angle <= min)) ;
            bbool valid;    // babysitting Coverity...
            if (min < max)
                valid = (min <= angle) && (angle <= max);
            else
                valid = (max <= angle) && (angle <= min);

            if (!valid)
            {
                DebugDraw::text(m_actor->getPos(), Vec2d(0, 30), Color::red(), Color(0.5f,1,1,1), " Invalid angle for this GPE");
            }

            //static const String8 s_AngleError = "invalid angle for this GPE";
            //if (m_actor->getDataErrorLabel() == s_AngleError)
            //{
            //    if (valid)
            //    {
            //        m_actor->resetDataError();
            //    }
            //}
            //else
            //{
            //    ITF_WARNING(m_actor, valid, s_AngleError.cStr());
            //}
        }
    }

    Vec3d Ray_GeyserPlatformAIComponent::editor_getTarget() const
    {
        return getTarget();
    }

    void Ray_GeyserPlatformAIComponent::editor_setTarget( const Vec3d& _worldPos )
    {
        setTarget( _worldPos );
        
        updateLength(0);
        updateForceOrWindTunnel(0);
        updatePatch(0);
        updatePolyline(0);
    }
    
#endif // ITF_SUPPORT_EDITOR

    Transform3d Ray_GeyserPlatformAIComponent::getTransform() const
    {
        const Transform2d& transform = getWorldTransform2d(bfalse);
        const f32 z = m_actor->getDepth();
        return Transform3d( transform.getPos().to3d(z),
            transform.getAngle(),
            m_actor->isFlipped() );
    }

    void Ray_GeyserPlatformAIComponent::growAABB()
    {
        // We assume that the patch is always bigger than the force,
        // so we use it to grow our AABB.

        const f32 width = !getTemplate()->getPatchSwapXY() ?
            getTemplate()->getBiggestWidth() * getScale().m_x :
            m_length * getTemplate()->getPatchLengthMultiplier() + getTemplate()->getPatchLengthOffset() - getTemplate()->getPatchStartOffset();    // no need to scale here because m_length is
        const Vec2d biggestHalfWidth = m_patchNormal * width * 0.5f;

        AABB aabb(m_patchStart - biggestHalfWidth);
        aabb.grow(m_patchStart + biggestHalfWidth);
        aabb.grow(m_patchEnd - biggestHalfWidth);
        aabb.grow(m_patchEnd + biggestHalfWidth);
        m_actor->growRelativeAABBFromAbsolute(aabb);

#ifndef ITF_FINAL
        //const f32 z = m_actor->getDepth();

        //DebugDraw::AABB(Vec2d::Zero, z, 0.f, aabb, Color::black(), 0, 3);

        //DebugDraw::circle(m_patchStart, z, 0.1f, Color::green());
        //DebugDraw::circle(m_patchStart, z, 0.2f, Color::green());
        //DebugDraw::circle(m_patchStart, z, 0.3f, Color::green());
        //DebugDraw::circle(m_patchEnd, z, 0.1f, Color::red());
        //DebugDraw::circle(m_patchEnd, z, 0.2f, Color::red());
        //DebugDraw::circle(m_patchEnd, z, 0.3f, Color::red());

        //DebugDraw::line2D(m_patchStart - biggestHalfWidth, m_patchStart + biggestHalfWidth, z, Color::red());
        //DebugDraw::line2D(m_patchEnd - biggestHalfWidth, m_patchEnd + biggestHalfWidth, z, Color::green());
#endif // ITF_FINAL
    }

    void Ray_GeyserPlatformAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        m_timer += _dt;

        m_prevLength = m_length;

        updateUsers(_dt);

        updateLength(_dt);      // must be done first

        updateForceOrWindTunnel(_dt);

        updateStim(_dt);

        updateRegions(_dt);

        updatePatch(_dt);

        updatePolyline(_dt);    // must be done after updatePatch()

        updateFX(_dt);

        updateAutoClose(_dt);

        growAABB();    // must be done after updatePatch()
    }

    void Ray_GeyserPlatformAIComponent::updateForceOrWindTunnel( f32 _dt )
    {
        if ( useWindTunnel() )
        {
            updateWindTunnel(_dt);
        }

        if ( useForce() )
        {
            updateForce(_dt);
        }

        if ( useReachPlatform() )
        {
            updateReachPlatform(_dt);
        }
    }

    void Ray_GeyserPlatformAIComponent::updateLength( f32 _dt )
    {
        const f32 maxLength = m_length_instance;

        f32 finalTargetLength = 0.f;
        f32 sinLength = 0.f;
        f32 targetSmoothFactor;
        f32 smoothFactor;

        if (m_state == State_Open)
        {
            targetSmoothFactor = getTemplate()->getGrowTargetSmoothFactor();
            smoothFactor = getTemplate()->getGrowSmoothFactor();


            finalTargetLength = maxLength;

            u32 numObjectsOnPlatform = 0;
            if (getTemplate()->getUsePlatform())
            {
                numObjectsOnPlatform = m_polyline.getObjectsOnPolyline().size();
            }
            else if (m_platformBoneIndex != U32_INVALID &&
                     m_polylineComponent != NULL)
            {
                numObjectsOnPlatform = m_polylineComponent->getUserCount(getTemplate()->getPlatformPolyline());
            }

            if (numObjectsOnPlatform > 0)
            {
                finalTargetLength -= getTemplate()->getPlatformHeightSink();
            }


            sinLength = f32_Sin(m_timer * getTemplate()->getLengthVariationFrequency()) * getTemplate()->getLengthVariationAmplitude();
        }
        else
        {
            targetSmoothFactor = getTemplate()->getShrinkTargetSmoothFactor();
            smoothFactor = getTemplate()->getShrinkSmoothFactor();
        }

        if (_dt)    // for onEditorMove
        {
            m_length_target += (finalTargetLength - m_length_target) * targetSmoothFactor;
            m_length_current += (m_length_target - m_length_current) * smoothFactor;
            m_lengthRatio = m_length_current / (finalTargetLength ? finalTargetLength : maxLength);
        }

        // DON'T scale unless length is locked, because m_length_instance is given already scaled
        f32 mainLength = m_length_current;
        if (getTemplate()->getLockLength() && !getTemplate()->getIgnoreActorScale())
        {
            mainLength *= m_actor->getScale().m_x;
        }

        m_length = (mainLength + sinLength);  // sinLength is not scaled
    }

    void Ray_GeyserPlatformAIComponent::updateForce( f32 _dt )
    {
        if ( m_physModifier.getTemplate()->getType() != PhysForceModifier::TYPE_POLYLINE )
        {
            return;
        }

        // NOTE: the PhysForceModifier works in actor space (has its own offset),
        // and doesn't use the actor's scale for TYPE_POLYLINE
        // -> compute everything in actor space!
        // BUT, still take the flip into account.

        const f32 forceLength = getForceLength();
        const Transform2d& localTransform = getLocalTransform2d(bfalse);    // we want to flip our points, but express them in the unflipped-actor-space

        const Vec2d start = localTransform.getPos();
        const Vec2d end = localTransform.transformPos(Vec2d(forceLength, 0.f));
        const Vec2d halfNormalScaled = localTransform.transformDir(Vec2d(0.f, getScale().m_y * 0.5f));


        const Vec2d botHalfWidth = halfNormalScaled * getTemplate()->getForceWidthBottom();
        const Vec2d topHalfWidth = halfNormalScaled * getTemplate()->getForceWidthTop();

        m_physModifierPoints[0] = start + botHalfWidth;
        m_physModifierPoints[1] = end + topHalfWidth;
        m_physModifierPoints[2] = end - topHalfWidth;
        m_physModifierPoints[3] = start - botHalfWidth;

        PhysForceModifier_Instance::PolylineData& data = m_physModifier.getPolyline();
        data.m_polygon.setPoints(m_physModifierPoints);

        data.m_aabb = AABB(m_physModifierPoints[0]);
        for (u32 i = 1; i < m_physModifierPoints.size(); i++)
        {
            data.m_aabb.grow(m_physModifierPoints[i]);
        }

        data.m_dir = localTransform.transformDir(getTemplate()->getForceDirection());
        data.m_center = (start + end) * 0.5f;   // only used for radial forces
        data.m_start = start.norm();
        data.m_end = data.m_start + forceLength;

        const f32 forceFadeOutLength = getTemplate()->getForceFadeOutLength();
        if (forceFadeOutLength)
        {
            f32 gradient = 0.f;
            if (forceLength > forceFadeOutLength)
            {
                gradient = 1 - forceFadeOutLength / forceLength;
            }

            gradient = Max(0.01f, gradient);    // don't leave at 0 or the physModifier will filter it out
            m_physModifier.setGradientPercent(gradient);
        }
    }

    void Ray_GeyserPlatformAIComponent::updateWindTunnel( f32 _dt )
    {
        updateForce(_dt);

        FixedArray <ActorRef,32> actorsThisFrame;
        const f32 forceLength = getForceLength();
        f32 angle = 0.0f;
        Vec2d start;

        getPosAndAngleLocal(start,angle);

        Vec2d end = Vec2d(forceLength, 0.f);
        end = end.Rotate(angle);
        end += start;

        m_windTunnelStartPoint = (start.Rotate(m_actor->getAngle())) + m_actor->get2DPos();
        m_windTunnelExitPoint = (end.Rotate(m_actor->getAngle())) + m_actor->get2DPos();

        BounceStim bounceStim;
        bounceStim.setSender(m_actor->getRef());
        bounceStim.setBounceType(BOUNCETYPE_WINDTUNEL);
        bounceStim.setBounceDir(Vec2d::Right.Rotate(angle+m_actor->getAngle()));
        bounceStim.setIsRadial(bfalse);
        bounceStim.setMultiplier(1.f);

        // get collisions
        const PhysForceModifier_Instance::PolylineData& data = m_physModifier.getPolyline();
        PhysContactsContainer contacts;
        PHYSWORLD->collidePhantoms(m_actor->get2DPos(), m_actor->get2DPos(), m_actor->getAngle(),
            &data.m_polygon, m_actor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
            contacts);

        const u32 contactCount = contacts.size();

        for ( u32 i = 0; i < contactCount; i++ )
        {
            ActorRef otherActorRef = contacts[i].m_collidableUserData;

            if (otherActorRef == m_actor->getRef())
            {
                continue;
            }

            Actor* otherActor = otherActorRef.getActor();
            if (!otherActor)
            {
                continue;
            }

            if ( actorsThisFrame.find(otherActorRef) == -1 )
            {
                if ( m_windTunnelsActorsInside.find(otherActorRef) == -1 )
                {
                    otherActor->onEvent(&bounceStim);
                }

                actorsThisFrame.push_back(otherActorRef);
            }
        }

        m_windTunnelsActorsInside.clear();

        u32 numActors = actorsThisFrame.size();

        for ( u32 i = 0; i < numActors; i++ )
        {
            m_windTunnelsActorsInside.push_back(actorsThisFrame[i]);
        }
    }

    void Ray_GeyserPlatformAIComponent::updateReachPlatform( f32 _dt )
    {
        updateForce(_dt);

        FixedArray <ActorRef,32> actorsThisFrame;
        FixedArray <Vec2d,4> points;

        const PhysForceModifier_Instance::PolylineData& data = m_physModifier.getPolyline();

        // get collisions
        PhysContactsContainer contacts;
        PHYSWORLD->collidePhantoms(m_actor->get2DPos(), m_actor->get2DPos(), m_actor->getAngle(),
            &data.m_polygon, m_actor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
            contacts);

        const u32 contactCount = contacts.size();

        if ( contactCount > 0 )
        {
            ObjectRef polyRef;

            for ( u32 i = 0; i < contactCount; i++ )
            {
                ActorRef otherActorRef = contacts[i].m_collidableUserData;

                if (otherActorRef == m_actor->getRef())
                {
                    continue;
                }

                Actor* otherActor = otherActorRef.getActor();
                if (!otherActor)
                {
                    continue;
                }

                if ( getReachPlatformData(polyRef) )
                {
                    Ray_EventJumpToPos jumpEvent(polyRef,U32_INVALID,0.f);

                    jumpEvent.setSender(m_actor->getRef());
                    otherActor->onEvent(&jumpEvent);

                    if ( m_jumpToActors.find(otherActorRef) == -1 )
                    {
                        m_jumpToActors.push_back(otherActorRef);
                    }
                }
            }
        }
    }

    bbool Ray_GeyserPlatformAIComponent::getReachPlatformData( ObjectRef& _polyRef ) const
    {
        if ( !m_polylineComponent )
        {
            return bfalse;
        }

        const PolyLine* poly = m_polylineComponent->getPolylineFromAnimRef(getTemplate()->getPlatformPolyline());

        if ( !poly )
        {
            return bfalse;
        }

        _polyRef = poly->getRef();

        return btrue;
    }

    void Ray_GeyserPlatformAIComponent::updatePatch( f32 _dt )
    {
        const Vec2d& scale = getScale();
        const Transform2d& transform = getWorldTransform2d();

        const Vec2d dir = transform.transformDir(Vec2d::Right);
        const Vec2d perp = dir.getPerpendicular();
        const f32 patchLength = m_length * getTemplate()->getPatchLengthMultiplier() + getTemplate()->getPatchLengthOffset() - getTemplate()->getPatchStartOffset();

        if (!getTemplate()->getPatchSwapXY())
        {
            m_patchStart = transform.getPos() + dir * getTemplate()->getPatchStartOffset();
            m_patchEnd = m_patchStart + dir * patchLength;
            m_patchNormal = perp;
        }
        else
        {
            const f32 patchWidth = Max(getTemplate()->getPatchWidthBottom(), getTemplate()->getPatchWidthTop()) * scale.m_y;
            m_patchStart = transform.getPos() + dir * getTemplate()->getPatchStartOffset() + dir * patchLength * 0.5f - perp * patchWidth * 0.5f;
            m_patchEnd = m_patchStart + perp * patchWidth;
            m_patchNormal = dir;
        }
    }

    void Ray_GeyserPlatformAIComponent::updatePolyline( f32 _dt )
    {
        if (getTemplate()->getUsePlatform())
        {
            updatePolyline_Procedural(_dt);
        }
        else if (m_platformBoneIndex != U32_INVALID)
        {
            updatePolyline_Anim(_dt);
        }
    }

    void Ray_GeyserPlatformAIComponent::updatePolyline_Procedural( f32 _dt )
    {
        if (m_state == State_Open)
        {
            m_polyline.activate();

            const Vec2d platformHalfWidth = m_patchNormal * getTemplate()->getPlatformWidth() * 0.5f;
            m_polylinePoints[0] = m_patchEnd - platformHalfWidth;
            m_polylinePoints[1] = m_patchEnd + platformHalfWidth;
            m_polyline.setPoints(m_polylinePoints);
            m_polyline.update(_dt);
        }
        else
        {
            m_polyline.deactivate();
        }
    }

    void Ray_GeyserPlatformAIComponent::updatePolyline_Anim( f32 _dt )
    {
        AnimBoneDyn* bone = m_animComponent->getBone(m_platformBoneIndex);
        if (!bone)
        {
            ITF_ASSERT(bone);
            return;
        }

        const Vec2d& dir = Vec2d::Right;
        const Vec2d& scale = getScale();

        f32 platformLength = m_length * getTemplate()->getPlatformLengthMultiplier() + getTemplate()->getPlatformLengthOffset();
        platformLength = Max(0.f, platformLength);

        bone->m_PosLocal += dir * platformLength / scale.m_x;   // m_length is scaled, but the local bone pos must be applied unscaled
    }


    void Ray_GeyserPlatformAIComponent::Draw()
    {
        Super::Draw();

        drawPatch();
    }

    void Ray_GeyserPlatformAIComponent::drawPatch()
    {
        if ( !m_texture.isValidResourceId() )
        {
            // no patch needed
            return;
        }

        if ( m_patchStart.IsEqual(m_patchEnd, MTH_EPSILON) )
        {
            // too small to draw anything
            return;
        }


        Vec2d scale = getScale();
        f32 patchLength = m_length * getTemplate()->getPatchLengthMultiplier() + getTemplate()->getPatchLengthOffset() - getTemplate()->getPatchStartOffset();
        f32 patchWidth = getTemplate()->getPatchWidthBottom() * scale.m_y;
        if (getTemplate()->getPatchSwapXY())
        {
            swap(patchLength, patchWidth);
            swap(scale.m_x, scale.m_y);
        }

        BezierPatchParams params;
        params.m_z = m_actor->getDepth() + getTemplate()->getZOffset();
        params.m_texture = (Texture*)m_texture.getResource();


        const Vec2d patchVec = (m_patchEnd - m_patchStart);
        params.m_p0 = m_patchStart;
        params.m_p1 = m_patchStart + patchVec * 0.333f;
        params.m_p2 = m_patchStart + patchVec * 0.667f;
        params.m_p3 = m_patchEnd;


        f32 uvLength = 1.f;
        if (getTemplate()->getTileLength())
        {
            uvLength = patchLength / (getTemplate()->getTileLength() * (getTemplate()->getTileWidth() ? 1.f : patchWidth));
        }

        f32 uvWidth = 1.f;
        if (getTemplate()->getTileWidth())
        {
            uvWidth = patchWidth / (getTemplate()->getTileWidth() * (getTemplate()->getTileLength() ? 1.f : patchLength));
        }

        const Vec2d uvOffset = m_timer * getTemplate()->getUVScrollSpeed();
        params.m_startUV = uvOffset;
        params.m_endUV = uvOffset + Vec2d(uvLength, uvWidth);
        params.m_uvMode = getTemplate()->getUvMode();


        params.m_startWidth = getTemplate()->getPatchWidthBottom() * scale.m_y;
        params.m_endWidth = getTemplate()->getPatchWidthTop() * scale.m_y;
        if (getTemplate()->getPatchSwapXY())
        {
            params.m_startWidth = patchWidth;
            params.m_endWidth = patchWidth;
        }


        Color color = getColorFactor();
        color.m_a *= m_lengthRatio;
        params.m_startColor = params.m_endColor = color;
        params.m_fogColor = getColorFog();


        f32 oneOverPatchLength = f32_Inv(patchLength);
        params.m_alphaLimitA = getTemplate()->getPatchFadeInLength() * oneOverPatchLength; 
        params.m_alphaValueA = 0.f;
        params.m_alphaLimitB = 1 - getTemplate()->getPatchFadeOutLength() * oneOverPatchLength;
        params.m_alphaValueB = 0.f;

        if (params.m_alphaLimitA > params.m_alphaLimitB)
        {
            // fade in+out is longer than patch -> re-distribute
            params.m_alphaLimitA = params.m_alphaLimitB = (params.m_alphaLimitA + params.m_alphaLimitB) * 0.5f;
        }


        if (getTemplate()->getTessellationRatio() > 0)
        {
            params.m_vdivToDraw = (u32)(patchLength * getTemplate()->getTessellationRatio());
        }


        m_patch.drawPatch(params);

#ifndef ITF_FINAL
        //Vec2d dir = (m_patchEnd - m_patchStart);
        //Vec2d perp = (m_patchEnd - m_patchStart).normalize().getPerpendicular();
        //Vec2d fadeInPos = m_patchStart + dir * params.m_alphaLimitA;
        //DebugDraw::line2D(fadeInPos - perp, fadeInPos + perp, m_actor->getDepth(), Color::green(), 5);
        //Vec2d fadeOutPos = m_patchStart + dir * params.m_alphaLimitB;
        //DebugDraw::line2D(fadeOutPos - perp, fadeOutPos + perp, m_actor->getDepth(), Color::red(), 5);

        //DebugDraw::circle(params.m_p0, m_actor->getDepth(), 0.1f, Color::yellow());
        //DebugDraw::circle(params.m_p1, m_actor->getDepth(), 0.1f, Color::yellow());
        //DebugDraw::circle(params.m_p2, m_actor->getDepth(), 0.1f, Color::yellow());
        //DebugDraw::circle(params.m_p3, m_actor->getDepth(), 0.1f, Color::yellow());
#endif // ITF_FINAL
    }

    Vec2d Ray_GeyserPlatformAIComponent::getFxSize() const
    {
        const Vec2d scale = getScale();
        return Vec2d(
            m_length_instance * getTemplate()->getFxLengthMultiplier() + getTemplate()->getFxLengthOffset(),
            getTemplate()->getFxWidth() * scale.m_y);
    }

    Transform3d Ray_GeyserPlatformAIComponent::getFxTransform() const
    {
        if (getTemplate()->getFxUseTransform())
        {
            return getWorldTransform2d().to3d(m_actor->getDepth());
        }
        else
        {
            return ITF::getWorldTransform(m_actor);
        }
    }

    void Ray_GeyserPlatformAIComponent::updateFX( f32 _dt )
    {
        if (!m_fxController)
        {
            return;
        }


        if (m_fxDirty)
        {
            m_fxDirty = bfalse;

            if (m_geyserFxHandle != U32_INVALID)
            {
                m_fxController->stopFX(m_geyserFxHandle);
                m_geyserFxHandle = U32_INVALID;
            }

            if (m_state == State_Open)
            {
                m_geyserFxHandle = m_fxController->playFX(getTemplate()->getFxName());
                ITF_ASSERT(!getTemplate()->getFxName().isValid() || m_geyserFxHandle != U32_INVALID);
                if (m_geyserFxHandle != U32_INVALID)
                {
                    if (getTemplate()->getFxUseTransform())
                    {
                        const Transform3d& transform = getFxTransform();
                        m_fxController->setFXPos(m_geyserFxHandle, transform.getPos());
                        m_fxController->setFXAngle(m_geyserFxHandle, transform.getAngle());
                        //m_fxController->setFXAngle(m_geyserFxHandle, transform.isFlipped() ? transform.getAngle() - MTH_PI : transform.getAngle());     // NOTE: this seems incorrect, yet somehow it more or less works for AlTranquilo (works normal or flipped, but doesn't work if actor has an angle)
                        //m_fxController->setFXAngle(m_geyserFxHandle, transform.transformDir(Vec2d::Right).getAngle());                                // this gives the same result

#ifndef ITF_FINAL
                        //f32 angle = transform.isFlipped() ? transform.getAngle() - MTH_PI : transform.getAngle();
                        //DebugDraw::line3D(transform.getPos(), transform.getPos() + Vec2d::Right.Rotate(angle).to3d(), Color::red(), 5, 1);
                        //DebugDraw::line3D(transform.getPos(), transform.getPos() + transform.transformDir(Vec2d::Right).to3d(), Color::red(), 5, 1);
#endif // ITF_FINAL
                    }

                    initFxGenerators();
                }
            }
        }


        updateFxGenerators();

        updateUsersFX();


        if (m_state == State_Closed)
        {
            if ( m_length < getTemplate()->getFxOnCloseThreshold() &&
                 m_prevLength > getTemplate()->getFxOnCloseThreshold() )
            {
                // we're closing and just got below the threshold
                m_fxController->playFX(getTemplate()->getFxOnClose());
            }
        }
    }


    void Ray_GeyserPlatformAIComponent::initFxGenerators()
    {
        const SafeArray<StringID>& generators = getTemplate()->getFxGenerators();

        switch (getTemplate()->getFxGeneratorType())
        {
        case Ray_GeyserPlatformAIComponent_Template::GeneratorType_Box:
            {
                const Vec2d fxSize = getFxSize();
                if (fxSize.m_x < MTH_EPSILON || fxSize.m_y < MTH_EPSILON)
                {
                    break;
                }

                for (u32 i = 0; i < generators.size(); i++)
                {
                    ITF_ParticleGenerator* generator = m_fxController->getParticleGenerator(m_geyserFxHandle, generators[i]);
                    if (generator)
                    {
                        initFxGenerator_box(generator, fxSize.m_x, fxSize.m_y);
                    }
                }
            }
            break;
        }
    }

    void Ray_GeyserPlatformAIComponent::updateFxGenerators()
    {
        if (m_geyserFxHandle == U32_INVALID)
        {
            return;
        }


        const SafeArray<StringID>& generators = getTemplate()->getFxGenerators();

        switch (getTemplate()->getFxGeneratorType())
        {
        case Ray_GeyserPlatformAIComponent_Template::GeneratorType_Ballistic:
            {
                const Vec2d fxSize = getFxSize();
                if (fxSize.m_x < MTH_EPSILON)
                {
                    break;
                }

                for (u32 i = 0; i < generators.size(); i++)
                {
                    ITF_ParticleGenerator* generator = m_fxController->getParticleGenerator(m_geyserFxHandle, generators[i]);
                    if (generator)
                    {
                        updateFxGenerator_ballistic(generator, fxSize.m_x);
                    }
                }
            }
            break;
        }
    }

    void Ray_GeyserPlatformAIComponent::updateFxGenerator_ballistic( ITF_ParticleGenerator* _generator, const f32 h )
    {
        /*
            http://fr.wikiversity.org/wiki/M%C3%A9canique_du_point/Dynamique#Altitude_maximale_atteinte
            h = v0� sin�(a) / 2g
            v0 = sqrt( 2gh ) / sin(a)

            At the moment, a = 90� (fixed), so:
            v0 = sqrt( 2gh )

            At apex:
                v(t) = v0 - g t = 0
                t = v0 / g
        */

        ParticleGeneratorParameters* params = _generator->getParameters();
        const f32 g = params->getGravity().norm();

        const f32 v0 = f32_Sqrt( 2.f * g * h );
        params->setVelocityNorm(v0);

        const ParticleGeneratorParameters& original = _generator->getTemplate()->getParameters();
        const f32 templateLifetime = original.computeSumPhaseTime();

        f32 lifetime = v0 / g;
        lifetime = getTemplate()->getFxLifetimeOffset() + lifetime * getTemplate()->getFxLifetimeMultiplier();

        params->setLifeTimeMultiplier(lifetime / templateLifetime);
    }

    void Ray_GeyserPlatformAIComponent::initFxGenerator_box( ITF_ParticleGenerator* _generator, const f32 _length, const f32 _width )
    {
        const ParticleGeneratorParameters& tplParams = _generator->getTemplate()->getParameters();
        ParticleGeneratorParameters* params = _generator->getParameters();

        const f32 f = tplParams.getFrequency() / _width;
        params->setFrequency(f);

        const f32 tplV = tplParams.getVelocityNorm();
        const f32 tplT = tplParams.computeSumPhaseTime();
        const f32 t = _length / tplV;
        params->setLifeTimeMultiplier(t / tplT);

        AABB& box = params->getGenBox();
        box.setMin(Vec2d( box.getMin().m_x, _width *  0.5f ));
        box.setMax(Vec2d( box.getMax().m_x, _width * -0.5f ));

        _generator->setMaxParticles(tplParams.getMaxParticles());
    }

    void Ray_GeyserPlatformAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        registerObjects(btrue);
    }

    void Ray_GeyserPlatformAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        registerObjects(bfalse);

        m_geyserFxHandle = U32_INVALID;
        m_fxDirty = btrue;
    }

    void Ray_GeyserPlatformAIComponent::onResourceReady()
    {
        Super::onResourceReady();

        if (m_animComponent)
        {
            m_attachBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getAttachBone());
            m_platformBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getPlatformBone());
        }

        // enforce the anim state (doing it on actorLoaded is too soon)
        if (m_state == State_Open)
        {
            if (m_animComponent && getTemplate()->getAnimOpened().isValid())
            {
                m_animComponent->setAnim(getTemplate()->getAnimOpened());
            }
        }
        else
        {
            if (m_animComponent && getTemplate()->getAnimClosed().isValid())
            {
                m_animComponent->setAnim(getTemplate()->getAnimClosed());
            }
        }
    }

    void Ray_GeyserPlatformAIComponent::processQueryCanJump( Ray_EventQueryCanJumpFromCurrent* _query )
    {
        const f32 forceLength = getForceLength();

        if (m_state == State_Closed || forceLength < MTH_EPSILON)
        {
            return;
        }

        registerUser(_query->getSender());

        if (getTemplate()->getJumpThreshold() >= 1)
        {
            return;
        }

        const Transform2d& transform = getWorldTransform2d();

        const Vec2d queryPos = _query->getPos();
        const Vec2d localPos = transform.inverseTransformPos(_query->getPos());
        const f32 percent = localPos.m_x / forceLength;

        _query->setCanJump(percent > getTemplate()->getJumpThreshold());

#ifdef DEVELOPER_BEN
        ////DebugDraw::circle(queryPos, 0.3f, Color::yellow());
        //Vec2d projectedPos = transform.transformPos(Vec2d(localPos.m_x, 0));
        //DebugDraw::circle(projectedPos, m_actor->getDepth(), 0.3f, _query->getCanJump() ? Color::green() : Color::red());
#endif // DEVELOPER_BEN
    }

    void Ray_GeyserPlatformAIComponent::processQueryWindTunnelInfo( Ray_EventQueryWindTunnelInfo* _query )
    {
        _query->setLevel(getTemplate()->getWindTunnelLevel());
        _query->setStartPoint(m_windTunnelStartPoint);
        _query->setExitPoint(m_windTunnelExitPoint);
    }

    void Ray_GeyserPlatformAIComponent::updateStim( f32 _dt )
    {
        if ( !getTemplate()->getUseStim() )
        {
            return;
        }

        const f32 forceLength = getForceLength();

        f32 angle = 0.0f;
        Vec2d start;
        getPosAndAngleLocal(start,angle);
        Vec2d end = Vec2d(forceLength, 0.f); 
        //end = end.Rotate(angle);
        end += start;
        Vec2d normal = Vec2d(0.f, getScale().m_y * 0.5f);
        //normal = normal.Rotate(angle);
        Vec2d dir = Vec2d::Right.Rotate(angle);

        const Vec2d botHalfWidth = normal * getTemplate()->getForceWidthBottom();
        const Vec2d topHalfWidth = normal * getTemplate()->getForceWidthTop();

        Vec2d points[4];
        points[0] = start + botHalfWidth;
        points[1] = end + topHalfWidth;
        points[2] = end - topHalfWidth;
        points[3] = start - botHalfWidth;
        m_stimShape.setPoints(points,4);

        PunchStim* punch = STIMSMANAGER->requestStim<PunchStim>(&m_stimShape);

        if (punch)
        {
            punch->setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
            punch->setSender(m_actor->getRef());
            punch->setLevel(0);
            punch->setAngle(angle);
            punch->setDepth(m_actor->getDepth());
            punch->setPos(m_actor->get2DPos());
            punch->setPrevPos(m_actor->get2DPos());
            punch->setDirection(dir);
            punch->setFaction(getTemplate()->getFaction());
            STIMSMANAGER->sendStim(punch);
        }
    }

    void Ray_GeyserPlatformAIComponent::createRegions()
    {
        u32 numRegions = getTemplate()->getNumRegions();

        m_regions.resize(numRegions);

        for ( u32 i = 0; i < numRegions; i++ )
        {
            const Ray_GeyserPlatformAIComponent_Template::RegionData& regionData = getTemplate()->getRegion(i);
            PolyLine* poly;

            if ( regionData.m_id.isValid() && regionData.m_gameMaterial.getStringID() != GameMaterial_Template::nullMatName.getStringID() )
            {
                World::addGameMaterialFromFile(regionData.m_gameMaterial, m_actor->getResourceGroup());

                poly = newAlloc(mId_Gameplay, PolyLine);
                poly->setOwnerActor(m_actor);

                for ( u32 i = 0; i < 4; i++ )
                {
                    poly->addPoint(Vec2d::Zero);
                    poly->setGameMaterialAt(regionData.m_gameMaterial.getStringID(),i);
                }

                poly->setLoop(btrue);
            }
            else
            {
                poly = NULL;
            }

            m_regions[i] = poly;
        }
    }

    void Ray_GeyserPlatformAIComponent::updateRegions( f32 _dt )
    {
        const f32 forceLength = getForceLength();

        if (forceLength == 0.0f)
            return;

        f32 angle = 0.0f;
        Vec2d start;
        getPosAndAngleLocal(start,angle);
        Vec2d end = Vec2d(forceLength, 0.f);
        end = end.Rotate(angle);
        end += start;

        start = (start.Rotate(m_actor->getAngle())) + m_actor->get2DPos();
        end = (end.Rotate(m_actor->getAngle())) + m_actor->get2DPos();

        Vec2d regionDir = end - start;

        regionDir.normalize();

        Vec2d regionNorm = regionDir.getPerpendicular();
        u32 numRegions = m_regions.size();
        f32 width = Max(getTemplate()->getForceWidthBottom(),getTemplate()->getForceWidthTop())*(getScale().m_y*0.5f);

        for ( u32 i = 0; i < numRegions; i++ )
        {
            PolyLine* poly = m_regions[i];

            if ( !poly )
            {
                continue;
            }

            const Ray_GeyserPlatformAIComponent_Template::RegionData& regionData = getTemplate()->getRegion(i);
            f32 regionWidth = width + regionData.m_widthOffset;

            Vec2d p0 = start + ( regionNorm * regionWidth ) + ( (-regionDir) * regionData.m_heightOffset );
            Vec2d p1 = end + ( regionNorm * regionWidth ) + ( regionDir * regionData.m_heightOffset );
            Vec2d p2 = end + ( (-regionNorm) * regionWidth ) + ( regionDir * regionData.m_heightOffset );
            Vec2d p3 = start + ( (-regionNorm) * regionWidth ) + ( (-regionDir) * regionData.m_heightOffset );

            poly->setPosAt(p0,0);
            poly->setPosAt(p1,1);
            poly->setPosAt(p2,2);
            poly->setPosAt(p3,3);

            poly->forceRecomputeData();
        }
    }

    void Ray_GeyserPlatformAIComponent::registerObjects( bbool _register )
    {
        if ( m_registered != _register)
        {
            m_registered = _register;

            if ( _register )
            {
                if ( useForce() )
                {
                    PHYSWORLD->addWindForce(m_actor->getRef(), &m_physModifier, m_actor->getDepth());
                }

                u32 numRegions = m_regions.size();

                for ( u32 i = 0; i < numRegions; i++ )
                {
                    PolyLine* poly = m_regions[i];

                    if ( !poly )
                    {
                        continue;
                    }

                    REGIONS_MANAGER->addRegion(m_regions[i]->getRef(), getTemplate()->getRegion(i).m_id, m_actor->getDepth());
                }

                if (getTemplate()->getRegisterToAIManager())
                {
                    AIData * data = AI_MANAGER->addActor(m_actor->getRef(),m_actor->getDepth());
                    if (data)
                        data->m_faction = getTemplate()->getFaction();
                }
            }
            else
            {
                if ( useForce() )
                {
                    PHYSWORLD->removeWindForce(m_actor->getRef(), &m_physModifier);
                }

                u32 numRegions = m_regions.size();

                for ( u32 i = 0; i < numRegions; i++ )
                {
                    PolyLine* poly = m_regions[i];

                    if ( !poly )
                    {
                        continue;
                    }

                    REGIONS_MANAGER->removeRegion(m_regions[i]->getRef());
                }
                if (getTemplate()->getRegisterToAIManager())
                {
                    AI_MANAGER->removeActor(m_actor->getRef(),m_actor->getDepth());
                }
            }
        }
    }

    void Ray_GeyserPlatformAIComponent::registerUser( ActorRef _userActor )
    {
        for (u32 i = 0; i < m_users.size(); i++)
        {
            User& user = m_users[i];
            if (user.m_actor == _userActor)
            {
                user.m_lastFramePresent = CURRENTFRAME;
                return;
            }
        }

        // not found, add
        m_users.push_back(User());
        User& user = m_users.back();
        user.m_actor = _userActor;
        user.m_lastFramePresent = CURRENTFRAME;
    }

    void Ray_GeyserPlatformAIComponent::updateUsers( f32 _dt )
    {
        const u32 frameLimit = CURRENTFRAME - 1;

        for (u32 i = 0; i < m_users.size();)
        {
            User& user = m_users[i];
            if (user.m_lastFramePresent < frameLimit)
            {
                if (m_fxController)
                {
                    m_fxController->stopFX(user.m_fxHandle);
                }

                m_users.eraseNoOrder(i);
            }
            else
            {
                i++;
            }
        }
    }

    void Ray_GeyserPlatformAIComponent::updateUsersFX()
    {
        if (!getTemplate()->getFxCollision().isValid())
        {
            return;
        }

        for (u32 i = 0; i < m_users.size(); i++)
        {
            User& user = m_users[i];

            if (user.m_fxHandle == U32_INVALID)
            {
                user.m_fxHandle = m_fxController->playFX(getTemplate()->getFxCollision());
            }

            Actor* userActor = user.m_actor.getActor();
            ITF_ASSERT(userActor);
            if (userActor)
            {
                m_fxController->setFXPos(user.m_fxHandle, userActor->getPos());
            }
        }
    }
    void Ray_GeyserPlatformAIComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
    {
        Super::onDepthChanged( _oldZ, _newZ);
        

        if (m_registered)
        {
            if (getTemplate()->getRegisterToAIManager())
            {
                AI_MANAGER->changeDepth(m_actor->getRef(),_oldZ,_newZ);
            }
        }
    }

    void Ray_GeyserPlatformAIComponent::processHit( HitStim* _hit )
    {
        if ( !AIUtils::isEnemyHit(_hit, getTemplate()->getFaction()) )
        {
            return;
        }

        if (m_state == State_Closed)
        {
            if (getTemplate()->getTriggerOnHitDelay())
            {
                anticipateOpen();
            }
            else
            {
                open();
            }
        }

        // reset the timer as long as we receive hits;
        // will auto-close after a delay when hits stop.
        m_autoCloseTimer = 0.f;
    }

    void Ray_GeyserPlatformAIComponent::anticipateOpen()
    {
        // NOTE: if this grows ANY more complex, make a proper state,
        // and replace triggerOnHitDelay by openDelay.

        if (m_triggerOnHitTimer == 0.f)
        {
            m_triggerOnHitTimer = getTemplate()->getTriggerOnHitDelay();

            if (m_animComponent && getTemplate()->getAnimOpened().isValid())
            {
                m_animComponent->setAnim(getTemplate()->getAnimOpened());
            }
        }
    }

    void Ray_GeyserPlatformAIComponent::updateAutoClose( f32 _dt )
    {
        if (m_triggerOnHitTimer > 0.f)
        {
            m_triggerOnHitTimer = Max(0.f, m_triggerOnHitTimer - _dt);
            if (m_triggerOnHitTimer == 0.f)
            {
                open();
            }
        }


        if (getTemplate()->getAutoCloseDelay() <= 0)
        {
            return;
        }

        if (m_state == State_Closed)
        {
            return;
        }

        m_autoCloseTimer += _dt;
        if (m_autoCloseTimer > getTemplate()->getAutoCloseDelay())
        {
            close();
        }
    }

    void Ray_GeyserPlatformAIComponent::getShape( PhysShapePolygon & _poly )
    {
        if ( m_physModifier.getTemplate()->getType() != PhysForceModifier::TYPE_POLYLINE )
        {
            return;
        }

        // NOTE: the PhysForceModifier works in actor space (has its own offset),
        // and doesn't use the actor's scale for TYPE_POLYLINE
        // -> compute everything in actor space!
        // BUT, still take the flip into account.

        const f32 forceLength = getForceLength();
        const Transform2d& localTransform = getLocalTransform2d(bfalse);    // we want to flip our points, but express them in the unflipped-actor-space

        const Vec2d start = localTransform.getPos();
        const Vec2d end = localTransform.transformPos(Vec2d(forceLength, 0.f));
        const Vec2d normal = localTransform.transformDir(Vec2d(0.f, getScale().m_y * 0.5f));


        const Vec2d botHalfWidth = normal * getTemplate()->getForceWidthBottom();
        const Vec2d topHalfWidth = normal * getTemplate()->getForceWidthTop();

        Vec2d points[4];

        points[0] = start + botHalfWidth;
        points[1] = end + topHalfWidth;
        points[2] = end - topHalfWidth;
        points[3] = start - botHalfWidth;

        //Transform2d worldTransform = getWorldTransform2d();
        //points[0] = worldTransform.transformPos(start + botHalfWidth);
        //points[1] = worldTransform.transformPos(end + topHalfWidth);
        //points[2] = worldTransform.transformPos(end - topHalfWidth);
        //points[3] = worldTransform.transformPos(start - botHalfWidth);


        _poly.setPoints(points,4);

    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void Ray_GeyserPlatformAIComponent::drawDebugPatch( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        const f32 z = m_actor->getDepth() + getTemplate()->getZOffset();
        const Vec2d scale = getScale();

        Vec2d botHalfWidth, topHalfWidth;
        if (!getTemplate()->getPatchSwapXY())
        {
            botHalfWidth = m_patchNormal * scale.m_y * getTemplate()->getPatchWidthBottom() * 0.5f;
            topHalfWidth = m_patchNormal * scale.m_y * getTemplate()->getPatchWidthTop() * 0.5f;
        }
        else
        {
            f32 patchLength = m_length * getTemplate()->getPatchLengthMultiplier() + getTemplate()->getPatchLengthOffset() - getTemplate()->getPatchStartOffset();
            botHalfWidth = m_patchNormal * patchLength * 0.5f;
            topHalfWidth = m_patchNormal * patchLength * 0.5f;
        }

        Vec2d points[4];
        // left side (= top)
        points[0] = m_patchStart - botHalfWidth;
        points[1] = m_patchEnd - topHalfWidth;
        // right side (= bottom)
        points[2] = m_patchEnd + topHalfWidth;
        points[3] = m_patchStart + botHalfWidth;

        const Color& color = Color::yellow();
        DebugDraw::line2D(points[0], points[1], z, color);
        DebugDraw::line2D(points[1], points[2], z, color);
        DebugDraw::line2D(points[2], points[3], z, color);
        DebugDraw::line2D(points[3], points[0], z, color);
    }

    void Ray_GeyserPlatformAIComponent::drawDebugFX() const
    {
        if (m_geyserFxHandle == U32_INVALID)
        {
            return;
        }


        const SafeArray<StringID>& generators = getTemplate()->getFxGenerators();

        switch (getTemplate()->getFxGeneratorType())
        {
        case Ray_GeyserPlatformAIComponent_Template::GeneratorType_Box:
            {
                const Vec2d fxSize = getFxSize();
                if (fxSize.m_x < MTH_EPSILON || fxSize.m_y < MTH_EPSILON)
                {
                    break;
                }

                for (u32 i = 0; i < generators.size(); i++)
                {
                    ITF_ParticleGenerator* generator = m_fxController->getParticleGenerator(m_geyserFxHandle, generators[i]);
                    if (generator)
                    {
                        drawDebugFxGenerator_box(generator, fxSize.m_x, fxSize.m_y);
                    }
                }
            }
            break;
        }
    }

    void Ray_GeyserPlatformAIComponent::drawDebugFxGenerator_box( ITF_ParticleGenerator* _generator, const f32 _length, const f32 _width ) const
    {
        const Transform3d& transform = getFxTransform();

        const ParticleGeneratorParameters& tplParams = _generator->getTemplate()->getParameters();
        ParticleGeneratorParameters* params = _generator->getParameters();

        const f32 tplV = tplParams.getVelocityNorm();
        const f32 t = _length / tplV;

        AABB fxBox( Vec2d(0, -_width * 0.5f), Vec2d(_length, _width * 0.5f) );
        DebugDraw::AABB(transform, fxBox, Color::white(), 0, 1, "fx");

        const AABB& genBox = params->getGenBox();
        DebugDraw::AABB(transform, genBox, Color::white(), 0, 1, "fx genBox");

        Vec3d min = transform.transformPos(genBox.getMin());
        DebugDraw::text(min, Vec2d(0, -20),
            Color::white(), DebugDraw::defaultBackgroundColor(),
            "particles: %d / surface: %.2f m� = density: %.2f ptcl/m�",
            _generator->getActiveParticlesCount(),
            _width * _length,
            _generator->getActiveParticlesCount() / (_width * _length));
        DebugDraw::text(min, Vec2d(0, -40),
            Color::white(), DebugDraw::defaultBackgroundColor(),
            "lt: %.2f s, freq: %.f ptcl/s (%.4f), max ptcls: %d",
            t, f32_Inv(params->getFrequency()), params->getFrequency(), _generator->getMaxParticlesCount());
    }
#endif // ITF_SUPPORT_DEBUGFEATURE
};

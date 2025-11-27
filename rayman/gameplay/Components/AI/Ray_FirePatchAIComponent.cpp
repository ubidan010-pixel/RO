#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_FIREPATCHAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_FirePatchAIComponent.h"
#endif //_ITF_RAY_FIREPATCHAICOMPONENT_H_

#ifndef _ITF_CURVECOMPONENT_H_
#include "gameplay/Components/Misc/CurveComponent.h"
#endif //_ITF_CURVECOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_FirePatchAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_FirePatchAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_FirePatchAIComponent)
        //VALIDATE_COMPONENT_PARAM("", m_curveComponent,  "Ray_FirePatchAIComponent requires an CurveComponent.");
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_FirePatchAIComponent::Ray_FirePatchAIComponent()
    : m_bezierCurveComponent(NULL)
    , m_fxControllerComponent(NULL)
    , m_init(bfalse)
    , m_tessellationRatio(0.0f)
    , m_color(Color::white())
    , m_fogColor(0.f, Color::white().m_r, Color::white().m_g, Color::white().m_b)
    , m_uvMode(UV_MODE_OPTIMUM)
    , m_countTrame(0)
    , m_distOpening(0.0f)
    , m_distClosing(0.0f)
    , m_state(STATE_CLOSED)
    , m_handleSound(U32_INVALID)
    {
        // none
    }

    //*****************************************************************************

    Ray_FirePatchAIComponent::~Ray_FirePatchAIComponent()
    {
        // None
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
     
        // Register Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), this);
        if (getTemplate()->getStopOnHitSuccessful())
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013),this);
        // Components
        m_bezierCurveComponent = m_actor->GetComponent<BezierCurveComponent>(); // Mandatory
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
        
        // Init texture
        initTexture();

        if (getTemplate()->getUseSmoke())
        {
            m_distOpening = -(getTemplate()->getSmokeForwardOffset());
        }

    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        if ( EventGeneric* eventGeneric = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)) )
        {
            const StringID s_Open = ITF_GET_STRINGID_CRC(Open,812432275);
            const StringID s_Close = ITF_GET_STRINGID_CRC(Close,3883599349);

            if (eventGeneric->getId() == s_Open)
            {
                receiveEvent(btrue);
            }
            else if (eventGeneric->getId() == s_Close)
            {
                receiveEvent(bfalse);
            }
        }
        else if (AnimGameplayEvent * gEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (getTemplate()->getStartMarker().isValid() && gEvent->getName() == getTemplate()->getStartMarker())
            {
                receiveEvent(btrue);
            }
            else if (getTemplate()->getStopMarker().isValid() && gEvent->getName() == getTemplate()->getStopMarker())
            {
                receiveEvent(bfalse);
            }
        }
        else if (getTemplate()->getStopOnHitSuccessful())
        {
            if (EventHitSuccessful * onHit = _event->DynamicCast<EventHitSuccessful>(ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013)))
            {
                receiveEvent(bfalse);
            }
        }
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::receiveEvent(bbool _open)
    {
        if (_open && m_state == STATE_CLOSED)
        {
            if (getTemplate()->getUseSmoke())
            {
                m_distOpening = -(getTemplate()->getSmokeForwardOffset());
            }
            else
            {
                m_distOpening = 0.0f;
            }

            m_distClosing = 0.0f;
            m_state = STATE_OPENING;

            startSound();

        }
        else if(!_open && (m_state == STATE_OPEN || m_state == STATE_OPENING))
        {
            if (getTemplate()->getUseSmoke())
            {
                m_distClosing = -(getTemplate()->getSmokeForwardOffset());
            }
            else
            {
                m_distClosing = 0.0f;
            }

            m_state = STATE_CLOSING;
        }
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::startSound()
    {
        if (m_fxControllerComponent)
        {
            StringID soundName = "FirePatch_Loop";
            if (soundName.isValid() && m_handleSound == U32_INVALID)
            {
                m_handleSound = m_fxControllerComponent->playFX(soundName);
                m_fxControllerComponent->setFXPosFromHandle(m_handleSound,m_actor->getPos()); 
            }
        }
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::stopSound()
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->stopFXFromHandle(m_handleSound);
            m_handleSound = U32_INVALID;
        }
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::moveSound(Vec3d _pos)
    {
        if (m_fxControllerComponent && m_handleSound != U32_INVALID)
        {
            m_fxControllerComponent->setFXPosFromHandle(m_handleSound, _pos); 
        }
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::initTexture()
    {
        // Side texture
        if (!getTemplate()->getTextureSideFile().isEmpty())
        {
            m_textureAnimSide.m_textureID = m_actor->getResourceGroup()->addResource(Resource::ResourceType_Texture, getTemplate()->getTextureSideFile());

            Texture * texture = static_cast<Texture*>(m_textureAnimSide.m_textureID.getResource());
            if(texture)
            {
                const UVAtlas* uvAtlas = texture->getUVAtlas();
                if (uvAtlas)
                {
                    m_textureAnimSide.m_useAtlas = btrue;
                    m_textureAnimSide.m_countAtlasUv = uvAtlas->getNumberUV();
                }
            }        
        }


        // Smoke texture
        if (!getTemplate()->getTextureSmokeFile().isEmpty())
        {
            m_textureAnimSmoke.m_textureID = m_actor->getResourceGroup()->addResource(Resource::ResourceType_Texture, getTemplate()->getTextureSmokeFile());

            Texture * texture = static_cast<Texture*>(m_textureAnimSmoke.m_textureID.getResource());
            if(texture)
            {
                const UVAtlas* uvAtlas = texture->getUVAtlas();
                if (uvAtlas)
                {
                    m_textureAnimSmoke.m_useAtlas = btrue;
                    m_textureAnimSmoke.m_countAtlasUv = uvAtlas->getNumberUV();
                }
            }        
        }


        if (getTemplate()->getTessellationLength() > 0)
        {
            m_tessellationRatio = f32_Inv(getTemplate()->getTessellationLength());
        }


    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::initPathesList()
    {

        BezierCurve& curve = getBezierCurve();
        f32 curveLengthTotal = curve.getTotalLength();

        f32 spaceMinBetween2Start = getTemplate()->getSpaceMinBetween2Start();

        f32 sidePatchLength = getTemplate()->getSidePatchLength();
        f32 sidePatchWidth = getTemplate()->getSidePatchWidth();

        const Transform3d worldTransform = getWorldTransform(m_actor);

        ITF_WARNING_CATEGORY(LD, m_actor, curveLengthTotal > sidePatchLength, "sidePatchLength is bigger than curveLengthTotal !");

        if (curveLengthTotal <= sidePatchLength)
        {
            return;
        }

        f32 offsetMin = getTemplate()->getOffsetMin();
        f32 offsetMax = getTemplate()->getOffsetMax();

        f32 scaleMin = getTemplate()->getScaleMin();
        f32 scaleMax = getTemplate()->getScaleMax();

        u32 modTileMin = getTemplate()->getModTileMin();
        u32 modTileMax = getTemplate()->getModTileMax();

        f32 distStartMax = 0.0f;
        f32 distEndMax   = 0.0f;

        m_patchesSide.clear();
        m_patchesSmoke.clear();

        u32 sideMode = getTemplate()->getSideMode();

        u32 index = 0;
        for (;;)
        {
            SubPatch newPatch;
            newPatch.m_isSidePatch = btrue;
            f32 scale = 1.0f;

            if ((sideMode == MODE_BOTH_SIDE && index%2 == 0) || sideMode == MODE_RIGHT_SIDE)
            {
                newPatch.m_flipped = btrue;
            }

            if (m_patchesSide.size() == 0)
            {
                // First patch
                newPatch.m_distStart = 0.0f;
                newPatch.m_offset = offsetMin; 
            }
            else
            {
                // Other patch
                //f32 space = Seeder::getSharedSeeder().GetFloat(spaceMinBetween2Start, spaceMaxBetween2Start); 
                f32 space = spaceMinBetween2Start; 
                newPatch.m_distStart = distStartMax + space;

                newPatch.m_offset = Seeder::getSharedSeeder().GetFloat(offsetMin, offsetMax); 

                scale  = Seeder::getSharedSeeder().GetFloat(scaleMin, scaleMax);

                //
                if (newPatch.m_distStart + (sidePatchLength * scale) >= curveLengthTotal)
                {
                    scale = 1.0f;
                }


            }


            newPatch.m_modTile  = Seeder::getSharedSeeder().GetU32(modTileMin, modTileMax);

            newPatch.m_length = sidePatchLength * scale;
            newPatch.m_width  = sidePatchWidth * scale;
            newPatch.m_texAnim = &m_textureAnimSide;

            distStartMax =  newPatch.m_distStart;
            distEndMax   = distStartMax + newPatch.m_length;

            // Smoke patch
            SubPatch newSmokePatch;
            if (getTemplate()->getUseSmoke())
            {
                newSmokePatch = newPatch;   
                newSmokePatch.m_texAnim = &m_textureAnimSmoke;
            }

            if (distEndMax < curveLengthTotal)
            {
				// Compute subPatch AABB
				Vec3d ptA = curve.getPosAtDistance(newPatch.m_distStart);
				ptA = worldTransform.transformPos(ptA);
				Vec3d ptB = curve.getPosAtDistance(newPatch.m_distStart + newPatch.m_length);
				ptB = worldTransform.transformPos(ptB);

				AABB subPatchAabb(ptA);
				subPatchAabb.grow(ptB);
								
				newPatch.m_aabb = subPatchAabb;
				newSmokePatch.m_aabb = subPatchAabb;
				// aabb

				m_patchesSide.push_back(newPatch);

                if (getTemplate()->getUseSmoke())
                {
                    m_patchesSmoke.push_back(newSmokePatch);
                }

            }
            else
            {
                newPatch.m_distStart = curveLengthTotal - newPatch.m_length;

				// Compute subPatch AABB
				Vec3d ptA = curve.getPosAtDistance(newPatch.m_distStart);
				ptA = worldTransform.transformPos(ptA);
				Vec3d ptB = curve.getPosAtDistance(newPatch.m_distStart + newPatch.m_length);
				ptB = worldTransform.transformPos(ptB);

				AABB subPatchAabb(ptA);
				subPatchAabb.grow(ptB);

				newPatch.m_aabb = subPatchAabb;
				newSmokePatch.m_aabb = subPatchAabb;
				// aabb

                m_patchesSide.push_back(newPatch);

                if (getTemplate()->getUseSmoke())
                {
                    newSmokePatch.m_distStart = curveLengthTotal - newSmokePatch.m_length;
                    m_patchesSmoke.push_back(newSmokePatch);
                }

                break;
            }

            index++;

        }

    }

//*****************************************************************************

    void Ray_FirePatchAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        // Init
        if (!m_init)
        {   
            initPathesList();
            m_init = btrue;
        }

    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::Update( f32 _dt ) 
    {
        Super::Update(_dt);

        BezierCurve& curve = getBezierCurve();
        const Transform3d worldTransform = getWorldTransform(m_actor);

        growAABB(&worldTransform);

        // opening
        if (m_state == STATE_OPENING)
        {
            m_distOpening += getTemplate()->getOpenSpeed() * _dt;

            // Move sound during opening
            if (m_distOpening <= curve.getTotalLength())
            {
                f32 distForSound = m_distOpening;
                if (distForSound < 0.0f)
                {
                    distForSound = 0.0f;
                }

                u32 indexEdge = curve.getEdgeIndexAtDistance(distForSound);
                f32 t = curve.getTAtDistance(distForSound, indexEdge);
                Vec3d soundPos = curve.getPosAtT(t,indexEdge);
                soundPos = worldTransform.transformPos(soundPos);
                moveSound(soundPos);
            }

            if (m_distOpening > curve.getTotalLength())
            {
                m_state = STATE_OPEN;
                m_distOpening = curve.getTotalLength();
            }
        }
        else if (m_state == STATE_CLOSING)
        {

            if (!getTemplate()->getStopDeployOnStop())
                m_distOpening += getTemplate()->getOpenSpeed() * _dt;
            m_distClosing += getTemplate()->getOpenSpeed() * _dt;

            // Move sound during opening
            if (m_distOpening <= curve.getTotalLength())
            {
                f32 distForSound = m_distOpening;
                if (distForSound < 0.0f)
                {
                    distForSound = 0.0f;
                }

                u32 indexEdge = curve.getEdgeIndexAtDistance(distForSound);
                f32 t = curve.getTAtDistance(distForSound, indexEdge);
                Vec3d soundPos = curve.getPosAtT(t,indexEdge);
                soundPos = worldTransform.transformPos(soundPos);
                moveSound(soundPos);
            }

            if (m_distOpening > curve.getTotalLength())
            {
                m_distOpening = curve.getTotalLength();
            }

            if (m_distClosing > m_distOpening + getTemplate()->getStimOffsetTail())
            {
                m_state = STATE_CLOSED;
                m_distClosing = m_distOpening;
                stopSound();
            }
        }



        m_countTrame++;

        updatePatchSide();

        if (getTemplate()->getUseSmoke())
        {
            updatePatchSmoke();
        }

        updateStim();

    }
  

    //*****************************************************************************

    void Ray_FirePatchAIComponent::updatePatchSide() 
    {
        for(u32 i = 0; i < m_patchesSide.size(); i++)
        {
            SubPatch& curPatch = m_patchesSide[i];

            if (curPatch.m_state == PATCH_OFF)
            {
                if (m_distOpening > curPatch.m_distStart && m_distClosing <= curPatch.m_distStart)
                {
                    curPatch.m_state = PATCH_OPENING;
                }
            }

            if (curPatch.m_state == PATCH_ON)
            {
                if (m_distClosing > curPatch.m_distStart)
                {
                    curPatch.m_state = PATCH_CLOSING;
                }
            }

            ITF_ASSERT(curPatch.m_modTile);
            u32 mod = m_countTrame%curPatch.m_modTile;

            u32 minIndex = 0;
            u32 maxIndex = 0;

            if (curPatch.m_state == PATCH_OPENING || curPatch.m_state == PATCH_ON)
            {
                if (mod == 0)
                {
                    curPatch.m_curIndexAtlas++;

                    if (curPatch.m_state == PATCH_OPENING)
                    {
                        minIndex = getTemplate()->getStartOpen();
                        maxIndex = getTemplate()->getEndOpen();

                        if (curPatch.m_curIndexAtlas >= maxIndex - minIndex)
                        {
                            curPatch.m_state = PATCH_ON;
                        }

                    }

                    if (curPatch.m_state == PATCH_ON)
                    {
                        minIndex = getTemplate()->getStartLoop();
                        maxIndex = getTemplate()->getEndLoop();
                    }

                    if (curPatch.m_curIndexAtlas >= maxIndex)
                    {
                        curPatch.m_curIndexAtlas = minIndex;
                    }

                }
            }
            else if (curPatch.m_state == PATCH_CLOSING)
            {
                if (mod == 0)
                {
                    curPatch.m_curIndexAtlas++;

                    if (curPatch.m_state == PATCH_CLOSING)
                    {
                        minIndex = getTemplate()->getStartClose();
                        maxIndex = getTemplate()->getEndClose();

                        if (curPatch.m_curIndexAtlas >= maxIndex)
                        {
                            curPatch.m_curIndexAtlas = 0;
                            curPatch.m_state = PATCH_OFF;
                        }
                    }
                }
            }

        }
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::updatePatchSmoke() 
    {
        f32 m_distSmokeOpening = m_distOpening + (getTemplate()->getSmokeForwardOffset());
        f32 m_distSmokeClosing = m_distOpening + (getTemplate()->getSmokeBackwardOffset());

        for(u32 i = 0; i < m_patchesSmoke.size(); i++)
        {
            SubPatch& curPatch = m_patchesSmoke[i];

            if (curPatch.m_state == PATCH_OFF)
            {
                if (m_distSmokeOpening > curPatch.m_distStart && m_distSmokeClosing <= curPatch.m_distStart)
                {
                    curPatch.m_state = PATCH_OPENING;
                }
            }

            if (curPatch.m_state == PATCH_ON || curPatch.m_state == PATCH_OPENING)
            {
                if (m_distSmokeClosing > curPatch.m_distStart)
                {
                    //ITF_WARNING_CATEGORY(LD, m_actor, curPatch.m_state != PATCH_OPENING, "Stop smoking is to early, you can have blink in smoke texture !");
                    curPatch.m_curIndexAtlas = getTemplate()->getSmokeStartClose();
                    curPatch.m_state = PATCH_CLOSING;
                }
            }

            ITF_ASSERT(curPatch.m_modTile);
            u32 mod = m_countTrame%curPatch.m_modTile;

            u32 minIndex = 0;
            u32 maxIndex = 0;

            if (curPatch.m_state == PATCH_OPENING || curPatch.m_state == PATCH_ON)
            {
                if (mod == 0)
                {
                    curPatch.m_curIndexAtlas++;

                    if (curPatch.m_state == PATCH_OPENING)
                    {
                        minIndex = getTemplate()->getSmokeStartOpen();
                        maxIndex = getTemplate()->getSmokeEndOpen();

                        if (curPatch.m_curIndexAtlas >= maxIndex)
                        {
                            curPatch.m_state = PATCH_ON;
                        }

                    }

                    if (curPatch.m_state == PATCH_ON)
                    {
                        minIndex = getTemplate()->getSmokeStartLoop();
                        maxIndex = getTemplate()->getSmokeEndLoop();
                    }

                    if (curPatch.m_curIndexAtlas >= maxIndex)
                    {
                        curPatch.m_curIndexAtlas = minIndex;
                    }

                }
            }
            else if (curPatch.m_state == PATCH_CLOSING)
            {
                if (mod == 0)
                {
                    curPatch.m_curIndexAtlas++;

                    minIndex = getTemplate()->getSmokeStartClose();
                    maxIndex = getTemplate()->getSmokeEndClose();

                    if (curPatch.m_curIndexAtlas >= maxIndex)
                    {
                        curPatch.m_curIndexAtlas = 0;
                        curPatch.m_state = PATCH_OFF;
                    }

                }

            }
        }
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::updateStim()
    {
        if (/*getTemplate()->getUseStim() &&*/ m_pointsList.size())
        {
            // Send stim
            PunchStim punchStim;
            punchStim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
            punchStim.setIsRadial(bfalse);
            punchStim.setLevel(0);
            punchStim.setSender(m_actor->getRef());
            punchStim.setFaction(getTemplate()->getFaction());

            ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> > actorCollisions;

            Vec2d referencePos(0.0f, 0.0f);
            Vec2d pointsList[4];
            PhysContactsContainer contacts;

            for (u32 i = 0; i < ((m_pointsList.size() / 2) -1); ++i)
            {
                const u32 base = i * 2;

                if(m_actor->isFlipped())
                {
                    pointsList[0] = m_pointsList[base+1];  
                    pointsList[1] = m_pointsList[base+3];
                    pointsList[2] = m_pointsList[base+2];
                    pointsList[3] = m_pointsList[base];
                }
                else
                {
                    pointsList[0] = m_pointsList[base];
                    pointsList[1] = m_pointsList[base+2];
                    pointsList[2] = m_pointsList[base+3];
                    pointsList[3] = m_pointsList[base+1];
                }

                m_stimShape.setPoints(pointsList, 4);

#ifndef ITF_FINAL
                //DebugDraw::shape(m_actor->getPos(),0.0f,&m_stimShape);
#endif // ITF_FINAL
                
                contacts.clear();
                PHYSWORLD->collidePhantoms(referencePos,referencePos, 0.0f,
                    &m_stimShape, m_actor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
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

                    actorCollisions[otherActorRef].push_back(&contacts[i]);
                }
                const f32 z = m_actor->getDepth();
                punchStim.setAngle(0.0f);

                for (ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> >::iterator it = actorCollisions.begin(); it != actorCollisions.end(); ++it)
                {

                    ITF_VECTOR<SCollidableContact*> & collidables = it->second;
                    SCollidableContact* firstContact = collidables[0];

                    punchStim.setDirection(firstContact->m_normal);

                    Vec3d fxPos = firstContact->m_contactPoint.to3d(z);
                    Vec2d contactPos = firstContact->m_contactPoint;

                    punchStim.setFxPos(fxPos);
                    punchStim.setPos(contactPos);

                    for (ITF_VECTOR<SCollidableContact*>::iterator it2 = collidables.begin(); it2 != collidables.end(); ++it2)
                    {
                        if (punchStim.getContacts().size() < punchStim.getContacts().capacity())
                            punchStim.addContact(*(*it2));
                    }

                    Actor * actor = it->first.getActor();
                    if (actor)
                        actor->onEvent(&punchStim);
                    punchStim.resetContacts();
                }

                if (getTemplate()->getStopOnHitSuccessful())
                {
                    PhysContactsContainer contacts;
                    bbool hitEnvironment = PHYSWORLD->checkEncroachment(
                        referencePos,referencePos, 0.0f,
                        &m_stimShape,
                        ECOLLISIONFILTER_ENVIRONMENT,
                        m_actor->getDepth(), 
                        contacts);

                    if (hitEnvironment)
                    {
                        bbool hitOne = bfalse;

                        for ( u32 i = 0; i < contacts.size(); i++ )
                        {
                            SCollidableContact& contact = contacts[i];

                            if ( contact.m_collidableUserData == m_actor->getRef() )
                            {
                                continue;
                            }

                            BaseObject* obj = contact.m_collidableUserData.getObject();
                            if (!obj)
                            {
                                continue;
                            }


                            if ( PolyLine* polyLine = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)) )
                            {
                                if (polyLine->getOwnerActor() == m_actor)
                                {
                                    continue;
                                }
                                //GFX_ADAPTER->drawDBGCircle(contact.m_contactPoint.to3d(m_actor->getPos().m_z), 0.2f, Color::blue(), 1.f);
                                hitOne = btrue;
                                break;
                            }
                        }

                        if (hitOne)
                        {
                            receiveEvent(bfalse);
                        }
                    }
                }
            }

        }
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::calculateStimShape( const Transform3d* _transform /*= NULL*/ )
    {
        if (m_distOpening < 1.0f)
        {
            return;
        }

       m_pointsList.clear();

        if (m_state == STATE_CLOSED)
        {
            return;
        }

        
        const f32 stimBezierLength = getTemplate()->getStimBezierLength();
        const f32 stimBezierWidth = getTemplate()->getStimBezierWidth();

        BezierCurve& curve = getBezierCurve();

        f32 distMax = m_distOpening - getTemplate()->getDistSafeHead();

        f32 currentLength = m_distClosing - getTemplate()->getStimOffsetTail();
        if (currentLength < 0.0f)
        {
            currentLength = 0.0f;
        }

        if (distMax < 0.0f)
        {
            return;
        }

        while (currentLength <= distMax)
        {

            if (currentLength > curve.getTotalLength())
            {
                break;
            }

            u32 indexEdge = curve.getEdgeIndexAtDistance(currentLength);
            f32 t         = curve.getTAtDistance(currentLength, indexEdge);
            Vec2d pos     = curve.getPosAtT(t, indexEdge).truncateTo2D();
            Vec2d tan     = curve.getTanAtT(t, indexEdge).normalize().truncateTo2D();


            const Vec2d dir = (tan * (stimBezierWidth * 0.5f)).getPerpendicular();

            Vec2d pA = pos + dir;
            Vec2d pB = pos - dir;

            if (_transform)
            {
                pA = _transform->transformPos(pA).truncateTo2D();
                pB = _transform->transformPos(pB).truncateTo2D();
            }

            m_pointsList.push_back(pA);
            m_pointsList.push_back(pB);

            if (currentLength == distMax)
            {
                currentLength += 1.0f; // to stop 
            }
            else
            {
                currentLength = Min(currentLength + stimBezierLength, distMax);
            }
        } 
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::growAABB( const Transform3d* _transform /*= NULL*/ )
    {
        BezierCurve& curve = getBezierCurve();
        AABB aabb(m_actor->getPos());

        for (u32 i = 0; i < curve.getNumEdges(); i++) 
        {
            const BezierCurve::Edge* edge = curve.getEdgeAt(i);

            if (_transform)
            {
                Vec3d p0 = _transform->transformPos(edge->m_bezier.m_p0);
                Vec3d p1 = _transform->transformPos(edge->m_bezier.m_p1);
                Vec3d p2 = _transform->transformPos(edge->m_bezier.m_p2);
                Vec3d p3 = _transform->transformPos(edge->m_bezier.m_p3);

                aabb.grow(p0);
                aabb.grow(p1);
                aabb.grow(p2);
                aabb.grow(p3);
            }
        }

        m_actor->growRelativeAABBFromAbsolute(aabb);
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::Draw()
    {
        Super::Draw();

        BezierCurve& curve = getBezierCurve();

        const Transform3d worldTransform = getWorldTransform(m_actor);
		bbool useOptimAABB = getTemplate()->getUseOptimAABB();


#ifndef ITF_FINAL
        //// DEBUG

        //f32 m_distSmokeOpening = m_distOpening + (getTemplate()->getSmokeForwardOffset());
        //f32 m_distSmokeClosing = m_distOpening + (getTemplate()->getSmokeBackwardOffset());

        //f32 m_distSmokeOpening = m_distOpening;
        //f32 m_distSmokeClosing = m_distClosing;

        //u32 indexEdge = curve.getEdgeIndexAtDistance(m_distSmokeOpening);
        //if (indexEdge != U32_INVALID)
        //{
        //    f32 t = curve.getTAtDistance(m_distSmokeOpening, indexEdge);
        //    Vec3d pos = curve.getPosAtT(t, indexEdge);

        //    pos = worldTransform.transformPos(pos);

        //    DebugDraw::circle(pos, 0.5f, Color::green());
        //    DebugDraw::circle(pos, 0.2f, Color::green());
        //}


        //indexEdge = curve.getEdgeIndexAtDistance(m_distSmokeClosing);
        //if (indexEdge != U32_INVALID)
        //{
        //    f32 t = curve.getTAtDistance(m_distSmokeClosing, indexEdge);
        //    Vec3d pos = curve.getPosAtT(t, indexEdge);

        //    pos = worldTransform.transformPos(pos);

        //    DebugDraw::circle(pos, 0.5f, Color::red());
        //    DebugDraw::circle(pos, 0.2f, Color::red());
        //}
        // DEBUG

#endif // ITF_FINAL



        ITF_ASSERT(curve.getNumEdges());
        if (curve.getNumEdges() == 0)
        {
            return;
        }

        const f32 totalLength = curve.getTotalLength();
        ITF_ASSERT_MSG(totalLength, "you need to sample your curve!");
        if (totalLength == 0)
        {
            return;
        }

        //const Transform3d worldTransform = getWorldInitTransform(m_actor);

        f32 zOffset = 0.0f;
        f32 valueZ = -0.00001f;
		f32 zActor = m_actor->getDepth();

        // Draw fire patch
        for(u32 i = 0; i < m_patchesSide.size(); i++)
        {
            SubPatch& curPatch = m_patchesSide[i];

            if (curPatch.m_state == PATCH_OFF)
            {
                continue;
            }

            zOffset += valueZ;

			if (!useOptimAABB || (useOptimAABB && CAMERA->isRectVisible(curPatch.m_aabb, zActor)))
			{
				drawSubPatch(curve, curPatch, zOffset, bfalse, &worldTransform);
			}

        }

        // Draw smoke patch
        zOffset += valueZ;
        if (getTemplate()->getUseSmoke())
        {
            for(u32 i = 0; i < m_patchesSmoke.size(); i++)
            {
                SubPatch& curPatch = m_patchesSmoke[i];

                if (curPatch.m_state == PATCH_OFF)
                {
                    continue;
                }

                zOffset += valueZ;

				if (!useOptimAABB || (useOptimAABB && CAMERA->isRectVisible(curPatch.m_aabb, zActor)))
				{
					drawSubPatch(curve, curPatch, zOffset, bfalse, &worldTransform);
				}
            }
        }

        calculateStimShape(&worldTransform);

    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::drawSubPatch(const BezierCurve& _curve, SubPatch& _subPatch, f32 _zOffset, bbool _flipped /* = bfalse*/, const Transform3d* _transform /*= NULL*/ )
    {

        if (_subPatch.m_state == PATCH_OFF)
        {
            return;
        }

        Texture* texture = static_cast<Texture*>(_subPatch.m_texAnim->m_textureID.getResource());
        ITF_ASSERT(texture);

        f32 distStart = _subPatch.m_distStart;
        f32 distEnd  = distStart + _subPatch.m_length;

        BezierPatchParams patchParams;
       
        Vec3d p0, p1, p2, p3;
        Vec3d tanP0, tanP3;

        // Compute P0
        if (distStart > _curve.getTotalLength())
        {
            return;
        }
        u32 indexEdgeP0   = _curve.getEdgeIndexAtDistance(distStart);
        f32 tP0           = _curve.getTAtDistance(distStart, indexEdgeP0);
        p0          = _curve.getPosAtT(tP0, indexEdgeP0);
        tanP0       = _curve.getTanAtT(tP0, indexEdgeP0);
        f32 normTanP0 = tanP0.f32_normalize();

        // Compute P3
        if (distEnd > _curve.getTotalLength())
        {
            return;
        }
        u32 indexEdgeP3   = _curve.getEdgeIndexAtDistance(distEnd);
        f32 tP3           = _curve.getTAtDistance(distEnd, indexEdgeP3);
        p3          = _curve.getPosAtT(tP3, indexEdgeP3);
        tanP3       = _curve.getTanAtT(tP3, indexEdgeP3);
        f32 normTanP3 = tanP3.f32_normalize();

        if (indexEdgeP0 == indexEdgeP3)
        {
            // Draw in one segment of curve
            Bezier4<Vec3d> bezierSegment = _curve.getEdgeAt(indexEdgeP0)->m_bezier;

            Bezier4<Vec3d> bezierSub;
            bezierSegment.getSegment(tP0, tP3, bezierSub);

            p1 = bezierSub.m_p1;
            p2 = bezierSub.m_p2;
        }
        else
        {
            // Ratio tang
            f32 ratioTanP0 = normTanP0 / (normTanP0 + normTanP3);
            f32 ratioTanP3 = normTanP3 / (normTanP0 + normTanP3);

            // Compute P1
            p1          = p0 + (tanP0 * (_subPatch.m_length * ratioTanP0));

            // Compute P2
            p2          = p3 - (tanP3 * (_subPatch.m_length * ratioTanP3));

        }

        // Create bezier for subPatch
        Bezier4<Vec3d> bezier(p0, p1, p2, p3);
        f32 lenBezier = bezier.getLength(10);

        // Fill params
        patchParams.m_z             = m_actor->getDepth() + getTemplate()->getZOffset() + _zOffset;
        patchParams.m_texture       = texture;
        patchParams.m_uvMode        = m_uvMode;
        patchParams.m_startColor    = patchParams.m_endColor = m_color;
        patchParams.m_fogColor      = m_fogColor;

        patchParams.m_startWidth = _subPatch.m_width;
        patchParams.m_endWidth   = _subPatch.m_width;

        patchParams.m_blendMode  = GFX_BLEND_ALPHA;


        if ( m_tessellationRatio > 0 )
        {
            patchParams.m_vdivToDraw = (u32)(lenBezier * m_tessellationRatio);
        }

        // Choose UV for animation
        if (_subPatch.m_texAnim->m_useAtlas)
        {
            Vec2d uv0, uv1;
            getUVDatabyIndex(texture, _subPatch.m_curIndexAtlas, uv0, uv1);
            patchParams.m_startUV = uv0;
            patchParams.m_endUV = uv1;

            if ((_subPatch.m_flipped && !m_actor->isFlipped()) || (!_subPatch.m_flipped && m_actor->isFlipped()))
            {
                f32 yStart = patchParams.m_startUV.m_y;
                f32 yEnd = patchParams.m_endUV.m_y;

                patchParams.m_startUV.m_y = yEnd;
                patchParams.m_endUV.m_y = yStart;
            }
        }

        // Scale offset / Move of semi width
        //if (_subPatch.m_isSidePatch)
        //{
        //    f32 scaleOffset = _subPatch.m_width * 0.5f;
        //    if (_subPatch.m_flipped)
        //    {
        //        scaleOffset = -scaleOffset;
        //    }
        //        p0 += tanP0.getPerpendicular() * scaleOffset;
        //        p1 += tanP0.getPerpendicular() * scaleOffset;
        //        p2 += tanP3.getPerpendicular() * scaleOffset;
        //        p3 += tanP3.getPerpendicular() * scaleOffset;
        //}


        // Apply offset
        f32 offset = _subPatch.m_offset;
        if (offset != 0.0f)
        {

            if (_subPatch.m_flipped)
            {
                offset = -offset;
            }

            p0 += tanP0.getPerpendicular() * offset;
            p1 += tanP0.getPerpendicular() * offset;
            p2 += tanP3.getPerpendicular() * offset;
            p3 += tanP3.getPerpendicular() * offset;
        }


        // Apply actor transform
        if (_transform)
        {
            p0 = _transform->transformPos(p0);
            p1 = _transform->transformPos(p1);
            p2 = _transform->transformPos(p2);
            p3 = _transform->transformPos(p3);
        }

        patchParams.m_p0 = p0.truncateTo2D();
        patchParams.m_p1 = p1.truncateTo2D();
        patchParams.m_p2 = p2.truncateTo2D(); 
        patchParams.m_p3 = p3.truncateTo2D();

        // Execute draw
        _subPatch.m_patch.drawPatch(patchParams);
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::getUVDatabyIndex(Texture* _texture, u32 _index, Vec2d& _uv0, Vec2d& _uv1)
    {
        if(_texture)
        {
            const UVAtlas* uvAtlas = _texture->getUVAtlas();

            if (uvAtlas)
            {
                const UVdata& uvData = uvAtlas->getUVDatabyIndex(_index);

                _uv0 = uvData.getUV0();
                _uv1 = uvData.getUV1();

            }
        }    
    }

    //*****************************************************************************

    void Ray_FirePatchAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
    }
    BezierCurve & Ray_FirePatchAIComponent::getBezierCurve()
    {
        return m_bezierCurveComponent->getBezierCurve();
    }
#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void Ray_FirePatchAIComponent::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_pointsList.size() /*&& getTemplate()->getUseStim()*/)
        {
            PhysShapePolygon polygon;
            for (u32 i = 0; i < ((m_pointsList.size() / 2) -1); ++i)
            {
                const u32 base = i * 2;
                PhysShapePolygon::PointsList pointsList;

                if(m_actor->isFlipped())
                {
                    pointsList.push_back(m_pointsList[base+1]);  
                    pointsList.push_back(m_pointsList[base+3]);
                    pointsList.push_back(m_pointsList[base+2]);
                    pointsList.push_back(m_pointsList[base]);
                }
                else
                {
                    pointsList.push_back(m_pointsList[base]);
                    pointsList.push_back(m_pointsList[base+2]);
                    pointsList.push_back(m_pointsList[base+3]);
                    pointsList.push_back(m_pointsList[base+1]);
                }

                polygon.setPoints(pointsList);

                DebugDraw::shape(Vec3d(0.0f,0.0f,0.0f),0.0f,&polygon);
            }
        }
    }

    //*****************************************************************************
    void Ray_FirePatchAIComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        initPathesList();
    }

 

#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_FirePatchAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_FirePatchAIComponent_Template)

        SERIALIZE_MEMBER("sidePatchLength",m_sidePatchLength);
        SERIALIZE_MEMBER("sidePatchWidth",m_sidePatchWidth);
       
        SERIALIZE_MEMBER("spaceMinBetween2Start",m_spaceMinBetween2Start);
        SERIALIZE_MEMBER("spaceMaxBetween2Start",m_spaceMaxBetween2Start);

        SERIALIZE_MEMBER("offsetMin",m_offsetMin);
        SERIALIZE_MEMBER("offsetMax",m_offsetMax);

        SERIALIZE_MEMBER("scaleMin",m_scaleMin);
        SERIALIZE_MEMBER("scaleMax",m_scaleMax);
        
        SERIALIZE_MEMBER("modTileMin",m_modTileMin);
        SERIALIZE_MEMBER("modTileMax",m_modTileMax);   

        SERIALIZE_MEMBER("openSpeed",m_openSpeed);

        // Animation Fire
        SERIALIZE_MEMBER("startOpen",m_startOpen);
        SERIALIZE_MEMBER("endOpen",m_endOpen);   

        SERIALIZE_MEMBER("startLoop",m_startLoop);
        SERIALIZE_MEMBER("endLoop",m_endLoop);   

        SERIALIZE_MEMBER("startClose",m_startClose);
        SERIALIZE_MEMBER("endClose",m_endClose);   

        // Animation Smoke
        SERIALIZE_MEMBER("smokeStartOpen",m_smokeStartOpen);
        SERIALIZE_MEMBER("smokeEndOpen",m_smokeEndOpen);   

        SERIALIZE_MEMBER("smokeStartLoop",m_smokeStartLoop);
        SERIALIZE_MEMBER("smokeEndLoop",m_smokeEndLoop);   

        SERIALIZE_MEMBER("smokeStartClose",m_smokeStartClose);
        SERIALIZE_MEMBER("smokeEndClose",m_smokeEndClose);   

        SERIALIZE_MEMBER("useSmoke",m_useSmoke);   
        SERIALIZE_MEMBER("smokeForwardOffset",m_smokeForwardOffset);   
        SERIALIZE_MEMBER("smokeBackwardOffset",m_smokeBackwardOffset);   

        SERIALIZE_MEMBER("textureSide",m_textureSideFile);
        SERIALIZE_MEMBER("textureSmoke",m_textureSmokeFile);

        SERIALIZE_MEMBER("tessellationLength",m_tessellationLength);

        SERIALIZE_MEMBER ("faction", m_faction);

        SERIALIZE_MEMBER ("stimBezierLength", m_stimBezierLength);
        SERIALIZE_MEMBER ("stimBezierWidth", m_stimBezierWidth);

        SERIALIZE_MEMBER ("distSafeHead", m_distSafeHead);
        SERIALIZE_MEMBER ("stimOffsetTail", m_stimOffsetTail);

        SERIALIZE_MEMBER ("sideMode", m_sideMode);

        SERIALIZE_MEMBER("zOffset", m_zOffset);
        SERIALIZE_MEMBER("startMarker", m_startMarker);
        SERIALIZE_MEMBER("stopMarker", m_stopMarker);
        SERIALIZE_MEMBER("stopOnHitSuccessful", m_stopOnHitSuccessful);
        SERIALIZE_MEMBER("stopDeployOnStop", m_stopDeployOnStop);

		SERIALIZE_MEMBER("useOptimAABB", m_useOptimAABB);
		

    END_SERIALIZATION()

    //*****************************************************************************

    Ray_FirePatchAIComponent_Template::Ray_FirePatchAIComponent_Template()
    : m_sidePatchLength(1.0f)
    , m_sidePatchWidth(1.0f)
    , m_spaceMinBetween2Start(1.0f)
    , m_spaceMaxBetween2Start(1.0f)   
    , m_tessellationLength(0.1f)    
    , m_offsetMin(0.0f)
    , m_offsetMax(0.0f)
    , m_scaleMin(1.0f)
    , m_scaleMax(1.0f)
    , m_modTileMin(2)
    , m_modTileMax(2)

    , m_startOpen(0)
    , m_endOpen(0)
    , m_startLoop(0)
    , m_endLoop(0)
    , m_startClose(0)
    , m_endClose(0)

    , m_smokeStartOpen(0)
    , m_smokeEndOpen(0)
    , m_smokeStartLoop(0)
    , m_smokeEndLoop(0)
    , m_smokeStartClose(0)
    , m_smokeEndClose(0)

    , m_useSmoke(bfalse)
    , m_smokeForwardOffset(2.0f)
    , m_smokeBackwardOffset(0.0f)

    , m_openSpeed(10.f)
    , m_faction(0)
    , m_stimBezierLength(2.0f)
    , m_stimBezierWidth(1.0f)
    , m_distSafeHead(0.0f)
    , m_stimOffsetTail(0.0f)
    , m_sideMode(MODE_BOTH_SIDE)
    , m_zOffset(0.0f)
    , m_stopOnHitSuccessful(bfalse)
    , m_stopDeployOnStop(bfalse)
	, m_useOptimAABB(bfalse)
    {
        // none
    }

    //*****************************************************************************

    Ray_FirePatchAIComponent_Template::~Ray_FirePatchAIComponent_Template()
    {
        // none
    }


    //*****************************************************************************


};

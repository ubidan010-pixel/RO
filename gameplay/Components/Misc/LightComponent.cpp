

#include "precompiled_gameplay.h"

#ifndef _ITF_LIGHTCOMPONENT_H_
#include "gameplay/components/misc/LightComponent.h"
#endif //_ITF_LIGHTCOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_PARTICLEGENERATORCOMPONENT_H_
#include "engine/actors/components/ParticleGeneratorComponent.h"
#endif //_ITF_PARTICLEGENERATORCOMPONENT_H_

#ifndef _ITF_FXBANKCOMPONENT_H_
#include "engine/actors/components/FxBankComponent.h"
#endif //_ITF_FXBANKCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_
 
#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(LightComponent)
    BEGIN_SERIALIZATION_CHILD(LightComponent)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("useOnActor",m_useOnActor);
            SERIALIZE_MEMBER("useOnFrize",m_useOnFrize);
            SERIALIZE_MEMBER("useBV",m_useBV);
            SERIALIZE_MEMBER("alpha",m_alpha);
            SERIALIZE_MEMBER("red",m_red);
            SERIALIZE_MEMBER("green",m_green);
            SERIALIZE_MEMBER("blue",m_blue);
        END_CONDITION_BLOCK()

    END_SERIALIZATION()


    LightComponent::LightComponent()
        : Super()
        , m_shape(shape_box)
        , m_useOnActor(btrue)
        , m_useOnFrize(btrue)
        , m_useBV(btrue)
        , m_alpha(-1.f)
        , m_red(-1.f)
        , m_green(-1.f)
        , m_blue(-1.f)
    {
    }

    LightComponent::~LightComponent()
    {
        if (m_actor)
        {
            Scene* pScene = m_actor->getScene();
            if (pScene)
                pScene->removeActiveLight(m_actor->getRef());
        }
      
    }

    void LightComponent::onBecomeActive  (    )
    {
        Scene* pScene = m_actor->getScene();
        if (pScene)
        {
            if (pScene->getActiveLightList().find(m_actor->getRef()) == -1)
                pScene->addActiveLight(m_actor->getRef());
        }
    }

    void LightComponent::onBecomeInactive(    )
    {
        Scene* pScene = m_actor->getScene();
        if (pScene)
            pScene->removeActiveLight(m_actor->getRef());
    }

    void LightComponent::onActorRemovedFromScene(Scene* _pScene, bbool)
    { 
        if (_pScene && m_actor)
            _pScene->removeActiveLight(m_actor->getRef());
    }

    void LightComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //get color comp from template (if needed)
        if( m_alpha < 0.f )
        {
            const Color & templateColor = getTemplate()->getLightColor();
            m_alpha = templateColor.getAlpha() * 255.f;
            m_red = templateColor.getRed() * 255.f;
            m_green = templateColor.getGreen() * 255.f;
            m_blue = templateColor.getBlue() * 255.f;
        }

        if (getTemplate()->getShape() == "circle")
            m_shape = shape_circle;
        else
            m_shape = shape_box;

        //get near and far from template
        m_near = getTemplate()->getNear();
        m_far = getTemplate()->getFar();
        
        updateAABB();
    }

    void LightComponent::updateAABB()
    {
        const Vec2d & actorPos = m_actor->get2DPos();
        const Vec2d & actorScale = m_actor->getScale();

        const f32 rad = f32_Max(actorScale.m_x, actorScale.m_y);
        const f32 radFar    = rad * getFar();

        if (m_shape == shape_circle)
        {
            Vec2d rad2d( radFar, radFar );
            m_farAABB.setMin(actorPos - rad2d);
            m_farAABB.setMax(actorPos + rad2d);
        }
        else
        {
            const f32 range = getTemplate()->getBoxRange();
            const Vec2d vecRange(range, range);

            const Vec2d & boundMin = getTemplate()->getBox().getMin();
            const Vec2d & boundMax = getTemplate()->getBox().getMax();

            Vec2d min;
            Vec2d::Mul( &min, &boundMin, &actorScale );
            min -= vecRange;

            Vec2d max;
            Vec2d::Mul( &max, &boundMax, &actorScale );
            max += vecRange;
            m_farAABB.setMin(actorPos + min);
            m_farAABB.setMax(actorPos + max);
        }
        m_actor->changeRelativeAABBFromAbsolute(m_farAABB);
    }

    void LightComponent::Update( f32 _deltaTime )
    {
        updateAABB();

        World* pWorld = CURRENTWORLD;
        ITF_ASSERT(pWorld);
        if(pWorld)
        {
            const u32 uPlayerCount = GAMEMANAGER->getMaxPlayerCount();
            for(u32 i = 0; i < uPlayerCount; ++i)
		    {
                const Actor* pActor = GAMEMANAGER->getActivePlayer(i);
                processLight((BaseObject*)pActor);
            }

            for (u32 i=0; i<pWorld->m_particlePoolNumber; ++i)
            {
                ITF_ParticleGenerator* pool = pWorld->getParticleGeneratorPool();
                processLight(NULL, &pool[i]);
            }
        }
    }

    void LightComponent::processLight( BaseObject* _ObjToDraw, ITF_ParticleGenerator* _gen )
    {
        f32 rad = m_actor->getScale().m_x;
        if (m_actor->getScale().m_y > rad)
            rad = m_actor->getScale().m_y;

        const f32 radNear   = rad * getNear();
        const f32 radFar    = rad * getFar();
 
        const Vec3d myPos = m_actor->getPos();

        const DepthRange depthRange(m_actor->getDepth());

        m_color = Color(m_alpha / 255.f, m_red / 255.f, m_green / 255.f, m_blue / 255.f);
        m_lightInfo.m_color = m_color;
        m_lightInfo.m_position = myPos;
        m_lightInfo.m_typeData = rad;
        m_lightInfo.m_type = LightInfo::LINEAR;
        m_lightInfo.m_radNear = radNear;
        m_lightInfo.m_radFar = radFar;
        m_lightInfo.m_useBV = m_useBV;

        if (_gen)
        {
            _gen->addlight(m_lightInfo);
            return;
        }

        Pickable* pObj = static_cast<Pickable *>(_ObjToDraw);
        
        if (m_shape == shape_circle)
        {
            const f32 refDist = radFar;// * radFar;
            if (
                pObj && 
                pObj->getObjectType() == BaseObject::eActor 
                //&& depthRange.contains(pObj->getDepth()) 
				&& m_useOnActor
                )
            {
                const Actor* pAct = static_cast<const Actor*>(pObj);
                GraphicComponent *pGraphComponent = pAct->GetComponent<GraphicComponent>();
                AABB myaabb;
                if (pGraphComponent)
                    myaabb = pGraphComponent->getVisualAABB();
                else
                    myaabb = pAct->getAABB();
                
                f32 sqDist;
                if (getUseBV())
                    sqDist = myaabb.getDistMinPosAABB(m_actor->getPos().truncateTo2D());
                else
                    sqDist = Vec2d(myPos.truncateTo2D() - myaabb.getCenter()).norm();

                if (sqDist <= refDist)
                {
                    AnimLightComponent *pAnimLight = pAct->GetComponent<AnimLightComponent>();
                    if (pAnimLight)
                        pAnimLight->addLight(m_lightInfo);

                    ParticleGeneratorComponent *pParticleComp = pAct->GetComponent<ParticleGeneratorComponent>();
                    if (pParticleComp)
                        pParticleComp->addLight(m_lightInfo);

                    FxBankComponent *pFxBankComp = pAct->GetComponent<FxBankComponent>();
                    if (pFxBankComp)
                        pFxBankComp->addLight(m_lightInfo);
                }
            }
            else if (
                pObj && 
                pObj->getObjectType() == BaseObject::eFrise
                //&& depthRange.contains(pObj->getDepth())
                && m_useOnFrize
                )
            {
                Frise *frieze = (Frise*)pObj;

                //if (frieze->getRealTimeLightning() || !frieze->getLightComputed())
                {
                    AABB myaabb;
                    myaabb = frieze->getAABB();

                    f32 sqDist;
                    if (getUseBV())
                        sqDist = myaabb.getDistMinPosAABB(m_actor->getPos().truncateTo2D());
                    else
                        sqDist = Vec2d(myPos.truncateTo2D() - myaabb.getCenter()).norm();
                    if (sqDist <= refDist)
                    {
                        frieze->addLight(m_lightInfo);
                    }
                }
            }
        }
        else // Box
        {
            if (pObj && pObj->getObjectType() == BaseObject::eActor && m_useOnActor )
            {
                Actor* const pAct = static_cast<Actor *>(pObj);

                const GraphicComponent* const pGraphComponent = pAct->GetComponent<GraphicComponent>();
                const AABB & actorAABB = pGraphComponent ? pGraphComponent->getVisualAABB() : pAct->getAABB();

                if( actorAABB.checkOverlap(m_farAABB) )
                {
                    //simulate circle light from box light
                    const Vec2d & actorAABBCenter = actorAABB.getCenter();
                    const f32 range = getTemplate()->getBoxRange();
                    const Vec2d vecRange(range, range);
                    AABB nearAABB( m_farAABB.getMin() + vecRange, m_farAABB.getMax() - vecRange);
                    const f32 distance = nearAABB.getDistMinPosAABB( actorAABBCenter );
                        //compute fake position
                    Vec2d fake2DPosition = actorAABBCenter + (Vec2d::Right * distance);
                    m_lightInfo.m_position = Vec3d( fake2DPosition.m_x, fake2DPosition.m_y, m_actor->getDepth() );
                    if( distance > 0.0f )
                    {
                        //set fake rad near and far
                        m_lightInfo.m_radFar = range;
                        m_lightInfo.m_radNear = 0.0f;
                    }
                    else
                    {
                        m_lightInfo.m_radNear = 0.0f;
                        m_lightInfo.m_radFar = 1.0f;
                    }

                        //add info
                    AnimLightComponent *pAnimLight = pAct->GetComponent<AnimLightComponent>();
                    if (pAnimLight)
                    {
                        pAnimLight->addLight(m_lightInfo);
                    }
                    ParticleGeneratorComponent *pParticleComp = pAct->GetComponent<ParticleGeneratorComponent>();
                    if (pParticleComp)
                    {
                        pParticleComp->addLight(m_lightInfo);
                    }
                    FxBankComponent *pFxBankComp = pAct->GetComponent<FxBankComponent>();
                    if (pFxBankComp)
                    {
                        pFxBankComp->addLight(m_lightInfo);
                    }
                }
            }
            else if (pObj && pObj->getObjectType() == BaseObject::eFrise && m_useOnFrize)
            {
                Frise * const frieze = static_cast<Frise *>(pObj);
                //if (frieze->getRealTimeLightning() || !frieze->getLightComputed())
                {
                    const AABB & friezeAABB = frieze->getAABB();
                    if( friezeAABB.checkOverlap(m_farAABB) )
                    {                        
                        //simulate circle light from box light
                        const Vec2d & actorAABBCenter = friezeAABB.getCenter();
                        const f32 range = getTemplate()->getBoxRange();
                        const Vec2d vecRange(range, range);
                        AABB nearAABB( m_farAABB.getMin() + vecRange, m_farAABB.getMax() - vecRange);                        
                            //compute fake position
                        const f32 distance = nearAABB. getDistMinPosAABB( actorAABBCenter );
                        const Vec2d fake2DPosition = actorAABBCenter + (Vec2d::Right * distance);
                        m_lightInfo.m_position = Vec3d( fake2DPosition.m_x, fake2DPosition.m_y, m_actor->getDepth() );
                            //set fake rad near and far
                        m_lightInfo.m_radFar = range;
                        m_lightInfo.m_radNear = 0.0f;

                            //add info
                        frieze->addLight(m_lightInfo);
                    }
                }
            }
        }
    }

    void LightComponent::Draw()
    {
    }

#ifdef ITF_SUPPORT_EDITOR
    void LightComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        f32 rad = m_actor->getScale().m_x;
        if (m_actor->getScale().m_y > rad)
            rad = m_actor->getScale().m_y;

        const f32 radNear   = rad * getNear();
        const f32 radFar    = rad * getFar();

        const Vec2d myPos(m_actor->getPos().m_x, m_actor->getPos().m_y);
        if (m_actor->isSelected() && (m_shape == shape_circle))
        {
            // Draw circle with near, far and standard radius
            GFX_ADAPTER->drawDBGSphere(myPos.m_x, myPos.m_y, radNear, 1.f, 1.f, 0.f, 0.f, m_actor->getPos().m_z, 32, btrue, bfalse, bfalse, bfalse);
            GFX_ADAPTER->drawDBGSphere(myPos.m_x, myPos.m_y, radFar, 1.f, 1.f, 1.f, 0.f, m_actor->getPos().m_z, 32, btrue, bfalse, btrue, bfalse);
            GFX_ADAPTER->drawDBGSphere(myPos.m_x, myPos.m_y, rad, 0.5f, 0.2f, 0.2f, 0.f, m_actor->getPos().m_z, 32, btrue, bfalse, bfalse, bfalse);
        }
        if (m_actor->isSelected() && (m_shape == shape_box))
        {
            const f32 actorDepth = m_actor->getDepth();
            const f32 range = getTemplate()->getBoxRange();
            const Vec2d vecRange(range, range);
            AABB nearAABB( m_farAABB.getMin() + vecRange, m_farAABB.getMax() - vecRange);

            GFX_ADAPTER->drawDBGAABB( m_farAABB, 1.f, 1.f, 1.f, 0.0f, 1.f, actorDepth );
            GFX_ADAPTER->drawDBGAABB( nearAABB, 1.f, 1.f, 0.f, 0.0f, 1.f, actorDepth );
        }

    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(LightComponent_Template)
    BEGIN_SERIALIZATION(LightComponent_Template)

        SERIALIZE_MEMBER("near",m_near);
        SERIALIZE_MEMBER("far",m_far);
        SERIALIZE_MEMBER("shape",m_strShape);
        SERIALIZE_OBJECT("box", m_box);
        SERIALIZE_MEMBER("boxRange", m_boxRange);
        SERIALIZE_MEMBER("lightColor", m_lightColor);

    END_SERIALIZATION()

    LightComponent_Template::LightComponent_Template()
    : m_near(0.f)
    , m_far(1.f)
    , m_boxRange(0.f)
    , m_lightColor(Color::white())
    {
    }
}
